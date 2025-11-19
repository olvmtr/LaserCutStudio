#ifndef POINT3D_H
#define POINT3D_H

#include <cmath>

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Représente un point en 3D
 */
class Point3D
{
public:
    /**
     * @brief Constructeur par défaut (origine)
     */
    Point3D() : x(0.0), y(0.0), z(0.0) {}

    /**
     * @brief Constructeur avec coordonnées
     * @param x Coordonnée X
     * @param y Coordonnée Y
     * @param z Coordonnée Z
     */
    Point3D(double x, double y, double z) : x(x), y(y), z(z) {}

    /**
     * @brief Calcule la distance avec un autre point
     * @param other Autre point
     * @return Distance euclidienne
     */
    double distance(const Point3D& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        double dz = z - other.z;
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }

    /**
     * @brief Opérateur d'égalité
     */
    bool operator==(const Point3D& other) const {
        const double epsilon = 1e-9;
        return std::abs(x - other.x) < epsilon &&
               std::abs(y - other.y) < epsilon &&
               std::abs(z - other.z) < epsilon;
    }

    /**
     * @brief Opérateur d'addition
     */
    Point3D operator+(const Point3D& other) const {
        return Point3D(x + other.x, y + other.y, z + other.z);
    }

    /**
     * @brief Opérateur de soustraction
     */
    Point3D operator-(const Point3D& other) const {
        return Point3D(x - other.x, y - other.y, z - other.z);
    }

    /**
     * @brief Opérateur de multiplication par un scalaire
     */
    Point3D operator*(double scalar) const {
        return Point3D(x * scalar, y * scalar, z * scalar);
    }

    double x; ///< Coordonnée X
    double y; ///< Coordonnée Y
    double z; ///< Coordonnée Z
};

} // namespace Core
} // namespace LaserCutStudio

#endif // POINT3D_H
