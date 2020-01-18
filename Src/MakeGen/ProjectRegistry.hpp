#pragma once
#include <vector>
#include "MakeGen/Def.hpp"

class Project;

// Singleton class which registers each project. The class just acts
// as a holder and does nothing intelligent. Note that the dependency
// list may have projects not in the project list: this needs to be
// tested for.
class ProjectRegistry {
public:
	// Singleton instance
	static ProjectRegistry& instance() {
		static ProjectRegistry x;
		return x;
	}

	// Add a live project - called from Project constructor.
	void addLiveProject(Project* project) {
		liveProjects_.push_back(project);
	}

	// Add a dead project - called from Project constructor.
	void addDeadProject(Project* project) {
		deadProjects_.push_back(project);
	}

	// Add a dependency - called from a Project derived class constructor.
	void addDependency(Project* project, const std::string& dependency) {
		dependencies_.push_back(std::make_pair(project, dependency));
	}

	// Get the build order. If incrementalBuild is true then only out of
	// date projects are included.
	std::vector<Project*> buildOrder(bool incrementalBuild) const;

private:

	ProjectRegistry() :
		liveProjects_(),
		deadProjects_(),
		dependencies_()
	{
	}

private:
	// List of registered projects which are to be made if necessary.
	std::vector<Project*> liveProjects_;

	// List of registered projects which are to be ignored.
	std::vector<Project*> deadProjects_;

	// List of dependencies.
	typedef std::pair<Project*, std::string> Dependency;
	std::vector<Dependency> dependencies_;
};
