#ifndef TABJOINT_H
#define TABJOINT_H

#include "core/models/joints/IJoint.h"
#include "core/infrastructure/patterns/properties/PropertyMixin.h"

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
    /**
     * @brief Constructeur par défaut
     *
     * Crée un tab joint avec largeur 20mm et profondeur 10mm
     */
    TabJoint();

    /**
     * @brief Constructeur avec paramètres
     * @param partA Première pièce à assembler
     * @param partB Seconde pièce à assembler
     * @param position Position 3D du joint dans l'espace
     * @param angle Angle de rotation du joint en degrés
     * @param tabWidth Largeur du tenon en mm (doit être > 0)
     * @param tabDepth Profondeur du tenon en mm (doit être > 0)
     */
    TabJoint(IPart* partA, IPart* partB, const Point3D& position, double angle,
             double tabWidth, double tabDepth);

    /**
     * @brief Constructeur de copie
     * @param other Tab joint à copier
     * @note Les pointeurs vers les pièces sont copiés (shallow copy)
     */
    TabJoint(const TabJoint& other);

    ~TabJoint() override = default;

    /**
     * @brief Clone le joint
     */
    IJoint* clone() const override;

    /**
     * @brief Retourne le nom de type statique pour le Factory Pattern
     */
    static QString staticTypeName() { return "TabJoint"; }

    /**
     * @brief Retourne le nom de type pour l'instance (Factory Pattern)
     */
    QString getTypeName() const override { return staticTypeName(); }

    /**
     * @brief Obtient la largeur du tenon
     */
    double getTabWidth() const { return m_tabWidth; }

    /**
     * @brief Définit la largeur du tenon et émet le signal si la valeur change
     * @param width Nouvelle largeur du tenon en mm (doit être > 0)
     */
    void setTabWidth(double width);

    /**
     * @brief Obtient la profondeur du tenon
     */
    double getTabDepth() const { return m_tabDepth; }

    /**
     * @brief Définit la profondeur du tenon et émet le signal si la valeur change
     * @param depth Nouvelle profondeur du tenon en mm (doit être > 0)
     */
    void setTabDepth(double depth);

private:
    double m_tabWidth;  ///< Largeur du tenon
    double m_tabDepth;  ///< Profondeur du tenon

    // Auto-enregistrement dans le Factory Pattern
    static const bool s_registered;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // TABJOINT_H
