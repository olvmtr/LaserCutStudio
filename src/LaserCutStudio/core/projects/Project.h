#ifndef PROJECT_H
#define PROJECT_H

#include "IProject.h"

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

    /**
     * @brief Clone le projet
     */
    IProject* clone() const override;

    /**
     * @brief Retourne le nom de type statique pour le Factory Pattern
     */
    static QString staticTypeName() { return "Project"; }

    /**
     * @brief Retourne le nom de type pour l'instance (Factory Pattern)
     */
    QString getTypeName() const override { return staticTypeName(); }

private:
    // Auto-enregistrement dans le Factory Pattern
    static const bool s_registered;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // PROJECT_H
