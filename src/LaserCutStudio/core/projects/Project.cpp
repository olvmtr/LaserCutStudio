#include "Project.h"

namespace LaserCutStudio {
namespace Core {

Project::Project()
    : IProject()
{
}

Project::Project(const QString& name)
    : IProject(name)
{
}

Project::Project(const Project& other)
    : IProject(other)
{
}

IProject* Project::clone() const
{
    return new Project(*this);
}

// Auto-enregistrement dans le Factory Pattern
const bool Project::s_registered = IProject::registerFactory<Project>();

} // namespace Core
} // namespace LaserCutStudio
