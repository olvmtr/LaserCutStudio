#include "ServiceLocator.h"

namespace LaserCutStudio {
namespace Core {
namespace DI {

ServiceLocator& ServiceLocator::instance()
{
    static ServiceLocator instance;
    return instance;
}

ServiceLocator::ServiceLocator()
{
    #if DEBUG
        qCDebug(logCore) << "ServiceLocator initialized";
    #endif
}

ServiceLocator::~ServiceLocator()
{
    clear();
}

void ServiceLocator::clear()
{
    #if DEBUG
        qCDebug(logCore) << "Clearing ServiceLocator...";
        qCDebug(logCore) << "  - Factories:" << m_factories.size();
        qCDebug(logCore) << "  - Instances:" << m_instances.size();
    #endif

    // Les instances seront détruites automatiquement car elles ont
    // le ServiceLocator comme parent (via setParent dans registerInstance/resolve)
    m_instances.clear();
    m_factories.clear();

    #if DEBUG
        qCDebug(logCore) << "ServiceLocator cleared";
    #endif
}

} // namespace DI
} // namespace Core
} // namespace LaserCutStudio
