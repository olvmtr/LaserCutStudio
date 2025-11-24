#ifndef PROJECT_H
#define PROJECT_H

#include "core/models/projects/IProject.h"
#include "core/models/patterns/prototype/ClonableMixin.h"

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Implémentation concrète d'un projet
 */
class Project : public IProject
{
    Q_OBJECT

    Q_CLASSINFO("Version", "1.0.0")
    Q_CLASSINFO("Category", "Projects")
    Q_CLASSINFO("Description", "Complete laser cutting project with parts and metadata")

public:
    Project();
    Project(const QString& name);
    Project(const Project& other);
    ~Project() override = default;

    // Interface IProject implementation
    IProject* clone() const override;
    DECLARE_TYPE_NAME(Project)

    QString getName() const override;
    void setName(const QString& name) override;
    ProjectMetadata getMetadata() const override;
    void setMetadata(const ProjectMetadata& metadata) override;
    void addPart(IPart* part) override;
    void removePart(IPart* part) override;
    QList<IPart*> getParts() const override;
    bool isEmpty() const override;
    int getPartCount() const override;
    double getTotalVolume() const override;
    double getTotalMass() const override;
    bool save(const QString& filePath) const override;
    bool load(const QString& filePath) override;

private:
    QString m_name;
    ProjectMetadata m_metadata;
    QList<IPart*> m_parts;

    // Auto-enregistrement dans le Factory Pattern
    static const bool s_registered;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // PROJECT_H
