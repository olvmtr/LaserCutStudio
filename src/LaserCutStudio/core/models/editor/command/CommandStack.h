/**
 * @file CommandStack.h
 * @brief Pile de commandes pour Undo/Redo
 *
 * CommandStack gère l'historique des commandes pour implémenter
 * un système Undo/Redo robuste avec nettoyage automatique des
 * commandes obsolètes et support de fusion.
 */

#ifndef COMMANDSTACK_H
#define COMMANDSTACK_H

#include <QObject>
#include <QStack>
#include "core/models/editor/command/IEditorCommand.h"

namespace LaserCutStudio {
namespace Core {
namespace Editor {

/**
 * @class CommandStack
 * @brief Gestionnaire d'historique Undo/Redo
 *
 * Cette classe gère une pile de commandes (Pattern Command) avec :
 * - Historique Undo/Redo illimité (ou limité par undoLimit)
 * - Nettoyage automatique des commandes obsolètes
 * - Fusion de commandes similaires (optimisation)
 * - Signaux pour mise à jour UI (canUndo/canRedo)
 *
 * ## Usage
 * @code
 * CommandStack* stack = new CommandStack();
 * stack->setUndoLimit(100);  // Limiter à 100 commandes
 *
 * // Ajouter une commande
 * CreateShapeCommand* cmd = new CreateShapeCommand(params, service);
 * stack->push(cmd);  // Exécute et ajoute à la pile
 *
 * // Undo/Redo
 * if (stack->canUndo()) {
 *     stack->undo();
 * }
 *
 * if (stack->canRedo()) {
 *     stack->redo();
 * }
 *
 * // Clear
 * stack->clear();
 * @endcode
 *
 * ## Fusion de commandes
 * Si la commande actuelle supporte canMerge() et mergeWith(), elle sera
 * fusionnée avec la précédente au lieu de créer une nouvelle entrée.
 *
 * ## Commandes obsolètes
 * Les commandes marquées isObsolete() sont automatiquement supprimées
 * et le Undo s'arrête à la première commande obsolète rencontrée.
 */
class CommandStack : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY canRedoChanged)
    Q_PROPERTY(int undoCount READ undoCount NOTIFY undoCountChanged)
    Q_PROPERTY(int redoCount READ redoCount NOTIFY redoCountChanged)
    Q_PROPERTY(int undoLimit READ undoLimit WRITE setUndoLimit NOTIFY undoLimitChanged)

public:
    explicit CommandStack(QObject* parent = nullptr);
    ~CommandStack() override;

    // ===== Gestion de la pile =====

    /**
     * @brief Ajoute et exécute une commande
     * @param command Commande à exécuter (prend ownership)
     *
     * La commande est d'abord exécutée via execute(), puis ajoutée
     * à la pile undo. Si la pile redo n'est pas vide, elle est vidée.
     *
     * Si la commande supporte la fusion et que la dernière commande
     * sur la pile peut être fusionnée, mergeWith() est appelé au lieu
     * d'ajouter une nouvelle entrée.
     */
    void push(IEditorCommand* command);

    /**
     * @brief Annule la dernière commande
     *
     * Appelle undo() sur la dernière commande, la retire de la pile
     * undo et l'ajoute à la pile redo.
     *
     * Les commandes obsolètes sont ignorées automatiquement.
     */
    void undo();

    /**
     * @brief Refait la dernière commande annulée
     *
     * Appelle redo() sur la dernière commande de la pile redo,
     * la retire de redo et l'ajoute à undo.
     */
    void redo();

    /**
     * @brief Vide les deux piles et détruit les commandes
     */
    void clear();

    // ===== État =====

    bool canUndo() const;
    bool canRedo() const;
    int undoCount() const { return m_undoStack.size(); }
    int redoCount() const { return m_redoStack.size(); }

    /**
     * @brief Limite du nombre de commandes undo
     * @return 0 = illimité, > 0 = limité
     */
    int undoLimit() const { return m_undoLimit; }

    /**
     * @brief Définit la limite de commandes undo
     * @param limit 0 = illimité, > 0 = limité
     *
     * Si la pile dépasse la limite, les anciennes commandes sont supprimées.
     */
    void setUndoLimit(int limit);

    /**
     * @brief Texte de la prochaine commande undo
     * @return Texte ou QString vide si aucune commande
     */
    QString undoText() const;

    /**
     * @brief Texte de la prochaine commande redo
     * @return Texte ou QString vide si aucune commande
     */
    QString redoText() const;

signals:
    void canUndoChanged(bool canUndo);
    void canRedoChanged(bool canRedo);
    void undoCountChanged(int count);
    void redoCountChanged(int count);
    void undoLimitChanged(int limit);

    void commandPushed(IEditorCommand* command);
    void commandUndone(IEditorCommand* command);
    void commandRedone(IEditorCommand* command);
    void stackCleared();

private:
    /**
     * @brief Nettoie les commandes obsolètes en haut de la pile undo
     */
    void cleanObsoleteCommands();

    /**
     * @brief Applique la limite undoLimit en supprimant les vieilles commandes
     */
    void enforceUndoLimit();

    QStack<IEditorCommand*> m_undoStack;
    QStack<IEditorCommand*> m_redoStack;
    int m_undoLimit = 0;  // 0 = illimité
};

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio

#endif // COMMANDSTACK_H
