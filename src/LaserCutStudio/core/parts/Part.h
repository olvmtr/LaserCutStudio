#ifndef PART_H
#define PART_H

#include "IPart.h"

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Implémentation concrète d'une pièce
 */
class Part : public IPart
{
public:
    Part();
    Part(const QString& name, IShape* shape, double thickness, const Material& material);
    Part(const Part& other);
    ~Part() override = default;

    /**
     * @brief Clone la pièce
     */
    IPart* clone() const override;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // PART_H
