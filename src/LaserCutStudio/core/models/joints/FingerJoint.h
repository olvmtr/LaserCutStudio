#ifndef FINGERJOINT_H
#define FINGERJOINT_H

#include "core/models/joints/IJoint.h"
#include "core/models/patterns/properties/PropertyMixin.h"

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
    /**
     * @brief Constructeur par défaut
     *
     * Crée un finger joint avec 5 doigts de 10mm de largeur
     */
    FingerJoint();

    /**
     * @brief Constructeur avec paramètres
     * @param partA Première pièce à assembler
     * @param partB Seconde pièce à assembler
     * @param position Position 3D du joint dans l'espace
     * @param angle Angle de rotation du joint en degrés
     * @param fingerCount Nombre de doigts (doit être ≥ 1)
     * @param fingerWidth Largeur de chaque doigt en mm (doit être > 0)
     */
    FingerJoint(IPart* partA, IPart* partB, const Point3D& position, double angle,
                int fingerCount, double fingerWidth);

    /**
     * @brief Constructeur de copie
     * @param other Finger joint à copier
     * @note Les pointeurs vers les pièces sont copiés (shallow copy)
     */
    FingerJoint(const FingerJoint& other);

    ~FingerJoint() override = default;

    /**
     * @brief Clone le finger joint
     * @return Nouveau finger joint identique alloué dynamiquement
     * @note Les pointeurs vers les pièces sont copiés (shallow copy)
     */
    IJoint* clone() const override;

    static QString staticTypeName() { return "FingerJoint"; }
    QString getTypeName() const override { return staticTypeName(); }

    /**
     * @brief Obtient le nombre de doigts
     * @return Nombre de doigts du joint
     */
    int getFingerCount() const { return m_fingerCount; }

    /**
     * @brief Définit le nombre de doigts
     * @param count Nouveau nombre de doigts (doit être ≥ 1)
     */
    void setFingerCount(int count);

    /**
     * @brief Obtient la largeur des doigts
     * @return Largeur de chaque doigt en mm
     */
    double getFingerWidth() const { return m_fingerWidth; }

    /**
     * @brief Définit la largeur des doigts
     * @param width Nouvelle largeur en mm (doit être > 0)
     */
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
