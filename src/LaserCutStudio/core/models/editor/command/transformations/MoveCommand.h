/**
 * @file MoveCommand.h
 * @brief Commande pour déplacer des formes avec Undo/Redo
 */

#ifndef MOVECOMMAND_H
#define MOVECOMMAND_H

#include "core/models/editor/command/transformations/ITransformationCommand.h"
#include "core/models/shapes/IShape.h"
#include <QVector>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

/**
 * @class MoveCommand
 * @brief Commande pour déplacer une ou plusieurs formes
 *
 * MoveCommand enregistre le déplacement de formes et permet de l'annuler/refaire.
 * Supporte le déplacement multiple (plusieurs formes en même temps).
 *
 * ## Architecture
 * - Hérite de ITransformationCommand (interface avec ListManagerMixin)
 * - Stocke le delta (dx, dy) de déplacement
 * - Applique le delta lors de execute/redo
 * - Applique l'inverse (-dx, -dy) lors de undo
 *
 * ## Fusion
 * - Supporte la fusion de déplacements consécutifs
 * - Les deltas sont additionnés lors de la fusion
 *
 * ## Utilisation
 * ```cpp
 * QVector<IShape*> shapes = selection->getSelectedShapes();
 * auto* cmd = new MoveCommand(shapes, 10.0, 20.0);
 * editorService->pushCommand(cmd);
 * ```
 */
class MoveCommand : public Transformations::ITransformationCommand
{
    Q_OBJECT

public:
    /**
     * @brief Constructeur principal
     * @param shapes Formes à déplacer
     * @param dx Déplacement horizontal
     * @param dy Déplacement vertical
     * @param parent Parent Qt
     */
    explicit MoveCommand(const QVector<IShape*>& shapes,
                        double dx,
                        double dy,
                        QObject* parent = nullptr);

    /**
     * @brief Constructeur Factory Pattern (depuis QVariantMap)
     *
     * Paramètres attendus:
     * - "shapeIds": QStringList des UUIDs des formes
     * - "dx": double - déplacement horizontal
     * - "dy": double - déplacement vertical
     *
     * @param params Paramètres de création
     * @param parent Parent Qt
     */
    explicit MoveCommand(const QVariantMap& params, QObject* parent = nullptr);

    ~MoveCommand() override;

    // ===== Type name (Factory Pattern) =====
    DECLARE_TYPE_NAME(MoveCommand)

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

    double m_dx;  ///< Déplacement horizontal
    double m_dy;  ///< Déplacement vertical

    // Enregistrement automatique dans Factory
    static const bool s_registered;
};

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio

#endif // MOVECOMMAND_H
