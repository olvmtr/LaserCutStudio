/**
 * @file ScaleCommand.h
 * @brief Commande pour mettre à l'échelle des formes avec Undo/Redo
 */

#ifndef SCALECOMMAND_H
#define SCALECOMMAND_H

#include "core/models/editor/command/transformations/ITransformationCommand.h"
#include "core/models/shapes/IShape.h"
#include "core/models/base/types/Point2D.h"
#include <QVector>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

/**
 * @class ScaleCommand
 * @brief Commande pour mettre à l'échelle une ou plusieurs formes
 *
 * ScaleCommand enregistre la mise à l'échelle de formes et permet de l'annuler/refaire.
 * Supporte la mise à l'échelle multiple (plusieurs formes en même temps).
 *
 * ## Architecture
 * - Hérite de ITransformationCommand (interface avec ListManagerMixin)
 * - Stocke les facteurs d'échelle (X, Y) et le centre de mise à l'échelle
 * - Applique les facteurs lors de execute/redo
 * - Applique les facteurs inverses lors de undo
 *
 * ## Fusion de commandes
 * - Supporte la fusion de mises à l'échelle consécutives sur les mêmes formes
 * - Les facteurs sont multipliés lors de la fusion
 * - Le centre de mise à l'échelle doit être identique pour fusionner
 *
 * ## Utilisation
 * ```cpp
 * QVector<IShape*> shapes = selection->getSelectedShapes();
 * Point2D center = selection->getCenter();
 * auto* cmd = new ScaleCommand(shapes, 1.5, 1.5, center);
 * editorService->pushCommand(cmd);
 * ```
 */
class ScaleCommand : public Transformations::ITransformationCommand
{
    Q_OBJECT

public:
    /**
     * @brief Constructeur
     * @param shapes Formes à mettre à l'échelle
     * @param scaleX Facteur d'échelle horizontal (1.0 = 100%)
     * @param scaleY Facteur d'échelle vertical (1.0 = 100%)
     * @param center Centre de mise à l'échelle
     * @param parent Parent Qt
     */
    explicit ScaleCommand(const QVector<IShape*>& shapes,
                         double scaleX,
                         double scaleY,
                         const Point2D& center,
                         QObject* parent = nullptr);

    /**
     * @brief Constructeur Factory Pattern (depuis QVariantMap)
     *
     * Paramètres attendus:
     * - "shapeIds": QStringList des UUIDs des formes
     * - "scaleX": double - facteur d'échelle horizontal
     * - "scaleY": double - facteur d'échelle vertical
     * - "centerX": double - centre X
     * - "centerY": double - centre Y
     *
     * @param params Paramètres de création
     * @param parent Parent Qt
     */
    explicit ScaleCommand(const QVariantMap& params, QObject* parent = nullptr);

    ~ScaleCommand() override;

    // ===== Type name (Factory Pattern) =====
    DECLARE_TYPE_NAME(ScaleCommand)

    // ===== Prototype Pattern =====
    IEditorCommand* clone() const override;

    // ===== IEditorCommand interface =====
    QString getText() const override;
    void execute() override;
    void undo() override;
    void redo() override;
    bool mergeWith(IEditorCommand* other) override;
    QString getCommandId() const override;

private:
    /**
     * @brief Helper pour résoudre les UUIDs en pointeurs IShape*
     * @param params QVariantMap contenant "shapeIds"
     * @return QVector<IShape*> des formes résolues
     */
    static QVector<IShape*> resolveShapesFromVariant(const QVariantMap& params);

    double m_scaleX;    ///< Facteur d'échelle horizontal
    double m_scaleY;    ///< Facteur d'échelle vertical
    Point2D m_center;   ///< Centre de mise à l'échelle

    // Enregistrement automatique dans Factory
    static const bool s_registered;
};

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio

#endif // SCALECOMMAND_H
