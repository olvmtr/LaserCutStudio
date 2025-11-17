#ifndef CIRCLE_H
#define CIRCLE_H

#include "IShape.h"

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Représente un cercle
 */
class Circle : public IShape
{
public:
    Circle();
    Circle(double centerX, double centerY, double radius);
    Circle(const Circle& other);
    ~Circle() override = default;

    IShape* clone() const override;
    double getArea() const override;
    QRectF getBoundingBox() const override;
    bool containsPoint(const Point2D& point) const override;
    QList<Point2D> getPoints() const override;
    void translate(double dx, double dy) override;
    void rotate(double angle, const Point2D& center) override;
    void scale(double scaleX, double scaleY, const Point2D& center) override;

    // Getters/Setters
    double getCenterX() const { return m_centerX; }
    double getCenterY() const { return m_centerY; }
    double getRadius() const { return m_radius; }
    void setCenterX(double x) { m_centerX = x; }
    void setCenterY(double y) { m_centerY = y; }
    void setRadius(double radius) { m_radius = radius; }

private:
    double m_centerX;
    double m_centerY;
    double m_radius;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // CIRCLE_H
