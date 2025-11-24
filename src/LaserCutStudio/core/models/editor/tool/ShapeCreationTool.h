/**
 * @file ShapeCreationTool.h
 * @brief Outil générique de création de formes via Factory Pattern
 *
 * ShapeCreationTool est un outil universel qui peut créer n'importe quelle
 * forme (Rectangle, Circle, Polygon, etc.) en utilisant le Factory Pattern.
 * Il fonctionne automatiquement avec les formes de plugins.
 */

#ifndef SHAPECREATIONTOOL_H
#define SHAPECREATIONTOOL_H

#include "core/models/editor/tool/ITool.h"
#include "core/models/patterns/properties/PropertyMixin.h"
#include "core/models/shapes/IShape.h"
#include "core/models/base/types/Point2D.h"
#include <QIcon>
#include <QCursor>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

/**
 * @class ShapeCreationTool
 * @brief Outil générique pour créer des formes via Factory Pattern
 *
 * ShapeCreationTool est un outil universel qui utilise IShape::create()
 * pour créer n'importe quelle forme enregistrée dans le Factory Pattern.
 *
 * ## Fonctionnement
 * 1. Définir le type de forme : setShapeType("Rectangle")
 * 2. L'utilisateur clique et glisse dans le canvas
 * 3. L'outil calcule les paramètres (position, dimensions)
 * 4. Crée la forme via IShape::create(shapeType, params)
 * 5. Émet shapeCreated(shape)
 *
 * ## Modes de création
 * - **Click & Drag** : Clic + glissement définit position et taille
 * - **Click & Click** : Premier clic = coin, second clic = coin opposé
 * - **Paramétrique** : Dimensions prédéfinies, clic = position
 *
 * ## Extensibilité
 * Fonctionne automatiquement avec :
 * - Formes built-in (Rectangle, Circle)
 * - Formes de plugins (Polygon, Star, Gear, etc.)
 *
 * @note Respecte le principe Open/Closed : pas de modification nécessaire
 *       pour ajouter de nouvelles formes
 */
class ShapeCreationTool : public ITool,
                          protected Patterns::PropertyMixin<ShapeCreationTool>
{
    Q_OBJECT

    Q_PROPERTY(QString shapeType READ getShapeType WRITE setShapeType NOTIFY shapeTypeChanged)
    Q_PROPERTY(CreationMode creationMode READ getCreationMode WRITE setCreationMode NOTIFY creationModeChanged)
    Q_PROPERTY(bool isDrawing READ isDrawing NOTIFY drawingStateChanged)

public:
    /**
     * @enum CreationMode
     * @brief Mode de création de forme
     */
    enum class CreationMode {
        ClickAndDrag,      ///< Clic + glissement (défaut)
        TwoClicks,         ///< Deux clics (coin à coin)
        Parametric         ///< Dimensions fixes, clic pour position
    };
    Q_ENUM(CreationMode)

    explicit ShapeCreationTool(const QString& shapeType = "Rectangle",
                               QObject* parent = nullptr);
    ~ShapeCreationTool() override;

    // ===== Macro pour getTypeName() =====
    DECLARE_TYPE_NAME(ShapeCreationTool)

    // ===== ITool interface =====

    QString getName() const override;
    QString getDescription() const override;
    QIcon getIcon() const override;
    QCursor getCursor() const override;

    bool isActive() const override { return m_isActive; }
    void setActive(bool active) override;

    void activate() override;
    void deactivate() override;

    bool handleMousePress(const Point2D& scenePos, Qt::MouseButton button) override;
    bool handleMouseMove(const Point2D& scenePos) override;
    bool handleMouseRelease(const Point2D& scenePos, Qt::MouseButton button) override;
    bool handleKeyPress(int key, Qt::KeyboardModifiers modifiers) override;

    // ===== Prototype Pattern =====

    ITool* clone() const override;

    // ===== Configuration =====

    /**
     * @brief Type de forme à créer (ex: "Rectangle", "Circle")
     */
    QString getShapeType() const { return m_shapeType; }
    void setShapeType(const QString& type);

    /**
     * @brief Mode de création
     */
    CreationMode getCreationMode() const { return m_creationMode; }
    void setCreationMode(CreationMode mode);

    /**
     * @brief Vérifie si l'utilisateur est en train de dessiner
     */
    bool isDrawing() const { return m_isDrawing; }

    /**
     * @brief Paramètres par défaut pour la forme
     *
     * Utilisés en mode Parametric ou comme valeurs initiales
     */
    QVariantMap getDefaultParameters() const { return m_defaultParams; }
    void setDefaultParameters(const QVariantMap& params);

signals:
    /**
     * @brief Émis quand le type de forme change
     */
    void shapeTypeChanged(const QString& type);

    /**
     * @brief Émis quand le mode de création change
     */
    void creationModeChanged(CreationMode mode);

    /**
     * @brief Émis quand l'état de dessin change
     */
    void drawingStateChanged(bool drawing);

    /**
     * @brief Émis pendant le dessin pour feedback visuel
     * @param previewShape Forme temporaire pour prévisualisation
     */
    void shapePreview(IShape* previewShape);

private:
    // État
    bool m_isActive = false;
    bool m_isDrawing = false;

    // Configuration
    QString m_shapeType;
    CreationMode m_creationMode;
    QVariantMap m_defaultParams;

    // Interaction
    Point2D m_startPoint;      ///< Point de départ (premier clic)
    Point2D m_currentPoint;    ///< Point actuel (curseur)
    IShape* m_previewShape = nullptr;  ///< Forme temporaire pour prévisualisation

    // Helpers
    /**
     * @brief Calcule les paramètres de la forme depuis deux points
     */
    QVariantMap calculateShapeParams(const Point2D& p1, const Point2D& p2) const;

    /**
     * @brief Crée la forme finale
     */
    IShape* createFinalShape();

    /**
     * @brief Met à jour la prévisualisation
     */
    void updatePreview();

    /**
     * @brief Nettoie la prévisualisation
     */
    void clearPreview();
};

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio

#endif // SHAPECREATIONTOOL_H
