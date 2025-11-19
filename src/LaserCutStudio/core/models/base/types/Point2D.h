#ifndef POINT2D_H
#define POINT2D_H

#include <cmath>

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Représente un point en 2D
 */
class Point2D
{
public:
    /**
     * @brief Constructeur par défaut (origine)
     */
    Point2D() : x(0.0), y(0.0) {}

    /**
     * @brief Constructeur avec coordonnées
     * @param x Coordonnée X
     * @param y Coordonnée Y
     */
    Point2D(double x, double y) : x(x), y(y) {}

    /**
     * @brief Calcule la distance avec un autre point
     * @param other Autre point
     * @return Distance euclidienne
     */
    double distance(const Point2D& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    /**
     * @brief Opérateur d'égalité
     */
    bool operator==(const Point2D& other) const {
        const double epsilon = 1e-9;
        return std::abs(x - other.x) < epsilon && std::abs(y - other.y) < epsilon;
    }

    /**
     * @brief Opérateur d'addition
     */
    Point2D operator+(const Point2D& other) const {
        return Point2D(x + other.x, y + other.y);
    }

    /**
     * @brief Opérateur de soustraction
     */
    Point2D operator-(const Point2D& other) const {
        return Point2D(x - other.x, y - other.y);
    }

    /**
     * @brief Opérateur de multiplication par un scalaire
     */
    Point2D operator*(double scalar) const {
        return Point2D(x * scalar, y * scalar);
    }

    double x; ///< Coordonnée X
    double y; ///< Coordonnée Y
};

} // namespace Core
} // namespace LaserCutStudio

#endif // POINT2D_H
