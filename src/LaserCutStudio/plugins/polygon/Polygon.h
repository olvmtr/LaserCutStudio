#ifndef POLYGON_H
#define POLYGON_H

#include "../../core/shapes/IShape.h"
#include "../../core/types/Point2D.h"
#include <QVector>
#include <cmath>

namespace LaserCutStudio {
namespace Plugins {

/**
 * @class Polygon
 * @brief Polygone régulier à N côtés
 *
 * Représente un polygone régulier centré avec un rayon donné.
 */
class Polygon : public Core::IShape
{
    Q_OBJECT

public:
    /**
     * @brief Constructeur de polygone
     *
     * @param centerX Position X du centre
     * @param centerY Position Y du centre
     * @param radius Rayon (distance centre-sommet)
     * @param sides Nombre de côtés (>= 3)
     * @param parent Objet parent Qt
     */
    Polygon(double centerX, double centerY, double radius, int sides,
            QObject* parent = nullptr)
        : IShape("Polygon", parent)
        , m_centerX(centerX)
        , m_centerY(centerY)
        , m_radius(radius)
        , m_sides(sides)
    {
        Q_ASSERT(sides >= 3);
        Q_ASSERT(radius > 0.0);
        computeVertices();
    }

    ~Polygon() override = default;

    // Implémentation IShape
    double getArea() const override
    {
        // Aire = (1/2) * périmètre * apothème
        // Pour polygone régulier: A = (n * r^2 * sin(2π/n)) / 2
        double angle = 2.0 * M_PI / m_sides;
        return (m_sides * m_radius * m_radius * std::sin(angle)) / 2.0;
    }

    double getPerimeter() const override
    {
        // Périmètre = n * longueur_côté
        // longueur_côté = 2 * r * sin(π/n)
        double sideLength = 2.0 * m_radius * std::sin(M_PI / m_sides);
        return m_sides * sideLength;
    }

    QRectF getBoundingBox() const override
    {
        // Calculer la boîte englobante à partir des sommets
        if (m_vertices.isEmpty()) {
            return QRectF();
        }

        double minX = m_vertices[0].x();
        double maxX = m_vertices[0].x();
        double minY = m_vertices[0].y();
        double maxY = m_vertices[0].y();

        for (const auto& vertex : m_vertices) {
            minX = std::min(minX, vertex.x());
            maxX = std::max(maxX, vertex.x());
            minY = std::min(minY, vertex.y());
            maxY = std::max(maxY, vertex.y());
        }

        return QRectF(minX, minY, maxX - minX, maxY - minY);
    }

    Interface* clone() const override
    {
        return new Polygon(m_centerX, m_centerY, m_radius, m_sides);
    }

    QVariant toVariant() const override
    {
        QVariantMap map = IShape::toVariant().toMap();
        map["centerX"] = m_centerX;
        map["centerY"] = m_centerY;
        map["radius"] = m_radius;
        map["sides"] = m_sides;
        return map;
    }

    // Méthodes spécifiques
    double centerX() const { return m_centerX; }
    double centerY() const { return m_centerY; }
    double radius() const { return m_radius; }
    int sides() const { return m_sides; }

    /**
     * @brief Retourne les sommets du polygone
     */
    QVector<Core::Point2D> vertices() const { return m_vertices; }

protected:
    void move_internal(double dx, double dy) override
    {
        m_centerX += dx;
        m_centerY += dy;
        computeVertices();
    }

    void rotate_internal(double angle, const Core::Point2D& center) override
    {
        // Rotation du centre autour du point
        double cos_a = std::cos(angle * M_PI / 180.0);
        double sin_a = std::sin(angle * M_PI / 180.0);

        double dx = m_centerX - center.x();
        double dy = m_centerY - center.y();

        m_centerX = center.x() + dx * cos_a - dy * sin_a;
        m_centerY = center.y() + dx * sin_a + dy * cos_a;

        // Note: Pour un polygone régulier, on pourrait aussi
        // faire tourner tous les sommets individuellement
        computeVertices();
    }

    void scale_internal(double sx, double sy, const Core::Point2D& center) override
    {
        // Mettre à l'échelle le centre et le rayon
        double dx = m_centerX - center.x();
        double dy = m_centerY - center.y();

        m_centerX = center.x() + dx * sx;
        m_centerY = center.y() + dy * sy;

        // Note: Pour simplifier, on utilise sx pour le rayon
        // Un vrai polygone non-régulier nécessiterait sy aussi
        m_radius *= sx;

        computeVertices();
    }

private:
    /**
     * @brief Calcule les coordonnées des sommets
     */
    void computeVertices()
    {
        m_vertices.clear();
        m_vertices.reserve(m_sides);

        double angleStep = 2.0 * M_PI / m_sides;
        double startAngle = -M_PI / 2.0;  // Commencer en haut

        for (int i = 0; i < m_sides; ++i) {
            double angle = startAngle + i * angleStep;
            double x = m_centerX + m_radius * std::cos(angle);
            double y = m_centerY + m_radius * std::sin(angle);
            m_vertices.append(Core::Point2D(x, y));
        }
    }

    double m_centerX;
    double m_centerY;
    double m_radius;
    int m_sides;
    QVector<Core::Point2D> m_vertices;
};

} // namespace Plugins
} // namespace LaserCutStudio

#endif // POLYGON_H
