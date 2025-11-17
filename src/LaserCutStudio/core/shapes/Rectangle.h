#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "IShape.h"

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Représente un rectangle
 */
class Rectangle : public IShape
{
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

    // Setters
    void setX(double x) { m_x = x; }
    void setY(double y) { m_y = y; }
    void setWidth(double width) { m_width = width; }
    void setHeight(double height) { m_height = height; }

private:
    double m_x;      ///< Position X du coin supérieur gauche
    double m_y;      ///< Position Y du coin supérieur gauche
    double m_width;  ///< Largeur
    double m_height; ///< Hauteur
};

} // namespace Core
} // namespace LaserCutStudio

#endif // RECTANGLE_H
