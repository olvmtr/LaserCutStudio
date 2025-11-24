#ifndef PARALLELCONSTRAINT_H
#define PARALLELCONSTRAINT_H

#include "IConstraint.h"
#include "../geometry/GeometricSegment.h"

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Contrainte de parallélisme entre deux segments
 *
 * Force deux segments à être parallèles en ajustant l'angle du second segment
 * pour qu'il corresponde à l'angle du premier segment.
 *
 * La contrainte agit sur les points d'extrémité non verrouillés des segments.
 */
class ParallelConstraint : public IConstraint
{
    Q_OBJECT

    Q_PROPERTY(GeometricSegment* segment1 READ segment1 WRITE setSegment1 NOTIFY segment1Changed)
    Q_PROPERTY(GeometricSegment* segment2 READ segment2 WRITE setSegment2 NOTIFY segment2Changed)

public:
    DECLARE_TYPE_NAME(ParallelConstraint)

    /**
     * @brief Constructeur
     * @param segment1 Premier segment (référence)
     * @param segment2 Second segment (sera ajusté pour être parallèle)
     * @param locked Si true, la contrainte a priorité maximale
     * @param parent Parent Qt (optionnel)
     */
    explicit ParallelConstraint(GeometricSegment* segment1 = nullptr,
                                GeometricSegment* segment2 = nullptr,
                                bool locked = false,
                                QObject* parent = nullptr);

    ~ParallelConstraint() override;

    // Accesseurs
    GeometricSegment* segment1() const { return m_segment1; }
    void setSegment1(GeometricSegment* segment);

    GeometricSegment* segment2() const { return m_segment2; }
    void setSegment2(GeometricSegment* segment);

    // Implémentation IConstraint
    bool isValid() const;
    bool isSatisfied(double tolerance = 1e-6) const override;
    double error() const override;
    void apply() override;
    QList<GeometricPoint*> affectedPoints() const override;

    // Clonage
    IConstraint* clone() const override;

signals:
    void segment1Changed(GeometricSegment* segment);
    void segment2Changed(GeometricSegment* segment);

private:
    void connectToSegment(GeometricSegment* segment);
    void disconnectFromSegment(GeometricSegment* segment);

    GeometricSegment* m_segment1;
    GeometricSegment* m_segment2;

    // Enregistrement automatique dans le Factory Pattern
    static const bool s_registered;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // PARALLELCONSTRAINT_H
