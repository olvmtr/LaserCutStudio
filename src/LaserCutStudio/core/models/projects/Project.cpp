#include "core/models/projects/Project.h"
#include "core/models/parts/IPart.h"
#include <QDateTime>

namespace LaserCutStudio {
namespace Core {

Project::Project()
    : IProject()
    , m_name("Unnamed Project")
    , m_metadata()
    , m_parts()
{
    m_metadata.creationDate = QDateTime::currentDateTime();
    m_metadata.lastModifiedDate = m_metadata.creationDate;
}

Project::Project(const QString& name)
    : IProject(name)
    , m_name(name)
    , m_metadata()
    , m_parts()
{
    m_metadata.creationDate = QDateTime::currentDateTime();
    m_metadata.lastModifiedDate = m_metadata.creationDate;
}

Project::Project(const Project& other)
    : IProject(other)
    , m_name(other.m_name)
    , m_metadata(other.m_metadata)
    , m_parts()
{
    // Clone all parts
    for (IPart* part : other.m_parts) {
        if (part) {
            m_parts.append(part->clone());
        }
    }
}

IMPLEMENT_CLONE(Project, IProject)

QString Project::getName() const
{
    return m_name;
}

void Project::setName(const QString& name)
{
    m_name = name;
    m_metadata.lastModifiedDate = QDateTime::currentDateTime();
}

ProjectMetadata Project::getMetadata() const
{
    return m_metadata;
}

void Project::setMetadata(const ProjectMetadata& metadata)
{
    m_metadata = metadata;
}

void Project::addPart(IPart* part)
{
    if (part && !m_parts.contains(part)) {
        m_parts.append(part);
        m_metadata.lastModifiedDate = QDateTime::currentDateTime();
    }
}

void Project::removePart(IPart* part)
{
    if (m_parts.removeAll(part) > 0) {
        m_metadata.lastModifiedDate = QDateTime::currentDateTime();
    }
}

QList<IPart*> Project::getParts() const
{
    return m_parts;
}

bool Project::isEmpty() const
{
    return m_parts.isEmpty();
}

int Project::getPartCount() const
{
    return m_parts.count();
}

double Project::getTotalVolume() const
{
    double totalVolume = 0.0;
    for (const IPart* part : m_parts) {
        if (part) {
            totalVolume += part->getVolume();
        }
    }
    return totalVolume;
}

double Project::getTotalMass() const
{
    double totalMass = 0.0;
    for (const IPart* part : m_parts) {
        if (part) {
            totalMass += part->getMass();
        }
    }
    return totalMass;
}

bool Project::save(const QString& filePath) const
{
    // TODO: Implement saving (will be done in IO module)
    Q_UNUSED(filePath);
    return false;
}

bool Project::load(const QString& filePath)
{
    // TODO: Implement loading (will be done in IO module)
    Q_UNUSED(filePath);
    return false;
}

// Auto-enregistrement dans le Factory Pattern
const bool Project::s_registered = IProject::registerFactory<Project>();

} // namespace Core
} // namespace LaserCutStudio
