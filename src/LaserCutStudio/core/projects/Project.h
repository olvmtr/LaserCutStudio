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
public:
    Project();
    Project(const QString& name);
    Project(const Project& other);
    ~Project() override = default;

    /**
     * @brief Clone le projet
     */
    IProject* clone() const override;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // PROJECT_H
