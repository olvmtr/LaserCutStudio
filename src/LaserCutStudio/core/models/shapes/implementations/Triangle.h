#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "core/models/shapes/interfaces/IShape.h"
#include "core/infrastructure/patterns/properties/PropertyMixin.h"

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Représente un triangle équilatéral
 */
class Triangle : public IShape,
                 protected Patterns::PropertyMixin<Triangle>
{
    Q_OBJECT

    Q_PROPERTY(double centerX READ getCenterX WRITE setCenterX NOTIFY centerXChanged)
    Q_PROPERTY(double centerY READ getCenterY WRITE setCenterY NOTIFY centerYChanged)
    Q_PROPERTY(double size READ getSize WRITE setSize NOTIFY sizeChanged)

    Q_CLASSINFO("Version", "1.0.0")
    Q_CLASSINFO("Category", "Shapes")
    Q_CLASSINFO("Description", "Equilateral triangle shape")

signals:
    void centerXChanged(double newCenterX);
    void centerYChanged(double newCenterY);
    void sizeChanged(double newSize);

public:
    /**
     * @brief Constructeur par défaut
     */
    Triangle();

    /**
     * @brief Constructeur avec dimensions
     * @param centerX Position X du centre
     * @param centerY Position Y du centre
     * @param size Longueur du côté
     */
    Triangle(double centerX, double centerY, double size);

    /**
     * @brief Constructeur de copie
     */
    Triangle(const Triangle& other);

    /**
     * @brief Destructeur
     */
    ~Triangle() override = default;

    /**
     * @brief Clone le triangle
     */
    IShape* clone() const override;

    /**
     * @brief Déclare automatiquement staticTypeName() et getTypeName()
     */
    DECLARE_TYPE_NAME(Triangle)

    /**
     * @brief Calcule l'aire
     */
    double getArea() const override;

    /**
     * @brief Calcule le rectangle englobant
     */
    QRectF getBoundingBox() const override;

    /**
     * @brief Vérifie si un point est à l'intérieur
     */
    bool containsPoint(const Point2D& point) const override;

    /**
     * @brief Obtient les 3 sommets du triangle
     */
    QList<Point2D> getPoints() const override;

    /**
     * @brief Translate le triangle
     */
    void translate(double dx, double dy) override;

    /**
     * @brief Fait pivoter le triangle
     */
    void rotate(double angle, const Point2D& center) override;

    /**
     * @brief Met à l'échelle le triangle
     */
    void scale(double factorX, double factorY, const Point2D& center) override;

    // Getters
    double getCenterX() const { return m_centerX; }
    double getCenterY() const { return m_centerY; }
    double getSize() const { return m_size; }

    // Setters
    /**
     * @brief Définit la position X du centre
     * @param centerX Nouvelle coordonnée X du centre
     */
    void setCenterX(double centerX);

    /**
     * @brief Définit la position Y du centre
     * @param centerY Nouvelle coordonnée Y du centre
     */
    void setCenterY(double centerY);

    /**
     * @brief Définit la longueur du côté
     * @param size Nouvelle longueur du côté (doit être > 0)
     */
    void setSize(double size);

private:
    double m_centerX = 0.0;
    double m_centerY = 0.0;
    double m_size = 0.0;  // Longueur du côté

    // Enregistrement automatique dans le Factory
    static const bool s_registered;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // TRIANGLE_H
