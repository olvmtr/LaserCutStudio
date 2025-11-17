#ifndef TABJOINT_H
#define TABJOINT_H

#include "IJoint.h"

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Joint à tenon/encoche simple
 */
class TabJoint : public IJoint
{
public:
    TabJoint();
    TabJoint(IPart* partA, IPart* partB, const Point3D& position, double angle,
             double tabWidth, double tabDepth);
    TabJoint(const TabJoint& other);
    ~TabJoint() override = default;

    /**
     * @brief Clone le joint
     */
    IJoint* clone() const override;

    /**
     * @brief Obtient la largeur du tenon
     */
    double getTabWidth() const { return m_tabWidth; }

    /**
     * @brief Définit la largeur du tenon
     */
    void setTabWidth(double width) { m_tabWidth = width; }

    /**
     * @brief Obtient la profondeur du tenon
     */
    double getTabDepth() const { return m_tabDepth; }

    /**
     * @brief Définit la profondeur du tenon
     */
    void setTabDepth(double depth) { m_tabDepth = depth; }

private:
    double m_tabWidth;  ///< Largeur du tenon
    double m_tabDepth;  ///< Profondeur du tenon
};

} // namespace Core
} // namespace LaserCutStudio

#endif // TABJOINT_H
