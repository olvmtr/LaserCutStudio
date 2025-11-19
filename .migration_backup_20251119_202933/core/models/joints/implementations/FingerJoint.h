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
    Q_OBJECT

    Q_PROPERTY(int fingerCount READ getFingerCount WRITE setFingerCount NOTIFY fingerCountChanged)
    Q_PROPERTY(double fingerWidth READ getFingerWidth WRITE setFingerWidth NOTIFY fingerWidthChanged)

    Q_CLASSINFO("Version", "1.0.0")
    Q_CLASSINFO("Category", "Joints")
    Q_CLASSINFO("Description", "Finger joint (box joint) for corner assembly")

signals:
    void fingerCountChanged(int newCount);
    void fingerWidthChanged(double newWidth);

public:
    FingerJoint();
    FingerJoint(IPart* partA, IPart* partB, const Point3D& position, double angle,
                int fingerCount, double fingerWidth);
    FingerJoint(const FingerJoint& other);
    ~FingerJoint() override = default;

    IJoint* clone() const override;

    static QString staticTypeName() { return "FingerJoint"; }
    QString getTypeName() const override { return staticTypeName(); }

    int getFingerCount() const { return m_fingerCount; }
    void setFingerCount(int count);

    double getFingerWidth() const { return m_fingerWidth; }
    void setFingerWidth(double width);

private:
    int m_fingerCount;      ///< Nombre de doigts
    double m_fingerWidth;   ///< Largeur de chaque doigt

    // Auto-enregistrement dans le Factory Pattern
    static const bool s_registered;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // FINGERJOINT_H
