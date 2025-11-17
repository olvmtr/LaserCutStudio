#ifndef FINGERJOINT_H
#define FINGERJOINT_H

#include "IJoint.h"

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Joint à doigts (finger joint / box joint)
 */
class FingerJoint : public IJoint
{
public:
    FingerJoint();
    FingerJoint(IPart* partA, IPart* partB, const Point3D& position, double angle,
                int fingerCount, double fingerWidth);
    FingerJoint(const FingerJoint& other);
    ~FingerJoint() override = default;

    IJoint* clone() const override;

    int getFingerCount() const { return m_fingerCount; }
    void setFingerCount(int count) { m_fingerCount = count; }

    double getFingerWidth() const { return m_fingerWidth; }
    void setFingerWidth(double width) { m_fingerWidth = width; }

private:
    int m_fingerCount;      ///< Nombre de doigts
    double m_fingerWidth;   ///< Largeur de chaque doigt
};

} // namespace Core
} // namespace LaserCutStudio

#endif // FINGERJOINT_H
