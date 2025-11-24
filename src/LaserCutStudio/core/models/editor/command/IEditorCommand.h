/**
 * @file IEditorCommand.h
 * @brief Interface pour le pattern Command (Undo/Redo)
 *
 * IEditorCommand définit le contrat pour toutes les commandes
 * d'édition annulables/refaisables. Utilisé avec CommandStack
 * pour implémenter Undo/Redo de manière robuste.
 */

#ifndef IEDITORCOMMAND_H
#define IEDITORCOMMAND_H

#include "core/models/base/Interface.h"
#include "core/models/patterns/factory/FactoryMixin.h"
#include <QString>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

/**
 * @class IEditorCommand
 * @brief Interface abstraite pour les commandes d'édition (pattern Command)
 *
 * IEditorCommand encapsule une opération d'édition qui peut être
 * exécutée, annulée et refaite. Utilisé pour implémenter Undo/Redo.
 *
 * ## Cycle de vie
 * 1. Création : La commande capture l'état nécessaire
 * 2. Exécution : execute() applique la modification
 * 3. Annulation : undo() restaure l'état précédent
 * 4. Refaire : redo() réapplique la modification
 *
 * ## Exemples de commandes
 * - CreateShapeCommand : Créer une nouvelle forme
 * - DeleteShapeCommand : Supprimer une forme
 * - MoveShapeCommand : Déplacer une forme
 * - RotateShapeCommand : Rotation d'une forme
 * - ScaleShapeCommand : Mise à l'échelle d'une forme
 *
 * @note Inspiré de QUndoCommand mais adapté à notre architecture
 */
class IEditorCommand : public Interface, protected Patterns::FactoryMixin<IEditorCommand>
{
    Q_OBJECT

    Q_PROPERTY(QString text READ getText NOTIFY textChanged)
    Q_PROPERTY(bool isObsolete READ isObsolete NOTIFY obsoleteChanged)
    Q_PROPERTY(bool canMerge READ canMerge CONSTANT)

public:
    // Expose les méthodes du Factory Pattern
    using FactoryMixin<IEditorCommand>::create;
    using FactoryMixin<IEditorCommand>::availableTypes;
    using FactoryMixin<IEditorCommand>::registerFactory;
    explicit IEditorCommand(QObject* parent = nullptr) : Interface(parent) {}
    ~IEditorCommand() override = default;

    /**
     * @brief Texte descriptif de la commande (pour affichage dans menu Undo/Redo)
     * @return Description lisible (ex: "Create Rectangle", "Move Shape")
     */
    virtual QString getText() const = 0;

    /**
     * @brief Exécute la commande pour la première fois
     *
     * Applique la modification. Appelé une seule fois après création.
     * Ne doit pas être appelé si redo() a déjà été utilisé.
     */
    virtual void execute() = 0;

    /**
     * @brief Annule la commande
     *
     * Restaure l'état précédent. Doit être idempotent :
     * appeler undo() plusieurs fois doit avoir le même effet qu'un seul appel.
     */
    virtual void undo() = 0;

    /**
     * @brief Refait la commande après annulation
     *
     * Réapplique la modification après un undo(). Doit être idempotent.
     */
    virtual void redo() = 0;

    /**
     * @brief Vérifie si la commande est obsolète
     *
     * Une commande devient obsolète si l'objet qu'elle modifie
     * a été supprimé ou n'existe plus.
     *
     * @return true si la commande ne peut plus être undo/redo
     */
    virtual bool isObsolete() const = 0;

    /**
     * @brief Vérifie si cette commande peut être fusionnée avec une autre
     *
     * Permet d'optimiser la stack Undo/Redo en fusionnant des commandes
     * similaires (ex: plusieurs Move consécutifs → un seul Move).
     *
     * @return true si la commande supporte la fusion
     */
    virtual bool canMerge() const = 0;

    /**
     * @brief Tente de fusionner avec une autre commande
     *
     * @param other Commande à fusionner
     * @return true si la fusion a réussi
     *
     * @note Seules les commandes avec canMerge() == true peuvent fusionner
     */
    virtual bool mergeWith(IEditorCommand* other) = 0;

    /**
     * @brief ID du type de commande (pour fusion)
     *
     * Commandes avec même ID peuvent potentiellement fusionner.
     * @return ID unique du type (ex: "MoveShape", "RotateShape")
     */
    virtual QString getCommandId() const = 0;

signals:
    /**
     * @brief Émis quand le texte change
     */
    void textChanged(const QString& text);

    /**
     * @brief Émis quand la commande devient obsolète
     */
    void obsoleteChanged(bool obsolete);

    /**
     * @brief Émis après execute()
     */
    void executed();

    /**
     * @brief Émis après undo()
     */
    void undone();

    /**
     * @brief Émis après redo()
     */
    void redone();
};

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio

#endif // IEDITORCOMMAND_H
