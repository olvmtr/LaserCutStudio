#ifndef GEOMETRYUTILS_H
#define GEOMETRYUTILS_H

#include "../types/Point2D.h"
#include <cmath>

namespace LaserCutStudio {
namespace Core {
namespace Utils {

/**
 * @brief Matrice de rotation 2D précalculée
 *
 * Stocke les valeurs cos et sin d'un angle pour éviter de les recalculer
 * plusieurs fois lors de la rotation de plusieurs points.
 */
struct RotationMatrix
{
    double cos;  ///< Cosinus de l'angle
    double sin;  ///< Sinus de l'angle

    /**
     * @brief Crée une matrice de rotation depuis un angle en degrés
     *
     * @param angleDegrees Angle de rotation en degrés
     * @return Matrice de rotation avec cos et sin précalculés
     *
     * @note Les valeurs positives tournent dans le sens trigonométrique (anti-horaire)
     */
    static RotationMatrix fromDegrees(double angleDegrees)
    {
        double rad = angleDegrees * M_PI / 180.0;
        return {std::cos(rad), std::sin(rad)};
    }

    /**
     * @brief Crée une matrice de rotation depuis un angle en radians
     *
     * @param angleRadians Angle de rotation en radians
     * @return Matrice de rotation avec cos et sin précalculés
     */
    static RotationMatrix fromRadians(double angleRadians)
    {
        return {std::cos(angleRadians), std::sin(angleRadians)};
    }
};

/**
 * @brief Applique une rotation 2D à un point autour d'un centre
 *
 * Effectue la transformation : Translation → Rotation → Translation inverse
 * pour faire pivoter un point autour d'un centre arbitraire.
 *
 * @param point Point à faire pivoter
 * @param center Centre de rotation
 * @param rotation Matrice de rotation (obtenue via RotationMatrix::fromDegrees)
 * @return Nouveau point après rotation
 *
 * @note Utilise la matrice de rotation 2D standard :
 *       [ cos  -sin ] [ x ]
 *       [ sin   cos ] [ y ]
 */
inline Point2D rotatePoint(const Point2D& point, const Point2D& center,
                          const RotationMatrix& rotation)
{
    // Translation vers l'origine
    double tx = point.x - center.x;
    double ty = point.y - center.y;

    // Application de la rotation
    double rotatedX = tx * rotation.cos - ty * rotation.sin;
    double rotatedY = tx * rotation.sin + ty * rotation.cos;

    // Translation retour
    return Point2D(rotatedX + center.x, rotatedY + center.y);
}

/**
 * @brief Applique une mise à l'échelle 2D à un point depuis un centre
 *
 * Effectue la transformation : Translation → Scale → Translation inverse
 * pour mettre à l'échelle un point depuis un centre arbitraire.
 *
 * @param point Point à mettre à l'échelle
 * @param center Centre de la transformation
 * @param scaleX Facteur d'échelle horizontal (1.0 = pas de changement)
 * @param scaleY Facteur d'échelle vertical (1.0 = pas de changement)
 * @return Nouveau point après mise à l'échelle
 *
 * @note Les valeurs d'échelle peuvent être négatives (miroir)
 */
inline Point2D scalePoint(const Point2D& point, const Point2D& center,
                         double scaleX, double scaleY)
{
    // Translation vers l'origine
    double tx = point.x - center.x;
    double ty = point.y - center.y;

    // Application de l'échelle
    tx *= scaleX;
    ty *= scaleY;

    // Translation retour
    return Point2D(tx + center.x, ty + center.y);
}

/**
 * @brief Convertit des degrés en radians
 *
 * @param degrees Angle en degrés
 * @return Angle en radians
 */
inline double degreesToRadians(double degrees)
{
    return degrees * M_PI / 180.0;
}

/**
 * @brief Convertit des radians en degrés
 *
 * @param radians Angle en radians
 * @return Angle en degrés
 */
inline double radiansToDegrees(double radians)
{
    return radians * 180.0 / M_PI;
}

} // namespace Utils
} // namespace Core
} // namespace LaserCutStudio

#endif // GEOMETRYUTILS_H
