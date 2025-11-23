/**
 * @file DeleteShapeCommand.h
 * @brief Commande pour supprimer des formes (Undo/Redo)
 *
 * DeleteShapeCommand encapsule l'opération de suppression de formes,
 * permettant d'annuler (restaurer) et refaire (supprimer à nouveau) l'opération.
 */

#ifndef DELETESHAPECOMMAND_H
#define DELETESHAPECOMMAND_H

#include "core/models/editor/command/IEditorCommand.h"
#include "core/models/shapes/IShape.h"
#include "core/services/editor/EditorService.h"
#include <QVector>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

/**
 * @class DeleteShapeCommand
 * @brief Commande de suppression de formes avec support Undo/Redo
 *
 * Cette commande encapsule la suppression d'une ou plusieurs formes de l'éditeur.
 *
 * ## Cycle de vie
 * 1. **Création** : La commande reçoit les formes à supprimer
 * 2. **execute()** : Retire les formes de l'éditeur, prend ownership
 * 3. **undo()** : Restaure les formes dans l'éditeur
 * 4. **redo()** : Retire à nouveau les formes
 *
 * ## Gestion mémoire
 * - La commande prend ownership des formes pendant execute/redo
 * - Les formes sont rendues à l'éditeur pendant undo
 * - Suppression automatique des formes si commande détruite après execute
 *
 * ## Utilisation
 * ```cpp
 * QVector<IShape*> selectedShapes = selection->getSelectedShapes();
 * auto* cmd = new DeleteShapeCommand(selectedShapes, editorService);
 * editorService->pushCommand(cmd);  // Execute automatiquement
 * ```
 *
 * @note Ne supporte PAS la fusion (canMerge = false)
 */
class DeleteShapeCommand : public IEditorCommand
{
    Q_OBJECT

public:
    /**
     * @brief Constructeur avec formes à supprimer
     * @param shapes Formes à supprimer (doivent exister dans l'éditeur)
     * @param editorService Service éditeur (non-owner)
     * @param parent Parent Qt
     */
    explicit DeleteShapeCommand(const QVector<IShape*>& shapes,
                                Services::EditorService* editorService,
                                QObject* parent = nullptr);

    ~DeleteShapeCommand() override;

    // ===== Macro pour getTypeName() =====
    DECLARE_TYPE_NAME(DeleteShapeCommand)

    // ===== IEditorCommand interface =====

    QString getText() const override;

    void execute() override;
    void undo() override;
    void redo() override;

    bool isObsolete() const override;
    bool canMerge() const override { return false; }  // Pas de fusion pour delete
    bool mergeWith(IEditorCommand* other) override;
    QString getCommandId() const override { return "DeleteShape"; }

    // ===== Prototype Pattern =====

    IEditorCommand* clone() const override;

private:
    QVector<IShape*> m_shapes;              ///< Formes supprimées (ownership variable)
    Services::EditorService* m_editorService;  ///< Service éditeur (non-owner)
    bool m_executed = false;                ///< État d'exécution
};

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio

#endif // DELETESHAPECOMMAND_H
