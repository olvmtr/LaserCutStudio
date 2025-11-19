#include "core/models/projects/interfaces/IProject.h"

namespace LaserCutStudio {
namespace Core {

// Note: s_factories et s_instances sont maintenant dans FactoryMixin et ListManagerMixin

IProject::IProject()
    : Interface()
    , m_name("Unnamed Project")
    , m_metadata()
{
    registerInstance(this);
}

IProject::IProject(const QString& name)
    : Interface()
    , m_name(name)
    , m_metadata()
{
    registerInstance(this);
}

IProject::IProject(const IProject& other)
    : Interface(other)
    , m_name(other.m_name)
    , m_metadata(other.m_metadata)
{
    // Clone toutes les pièces
    for (IPart* part : other.m_parts) {
        if (part) {
            m_parts.append(part->clone());
        }
    }

    registerInstance(this);
}

IProject::~IProject()
{
    unregisterInstance(this);
}

void IProject::addPart(IPart* part)
{
    if (part && !m_parts.contains(part)) {
        m_parts.append(part);
        m_metadata.lastModifiedDate = QDateTime::currentDateTime();
    }
}

void IProject::removePart(IPart* part)
{
    if (m_parts.removeAll(part) > 0) {
        m_metadata.lastModifiedDate = QDateTime::currentDateTime();
    }
}

double IProject::getTotalVolume() const
{
    double totalVolume = 0.0;
    for (const IPart* part : m_parts) {
        if (part) {
            totalVolume += part->getVolume();
        }
    }
    return totalVolume;
}

double IProject::getTotalMass() const
{
    double totalMass = 0.0;
    for (const IPart* part : m_parts) {
        if (part) {
            totalMass += part->getMass();
        }
    }
    return totalMass;
}

bool IProject::save(const QString& filePath) const
{
    // TODO: Implémenter la sauvegarde (sera fait dans le module IO)
    Q_UNUSED(filePath);
    return false;
}

bool IProject::load(const QString& filePath)
{
    // TODO: Implémenter le chargement (sera fait dans le module IO)
    Q_UNUSED(filePath);
    return false;
}

// ===== Factory Pattern =====
// Note: create(), availableTypes() et toVariant() sont maintenant fournis par FactoryMixin et Interface
// Note: Gestion de liste (add/remove/clear) maintenant fournie par ListManagerMixin

} // namespace Core
} // namespace LaserCutStudio
