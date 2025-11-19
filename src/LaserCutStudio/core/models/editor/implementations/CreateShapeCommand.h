/**
 * @file CreateShapeCommand.h
 * @brief Commande pour créer une forme (Undo/Redo)
 *
 * CreateShapeCommand encapsule l'opération de création d'une forme,
 * permettant d'annuler (delete) et refaire (recréer) l'opération.
 */

#ifndef CREATESHAPECOMMAND_H
#define CREATESHAPECOMMAND_H

#include "core/models/editor/interfaces/IEditorCommand.h"
#include "core/models/shapes/interfaces/IShape.h"
#include "core/services/editor/EditorService.h"
#include <QVariantMap>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

/**
 * @class CreateShapeCommand
 * @brief Commande de création de forme avec support Undo/Redo
 *
 * Cette commande encapsule la création d'une forme dans l'éditeur.
 *
 * ## Cycle de vie
 * 1. **Création** : La commande stocke les paramètres de la forme
 * 2. **execute()** : Crée la forme via Factory Pattern
 * 3. **undo()** : Supprime la forme de l'éditeur
 * 4. **redo()** : Recrée la forme avec les mêmes paramètres
 *
 * ## Gestion mémoire
 * - La commande prend ownership de la forme pendant undo
 * - La forme est rendue à l'éditeur pendant execute/redo
 * - Suppression automatique si commande obsolète
 *
 * @note Ne supporte PAS la fusion (canMerge = false)
 */
class CreateShapeCommand : public IEditorCommand
{
    Q_OBJECT

public:
    /**
     * @brief Constructeur avec paramètres de forme
     * @param shapeParams Paramètres pour IShape::create() (doit contenir "type")
     * @param editorService Service éditeur (non-owner)
     * @param parent Parent Qt
     */
    explicit CreateShapeCommand(const QVariantMap& shapeParams,
                                Services::EditorService* editorService,
                                QObject* parent = nullptr);

    ~CreateShapeCommand() override;

    // ===== Macro pour getTypeName() =====
    DECLARE_TYPE_NAME(CreateShapeCommand)

    // ===== IEditorCommand interface =====

    QString getText() const override;

    void execute() override;
    void undo() override;
    void redo() override;

    bool isObsolete() const override;
    bool canMerge() const override { return false; }  // Pas de fusion pour create
    bool mergeWith(IEditorCommand* other) override;
    QString getCommandId() const override { return "CreateShape"; }

    // ===== Prototype Pattern =====

    IEditorCommand* clone() const override;

private:
    QVariantMap m_shapeParams;              ///< Paramètres pour recréer la forme
    Services::EditorService* m_editorService;  ///< Service éditeur (non-owner)
    IShape* m_shape = nullptr;              ///< Forme créée (ownership variable)
    bool m_executed = false;                ///< État d'exécution
};

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio

#endif // CREATESHAPECOMMAND_H
