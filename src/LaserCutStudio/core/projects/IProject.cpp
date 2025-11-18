#include "IProject.h"

namespace LaserCutStudio {
namespace Core {

// Initialisation de la liste statique
QList<IProject*> IProject::s_projects;
// Note: s_factories est maintenant dans FactoryMixin et initialisé automatiquement

IProject::IProject()
    : Interface()
    , m_name("Unnamed Project")
    , m_metadata()
{
    addProject(this);
}

IProject::IProject(const QString& name)
    : Interface()
    , m_name(name)
    , m_metadata()
{
    addProject(this);
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

    addProject(this);
}

IProject::~IProject()
{
    removeProject(this);
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

void IProject::addProject(IProject* project)
{
    if (project && !s_projects.contains(project)) {
        s_projects.append(project);
    }
}

void IProject::removeProject(IProject* project)
{
    s_projects.removeAll(project);
}

void IProject::clearAllProjects()
{
    s_projects.clear();
}

// ===== Factory Pattern =====
// Note: create(), availableTypes() et toVariant() sont maintenant fournis par FactoryMixin et Interface

} // namespace Core
} // namespace LaserCutStudio
