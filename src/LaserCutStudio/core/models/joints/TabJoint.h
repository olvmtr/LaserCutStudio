#ifndef TABJOINT_H
#define TABJOINT_H

#include "core/models/joints/IJoint.h"
#include "core/models/patterns/properties/PropertyMixin.h"

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Joint à tenon/encoche simple
 *
 * Utilise Q_PROPERTY pour l'introspection automatique et la sérialisation.
 * Utilise PropertyMixin pour simplifier les setters avec émission automatique de signaux.
 */
class TabJoint : public IJoint,
                 protected Patterns::PropertyMixin<TabJoint>
{
    Q_OBJECT

    // Propriétés Qt pour introspection et sérialisation automatiques
    Q_PROPERTY(double tabWidth READ getTabWidth WRITE setTabWidth NOTIFY tabWidthChanged)
    Q_PROPERTY(double tabDepth READ getTabDepth WRITE setTabDepth NOTIFY tabDepthChanged)

    // Métadonnées accessibles au runtime
    Q_CLASSINFO("Version", "1.0.0")
    Q_CLASSINFO("Category", "Joints")
    Q_CLASSINFO("Description", "Tab and slot joint for perpendicular assembly")

signals:
    /**
     * @brief Signal émis lorsque la largeur du tenon change
     */
    void tabWidthChanged(double newWidth);

    /**
     * @brief Signal émis lorsque la profondeur du tenon change
     */
    void tabDepthChanged(double newDepth);

public:
    TabJoint();
    TabJoint(IPart* partA, IPart* partB, const Point3D& position, double angle,
             double tabWidth, double tabDepth);
    TabJoint(const TabJoint& other);
    ~TabJoint() override = default;

    // Interface IJoint implementation
    IJoint* clone() const override;
    DECLARE_TYPE_NAME(TabJoint)

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

    // TabJoint specific properties
    double getTabWidth() const { return m_tabWidth; }
    void setTabWidth(double width);
    double getTabDepth() const { return m_tabDepth; }
    void setTabDepth(double depth);

private:
    // IJoint members
    JointType m_type;
    IPart* m_partA;
    IPart* m_partB;
    Point3D m_position;
    double m_angle;

    // TabJoint specific members
    double m_tabWidth;
    double m_tabDepth;

    // Helpers
    void connectToPart(IPart*& partMember, IPart* newPart);
    void disconnectFromPart(IPart*& partMember);

    // Auto-enregistrement dans le Factory Pattern
    static const bool s_registered;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // TABJOINT_H
