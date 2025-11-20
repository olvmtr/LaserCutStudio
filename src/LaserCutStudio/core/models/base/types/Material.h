#ifndef MATERIAL_H
#define MATERIAL_H

#include <QString>
#include <QColor>

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Représente un matériau pour les pièces
 */
class Material
{
public:
    /**
     * @brief Constructeur par défaut
     */
    Material()
        : m_name("Unknown")
        , m_density(0.0)
        , m_color(Qt::gray)
    {}

    /**
     * @brief Constructeur avec paramètres
     * @param name Nom du matériau
     * @param density Densité (kg/m³)
     * @param color Couleur de visualisation
     */
    Material(const QString& name, double density, const QColor& color)
        : m_name(name)
        , m_density(density)
        , m_color(color)
    {}

    /**
     * @brief Obtient le nom du matériau
     * @return Nom du matériau
     */
    QString getName() const { return m_name; }

    /**
     * @brief Définit le nom du matériau
     * @param name Nouveau nom du matériau
     */
    void setName(const QString& name) { m_name = name; }

    /**
     * @brief Obtient la densité
     * @return Densité en kg/m³
     */
    double getDensity() const { return m_density; }

    /**
     * @brief Définit la densité
     * @param density Nouvelle densité en kg/m³ (doit être > 0)
     */
    void setDensity(double density) { m_density = density; }

    /**
     * @brief Obtient la couleur
     * @return Couleur de visualisation du matériau
     */
    QColor getColor() const { return m_color; }

    /**
     * @brief Définit la couleur
     * @param color Nouvelle couleur de visualisation
     */
    void setColor(const QColor& color) { m_color = color; }

    // Matériaux prédéfinis

    /**
     * @brief Crée un matériau Bois
     * @return Matériau bois avec densité 600 kg/m³ et couleur marron
     */
    static Material Wood() { return Material("Wood", 600.0, QColor(139, 90, 43)); }

    /**
     * @brief Crée un matériau Contreplaqué
     * @return Matériau contreplaqué avec densité 550 kg/m³ et couleur beige
     */
    static Material Plywood() { return Material("Plywood", 550.0, QColor(210, 180, 140)); }

    /**
     * @brief Crée un matériau MDF (Medium-Density Fiberboard)
     * @return Matériau MDF avec densité 750 kg/m³ et couleur brun clair
     */
    static Material MDF() { return Material("MDF", 750.0, QColor(160, 120, 90)); }

    /**
     * @brief Crée un matériau Acrylique
     * @return Matériau acrylique avec densité 1180 kg/m³ et couleur bleu translucide
     */
    static Material Acrylic() { return Material("Acrylic", 1180.0, QColor(200, 200, 255)); }

    /**
     * @brief Crée un matériau Carton
     * @return Matériau carton avec densité 300 kg/m³ et couleur beige clair
     */
    static Material Cardboard() { return Material("Cardboard", 300.0, QColor(210, 180, 140)); }

private:
    QString m_name;   ///< Nom du matériau
    double m_density; ///< Densité en kg/m³
    QColor m_color;   ///< Couleur pour la visualisation
};

} // namespace Core
} // namespace LaserCutStudio

#endif // MATERIAL_H
