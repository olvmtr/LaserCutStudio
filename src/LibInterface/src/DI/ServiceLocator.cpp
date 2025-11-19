#include "LibInterface/DI/ServiceLocator.h"

namespace LibInterface {

namespace DI {

ServiceLocator& ServiceLocator::instance()
{
    static ServiceLocator instance;
    return instance;
}

ServiceLocator::ServiceLocator()
{
    #if DEBUG
        qDebug() << "LibInterface::ServiceLocator initialized";
    #endif
}

ServiceLocator::~ServiceLocator()
{
    clear();
}

void ServiceLocator::clear()
{
    #if DEBUG
        qDebug() << "LibInterface::ServiceLocator: Clearing...";
        qDebug() << "  - Factories:" << m_factories.size();
        qDebug() << "  - Instances:" << m_instances.size();
    #endif

    // Les instances seront détruites automatiquement car elles ont
    // le ServiceLocator comme parent (via setParent dans registerInstance/resolve)
    m_instances.clear();
    m_factories.clear();

    #if DEBUG
        qDebug() << "LibInterface::ServiceLocator cleared";
    #endif
}

} // namespace DI

} // namespace LibInterface
