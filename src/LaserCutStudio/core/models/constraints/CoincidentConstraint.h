#ifndef COINCIDENTCONSTRAINT_H
#define COINCIDENTCONSTRAINT_H

#include "IConstraint.h"
#include "../geometry/IGeometricPoint.h"

namespace LaserCutStudio {
namespace Core {

// Forward declarations
class GeometricSegment;
class GeometricPoint;

/**
 * @brief Contrainte de coïncidence entre deux points
 *
 * Force deux points géométriques à occuper la même position en ajustant
 * la position du second point pour qu'il coïncide avec le premier.
 *
 * Cette contrainte est utile pour créer des connexions entre segments
 * ou pour maintenir des points alignés.
 */
class CoincidentConstraint : public IConstraint
{
    Q_OBJECT

    Q_PROPERTY(IGeometricPoint* point1 READ point1 WRITE setPoint1 NOTIFY point1Changed)
    Q_PROPERTY(IGeometricPoint* point2 READ point2 WRITE setPoint2 NOTIFY point2Changed)

public:
    DECLARE_TYPE_NAME(CoincidentConstraint)

    /**
     * @brief Constructeur
     * @param point1 Premier point (référence fixe)
     * @param point2 Second point (sera déplacé pour coïncider)
     * @param locked Si true, la contrainte a priorité maximale
     * @param parent Parent Qt (optionnel)
     */
    explicit CoincidentConstraint(IGeometricPoint* point1 = nullptr,
                                   IGeometricPoint* point2 = nullptr,
                                   bool locked = false,
                                   QObject* parent = nullptr);

    ~CoincidentConstraint() override;

    // Accesseurs
    IGeometricPoint* point1() const { return m_point1; }
    void setPoint1(IGeometricPoint* point);

    IGeometricPoint* point2() const { return m_point2; }
    void setPoint2(IGeometricPoint* point);

    // Implémentation IConstraint
    bool isValid() const;
    bool isSatisfied(double tolerance = 1e-6) const override;
    double error() const override;
    void apply() override;
    QList<IGeometricPoint*> affectedPoints() const override;

    // Clonage
    IConstraint* clone() const override;

signals:
    void point1Changed(IGeometricPoint* point);
    void point2Changed(IGeometricPoint* point);

private:
    void connectToPoint(IGeometricPoint* point);
    void disconnectFromPoint(IGeometricPoint* point);

    IGeometricPoint* m_point1;
    IGeometricPoint* m_point2;

    // Enregistrement automatique dans le Factory Pattern
    static const bool s_registered;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // COINCIDENTCONSTRAINT_H
