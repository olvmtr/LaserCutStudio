#include "core/models/projects/IProject.h"
#include "core/models/parts/IPart.h"

namespace LaserCutStudio {
namespace Core {

IProject::IProject()
    : Interface()
{
    registerInstance(this);
}

IProject::IProject(const QString& name)
    : Interface()
{
    registerInstance(this);
}

IProject::IProject(const IProject& other)
    : Interface(other)
{
    registerInstance(this);
}

IProject::~IProject()
{
    unregisterInstance(this);
}

} // namespace Core
} // namespace LaserCutStudio
