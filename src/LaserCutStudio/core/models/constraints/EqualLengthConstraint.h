#ifndef EQUALLENGTHCONSTRAINT_H
#define EQUALLENGTHCONSTRAINT_H

#include "IConstraint.h"
#include "../geometry/IGeometricSegment.h"

namespace LaserCutStudio {
namespace Core {

// Forward declarations
class GeometricSegment;
class GeometricPoint;

/**
 * @brief Contrainte d'égalité de longueur entre deux segments
 *
 * Force deux segments à avoir la même longueur en ajustant la longueur
 * du second segment pour qu'elle corresponde à celle du premier segment.
 *
 * La contrainte agit sur les points d'extrémité non verrouillés des segments.
 */
class EqualLengthConstraint : public IConstraint
{
    Q_OBJECT

    Q_PROPERTY(IGeometricSegment* segment1 READ segment1 WRITE setSegment1 NOTIFY segment1Changed)
    Q_PROPERTY(IGeometricSegment* segment2 READ segment2 WRITE setSegment2 NOTIFY segment2Changed)

public:
    DECLARE_TYPE_NAME(EqualLengthConstraint)

    /**
     * @brief Constructeur
     * @param segment1 Premier segment (référence)
     * @param segment2 Second segment (sera ajusté pour avoir la même longueur)
     * @param locked Si true, la contrainte a priorité maximale
     * @param parent Parent Qt (optionnel)
     */
    explicit EqualLengthConstraint(IGeometricSegment* segment1 = nullptr,
                                   IGeometricSegment* segment2 = nullptr,
                                   bool locked = false,
                                   QObject* parent = nullptr);

    ~EqualLengthConstraint() override;

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

#endif // EQUALLENGTHCONSTRAINT_H
