#ifndef PART_H
#define PART_H

#include "core/models/parts/IPart.h"
#include "core/models/patterns/prototype/ClonableMixin.h"

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

    // Interface IPart implementation
    IPart* clone() const override;
    DECLARE_TYPE_NAME(Part)

    QString getName() const override;
    void setName(const QString& name) override;
    IShape* getShape() const override;
    void setShape(IShape* shape) override;
    double getThickness() const override;
    void setThickness(double thickness) override;
    Material getMaterial() const override;
    void setMaterial(const Material& material) override;
    void addJoint(IJoint* joint) override;
    void removeJoint(IJoint* joint) override;
    QList<IJoint*> getJoints() const override;
    double getVolume() const override;
    double getMass() const override;

private:
    QString m_name;
    IShape* m_shape;
    double m_thickness;
    Material m_material;
    QList<IJoint*> m_joints;

    // Auto-enregistrement dans le Factory Pattern
    static const bool s_registered;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // PART_H
