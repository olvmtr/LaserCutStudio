#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "core/models/shapes/interfaces/IShape.h"
#include "core/infrastructure/patterns/properties/PropertyMixin.h"

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Représente un rectangle
 */
class Rectangle : public IShape,
                  protected Patterns::PropertyMixin<Rectangle>
{
    Q_OBJECT

    Q_PROPERTY(double x READ getX WRITE setX NOTIFY xChanged)
    Q_PROPERTY(double y READ getY WRITE setY NOTIFY yChanged)
    Q_PROPERTY(double width READ getWidth WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(double height READ getHeight WRITE setHeight NOTIFY heightChanged)

    Q_CLASSINFO("Version", "1.0.0")
    Q_CLASSINFO("Category", "Shapes")
    Q_CLASSINFO("Description", "Rectangular shape")

signals:
    void xChanged(double newX);
    void yChanged(double newY);
    void widthChanged(double newWidth);
    void heightChanged(double newHeight);

public:
    /**
     * @brief Constructeur par défaut
     */
    Rectangle();

    /**
     * @brief Constructeur avec dimensions
     * @param x Position X du coin supérieur gauche
     * @param y Position Y du coin supérieur gauche
     * @param width Largeur
     * @param height Hauteur
     */
    Rectangle(double x, double y, double width, double height);

    /**
     * @brief Constructeur de copie
     */
    Rectangle(const Rectangle& other);

    /**
     * @brief Destructeur
     */
    ~Rectangle() override = default;

    /**
     * @brief Clone le rectangle
     */
    IShape* clone() const override;

    /**
     * @brief Déclare automatiquement staticTypeName() et getTypeName()
     */
    DECLARE_TYPE_NAME(Rectangle)

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
     * @brief Obtient les 4 coins du rectangle
     */
    QList<Point2D> getPoints() const override;

    /**
     * @brief Translate le rectangle
     */
    void translate(double dx, double dy) override;

    /**
     * @brief Fait pivoter le rectangle
     */
    void rotate(double angle, const Point2D& center) override;

    /**
     * @brief Met à l'échelle le rectangle
     */
    void scale(double scaleX, double scaleY, const Point2D& center) override;

    // Getters
    double getX() const { return m_x; }
    double getY() const { return m_y; }
    double getWidth() const { return m_width; }
    double getHeight() const { return m_height; }

    // Setters avec signaux
    /**
     * @brief Définit la position X
     * @param x Nouvelle coordonnée X du coin supérieur gauche
     */
    void setX(double x);

    /**
     * @brief Définit la position Y
     * @param y Nouvelle coordonnée Y du coin supérieur gauche
     */
    void setY(double y);

    /**
     * @brief Définit la largeur
     * @param width Nouvelle largeur (doit être > 0)
     */
    void setWidth(double width);

    /**
     * @brief Définit la hauteur
     * @param height Nouvelle hauteur (doit être > 0)
     */
    void setHeight(double height);

private:
    double m_x;      ///< Position X du coin supérieur gauche
    double m_y;      ///< Position Y du coin supérieur gauche
    double m_width;  ///< Largeur
    double m_height; ///< Hauteur

    // Auto-enregistrement dans le Factory Pattern
    static const bool s_registered;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // RECTANGLE_H
