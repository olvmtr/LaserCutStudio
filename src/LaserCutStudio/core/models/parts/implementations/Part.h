#ifndef PART_H
#define PART_H

#include "core/models/parts/interfaces/IPart.h"

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Implémentation concrète d'une pièce
 *
 * Les propriétés sont héritées de IPart (name, shape, thickness, material).
 * Utilise Q_PROPERTY via IPart pour l'introspection.
 */
class Part : public IPart
{
    Q_OBJECT

    Q_CLASSINFO("Version", "1.0.0")
    Q_CLASSINFO("Category", "Parts")
    Q_CLASSINFO("Description", "Basic part with shape, thickness and material")

public:
    Part();
    Part(const QString& name, IShape* shape, double thickness, const Material& material);
    Part(const Part& other);
    ~Part() override = default;

    /**
     * @brief Clone la pièce
     */
    IPart* clone() const override;

    /**
     * @brief Retourne le nom de type statique pour le Factory Pattern
     */
    static QString staticTypeName() { return "Part"; }

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

#endif // PART_H
