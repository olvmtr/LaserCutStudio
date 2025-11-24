#ifndef FINGERJOINT_H
#define FINGERJOINT_H

#include "core/models/joints/IJoint.h"
#include "core/models/patterns/properties/PropertyMixin.h"
#include "core/models/patterns/prototype/ClonableMixin.h"

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Joint à doigts (finger joint / box joint)
 *
 * Utilise PropertyMixin pour simplifier les setters avec émission automatique de signaux.
 */
class FingerJoint : public IJoint,
                    protected Patterns::PropertyMixin<FingerJoint>
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

    // Interface IJoint implementation
    IJoint* clone() const override;
    DECLARE_TYPE_NAME(FingerJoint)

    JointType getType() const override;
    void setType(JointType type) override;
    IPart* getPartA() const override;
    IPart* getPartB() const override;
    void connect(IPart* partA, IPart* partB) override;
    void disconnect() override;
    Point3D getPosition() const override;
    void setPosition(const Point3D& position) override;
    double getAngle() const override;
    void setAngle(double angle) override;
    bool isValid() const override;

    // FingerJoint specific properties
    int getFingerCount() const { return m_fingerCount; }
    void setFingerCount(int count);
    double getFingerWidth() const { return m_fingerWidth; }
    void setFingerWidth(double width);

private:
    // IJoint members
    JointType m_type;
    IPart* m_partA;
    IPart* m_partB;
    Point3D m_position;
    double m_angle;

    // FingerJoint specific members
    int m_fingerCount;
    double m_fingerWidth;

    // Helpers
    void connectToPart(IPart*& partMember, IPart* newPart);
    void disconnectFromPart(IPart*& partMember);

    // Auto-enregistrement dans le Factory Pattern
    static const bool s_registered;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // FINGERJOINT_H
