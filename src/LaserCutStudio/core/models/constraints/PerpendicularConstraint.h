#ifndef PERPENDICULARCONSTRAINT_H
#define PERPENDICULARCONSTRAINT_H

#include "IConstraint.h"
#include "../geometry/IGeometricSegment.h"

namespace LaserCutStudio {
namespace Core {

// Forward declarations
class GeometricSegment;
class GeometricPoint;

/**
 * @brief Contrainte de perpendicularité entre deux segments
 *
 * Force deux segments à être perpendiculaires (angle de 90°) en ajustant
 * l'angle du second segment pour qu'il soit à 90° du premier segment.
 *
 * La contrainte agit sur les points d'extrémité non verrouillés des segments.
 */
class PerpendicularConstraint : public IConstraint
{
    Q_OBJECT

    Q_PROPERTY(IGeometricSegment* segment1 READ segment1 WRITE setSegment1 NOTIFY segment1Changed)
    Q_PROPERTY(IGeometricSegment* segment2 READ segment2 WRITE setSegment2 NOTIFY segment2Changed)

public:
    DECLARE_TYPE_NAME(PerpendicularConstraint)

    /**
     * @brief Constructeur
     * @param segment1 Premier segment (référence)
     * @param segment2 Second segment (sera ajusté pour être perpendiculaire)
     * @param locked Si true, la contrainte a priorité maximale
     * @param parent Parent Qt (optionnel)
     */
    explicit PerpendicularConstraint(IGeometricSegment* segment1 = nullptr,
                                     IGeometricSegment* segment2 = nullptr,
                                     bool locked = false,
                                     QObject* parent = nullptr);

    ~PerpendicularConstraint() override;

    // Accesseurs
    IGeometricSegment* segment1() const { return m_segment1; }
    void setSegment1(IGeometricSegment* segment);

    IGeometricSegment* segment2() const { return m_segment2; }
    void setSegment2(IGeometricSegment* segment);

    // Implémentation IConstraint
    bool isValid() const;
    bool isSatisfied(double tolerance = 1e-6) const override;
    double error() const override;
    void apply() override;
    QList<GeometricPoint*> affectedPoints() const override;

    // Clonage
    IConstraint* clone() const override;

signals:
    void segment1Changed(IGeometricSegment* segment);
    void segment2Changed(IGeometricSegment* segment);

private:
    void connectToSegment(IGeometricSegment* segment);
    void disconnectFromSegment(IGeometricSegment* segment);

    IGeometricSegment* m_segment1;
    IGeometricSegment* m_segment2;

    // Enregistrement automatique dans le Factory Pattern
    static const bool s_registered;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // PERPENDICULARCONSTRAINT_H
