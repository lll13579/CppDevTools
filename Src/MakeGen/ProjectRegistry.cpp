#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "MakeGen/Dir.hpp"
#include "MakeGen/File.hpp"
#include "MakeGen/Project.hpp"
#include "MakeGen/ProjectRegistry.hpp"
#include "MakeGen/ResourcePath.hpp"

struct ProjectInfo {
	ProjectInfo(Project* project_, bool isLiveNotDead_) :
		project(project_),
		isLiveNotDead(isLiveNotDead_),
		makeSuccessTime(),
		inBuildOrder(false),
		needsMaking(false)
	{
	}
	Project* project;
	bool isLiveNotDead;
	std::set<const ProjectInfo*> dependencies;
	fs::file_time_type makeSuccessTime;
	bool inBuildOrder;
	bool needsMaking;
};

std::vector<Project*> ProjectRegistry::buildOrder(bool incrementalBuild) const {
	// First get a mapping from a project name to its information.
	typedef std::map<std::string, ProjectInfo> ProjectMapType;
	ProjectMapType map;
	for (const auto& p : liveProjects_) {
		const auto& pr = map.insert(std::make_pair(p->name(), ProjectInfo(p, true)));
		ASSERT(pr.second);
	}
	for (const auto& p : deadProjects_) {
		const auto& pr = map.insert(std::make_pair(p->name(), ProjectInfo(p, false)));
		ASSERT(pr.second);
	}

	// Add in the dependencies. A dependency can only be on a
	// live project.
	for (const auto& d : dependencies_) {
		auto it = map.find(d.first->name());
		ASSERT(it != map.end());
		ProjectInfo& pi = it->second;
		ASSERT(pi.project == d.first);
		ASSERT(pi.isLiveNotDead);

		auto jt = map.find(d.second);
		ASSERT(jt != map.end());
		ProjectInfo& pj = jt->second;
		ASSERT(pj.isLiveNotDead);
		ASSERT(pi.dependencies.insert(&pj).second);
	}

	// Now everything we need is stored in map and the member variables
	// are no longer needed.

	// Get a list of actual project directories and check that all of them
	// have been registered.
	{
		DirList projectDirList = Dir::SRC().allSubdirs();
		std::set<std::string> actualProjectDirs;
		for (const auto& d : projectDirList) {
			ASSERT(actualProjectDirs.insert(d.name()).second);
		}
		std::set<std::string> regProjectDirs;
		for (const auto& m : map) {
			ASSERT(regProjectDirs.insert(m.first).second);
		}
		ASSERT(actualProjectDirs == regProjectDirs);
	}

	// Delete all the dead projects as they are not of interest.
	// Also set the make success time for the remainder.
	for (ProjectMapType::iterator it = map.begin(); it != map.end(); ) {
		ProjectInfo& pi = it->second;
		if (pi.isLiveNotDead) {
			File makeSuccessFile(Dir::BUILD(pi.project->name()), MAKE_SUCCESS_TXT);
			pi.makeSuccessTime = makeSuccessFile.lastWriteTime();
			++it;
		}
		else {
			it = map.erase(it);
		}
	}

	// Determine the build order. This does not take into account whether
	// a project needs to be built - it is the full list for when everything
	// needs building. A project gets added to the build order once all of
	// its dependencies are also in the build order.
	std::vector<ProjectInfo*> buildOrder;
	for (;;) {
		std::cout << "Starting pass" << std::endl;
		bool changeMade = false;
		for (auto& m : map) {
			ProjectInfo& pi = m.second;
			if (pi.inBuildOrder) {
				std::cout << "= " << pi.project->name() << std::endl;
				continue;
			}
			bool canBuild = true;
			for (const auto& d : pi.dependencies) {
				if (!d->inBuildOrder) {
					canBuild = false;
					break;
				}
			}
			if (canBuild) {
				std::cout << "+ " << pi.project->name() << std::endl;
				buildOrder.push_back(&pi);
				pi.inBuildOrder = true;
				changeMade = true;
			}
			else {
				std::cout << ". " << pi.project->name() << std::endl;
			}
		}
		if (buildOrder.size() == map.size()) {
			break;
		}
		ASSERT(buildOrder.size() < map.size());
		ASSERT(changeMade);
	}

	// Scan the build order and construct a list of projects which need to
	// be made. A project needs to be made if any of the following is true:
	// (1) It is a full build.
	// (2) Any of its dependencies needs to be made.
	// (3) The make success file for a dependency is younger than the make
	//     success file for the project.
	// (4) Any of the project source directory files are younger than
	//     the make success file for the project.
	std::vector<Project*> ret;
	for (auto pi : buildOrder) {
		if (incrementalBuild) {
			for (const auto& d : pi->dependencies) {
				if (d->needsMaking) {
					pi->needsMaking = true;
					std::cout << pi->project->name() << " needs making because dependent " << d->project->name() << " needs making" << std::endl;
					break;
				}
				if (d->makeSuccessTime > pi->makeSuccessTime) {
					pi->needsMaking = true;
					std::cout << pi->project->name() << " needs making because dependent " << d->project->name() << " success time is later" << std::endl;
					break;
				}
			}
			if (!pi->needsMaking) {
				const FileList& allSrcFiles = pi->project->allSrcFiles();
				for (const auto& f : allSrcFiles) {
					// Ignore file which have extensions only used by Visual Studio.
					std::string ext = f.ext();
					if ((ext == "props") ||
						(ext == "sln") ||
						(ext == "vcxproj") ||
						(ext == "vcxproj.filters"))
					{
						continue;
					}

					if (f.lastWriteTime() > pi->makeSuccessTime) {
						std::cout << pi->project->name() << " needs making because file " << f.str() << " is later than success time" << std::endl;
						pi->needsMaking = true;
						break;
					}
				}
			}
			if (!pi->needsMaking) {
				std::cout << pi->project->name() << " does not need making" << std::endl;
				continue;
			}
		}
		ret.push_back(pi->project);
	}

	return ret;
}
