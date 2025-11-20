/**
 * @file MoveCommand.h
 * @brief Commande pour déplacer des formes avec Undo/Redo
 */

#ifndef MOVECOMMAND_H
#define MOVECOMMAND_H

#include "core/models/editor/interfaces/IEditorCommand.h"
#include "core/models/shapes/interfaces/IShape.h"
#include <QVector>
#include <QMap>

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
 * - Stocke les positions initiales de chaque forme
 * - Applique un delta (dx, dy) lors de execute/redo
 * - Restaure les positions lors de undo
 *
 * ## Utilisation
 * ```cpp
 * QVector<IShape*> shapes = selection->getSelectedShapes();
 * auto* cmd = new MoveCommand(shapes, 10.0, 20.0);
 * editorService->pushCommand(cmd);
 * ```
 */
class MoveCommand : public IEditorCommand
{
    Q_OBJECT

public:
    /**
     * @brief Constructeur
     * @param shapes Formes à déplacer
     * @param dx Déplacement horizontal
     * @param dy Déplacement vertical
     * @param parent Parent Qt
     */
    explicit MoveCommand(const QVector<IShape*>& shapes,
                        double dx,
                        double dy,
                        QObject* parent = nullptr);

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
    bool isObsolete() const override;
    bool canMerge() const override;
    bool mergeWith(IEditorCommand* other) override;
    QString getCommandId() const override;

private:
    QVector<IShape*> m_shapes;  // Formes à déplacer
    double m_dx;  // Déplacement X
    double m_dy;  // Déplacement Y
    bool m_executed;  // État d'exécution

    // Positions initiales (pour undo)
    QMap<IShape*, QPair<double, double>> m_initialPositions;
};

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio

#endif // MOVECOMMAND_H
