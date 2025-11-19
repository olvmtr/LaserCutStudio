#ifndef CIRCLE_H
#define CIRCLE_H

#include "IShape.h"
#include "../patterns/PropertyMixin.h"

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
    Circle();
    Circle(double centerX, double centerY, double radius);
    Circle(const Circle& other);
    ~Circle() override = default;

    IShape* clone() const override;
    DECLARE_TYPE_NAME(Circle)
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
    void setCenterX(double x);
    void setCenterY(double y);
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
