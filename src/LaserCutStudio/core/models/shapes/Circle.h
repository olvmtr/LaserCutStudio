#ifndef CIRCLE_H
#define CIRCLE_H

#include "core/models/shapes/IShape.h"
#include "core/models/patterns/properties/PropertyMixin.h"

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Représente un cercle
 */
class Circle : public IShape,
               protected Patterns::PropertyMixin<Circle>
{
    Q_OBJECT

    Q_PROPERTY(double centerX READ getCenterX WRITE setCenterX NOTIFY centerXChanged)
    Q_PROPERTY(double centerY READ getCenterY WRITE setCenterY NOTIFY centerYChanged)
    Q_PROPERTY(double radius READ getRadius WRITE setRadius NOTIFY radiusChanged)

    Q_CLASSINFO("Version", "1.0.0")
    Q_CLASSINFO("Category", "Shapes")
    Q_CLASSINFO("Description", "Circular shape")

signals:
    void centerXChanged(double newX);
    void centerYChanged(double newY);
    void radiusChanged(double newRadius);

public:
    /**
     * @brief Constructeur par défaut
     *
     * Crée un cercle avec centre (0,0) et rayon 1.0
     */
    Circle();

    /**
     * @brief Constructeur avec paramètres
     * @param centerX Position X du centre du cercle
     * @param centerY Position Y du centre du cercle
     * @param radius Rayon du cercle (doit être > 0)
     */
    Circle(double centerX, double centerY, double radius);

    /**
     * @brief Constructeur de copie
     * @param other Cercle à copier
     */
    Circle(const Circle& other);

    ~Circle() override = default;

    /**
     * @brief Clone le cercle
     * @return Nouveau cercle identique alloué dynamiquement
     */
    IShape* clone() const override;

    DECLARE_TYPE_NAME(Circle)

    /**
     * @brief Calcule l'aire du cercle
     * @return Aire en unités carrées (π * r²)
     */
    double getArea() const override;

    /**
     * @brief Calcule le rectangle englobant
     * @return Rectangle minimal contenant le cercle
     */
    QRectF getBoundingBox() const override;

    /**
     * @brief Vérifie si un point est à l'intérieur du cercle
     * @param point Point à tester
     * @return true si distance au centre ≤ rayon
     */
    bool containsPoint(const Point2D& point) const override;

    /**
     * @brief Retourne des points d'échantillonnage du cercle
     * @return Liste de 36 points espacés de 10° sur le périmètre
     */
    QList<Point2D> getPoints() const override;

    /**
     * @brief Translate le cercle
     * @param dx Déplacement horizontal
     * @param dy Déplacement vertical
     */
    void translate(double dx, double dy) override;

    /**
     * @brief Rotation du cercle autour d'un point
     * @param angle Angle de rotation en degrés
     * @param center Centre de rotation
     * @note Pour un cercle, seul le centre est affecté
     */
    void rotate(double angle, const Point2D& center) override;

    /**
     * @brief Mise à l'échelle du cercle
     * @param scaleX Facteur d'échelle horizontal
     * @param scaleY Facteur d'échelle vertical
     * @param center Centre de la transformation
     * @note Utilise la moyenne des facteurs pour préserver la circularité
     */
    void scale(double scaleX, double scaleY, const Point2D& center) override;

    // Getters/Setters
    /**
     * @brief Obtient la position X du centre
     * @return Coordonnée X du centre
     */
    double getCenterX() const { return m_centerX; }

    /**
     * @brief Obtient la position Y du centre
     * @return Coordonnée Y du centre
     */
    double getCenterY() const { return m_centerY; }

    /**
     * @brief Obtient le rayon du cercle
     * @return Rayon en unités de longueur
     */
    double getRadius() const { return m_radius; }

    /**
     * @brief Définit la position X du centre
     * @param x Nouvelle coordonnée X
     */
    void setCenterX(double x);

    /**
     * @brief Définit la position Y du centre
     * @param y Nouvelle coordonnée Y
     */
    void setCenterY(double y);

    /**
     * @brief Définit le rayon du cercle
     * @param radius Nouveau rayon (doit être > 0)
     */
    void setRadius(double radius);

private:
    double m_centerX;
    double m_centerY;
    double m_radius;

    // Auto-enregistrement dans le Factory Pattern
    static const bool s_registered;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // CIRCLE_H
