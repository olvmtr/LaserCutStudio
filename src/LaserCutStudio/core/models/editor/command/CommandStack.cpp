/**
 * @file CommandStack.cpp
 * @brief Implémentation de la pile de commandes
 */

#include "core/models/editor/command/CommandStack.h"
#include "core/infrastructure/logging/LogCategories.h"
#include <QDebug>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

CommandStack::CommandStack(QObject* parent)
    : QObject(parent)
{
    qCInfo(logCore()) << "CommandStack created";
}

CommandStack::~CommandStack()
{
    clear();
    qCInfo(logCore()) << "CommandStack destroyed";
}

// ===== Gestion de la pile =====

void CommandStack::push(IEditorCommand* command)
{
    if (!command) {
        qCWarning(logCore()) << "CommandStack::push: null command";
        return;
    }

    // Essayer de fusionner avec la dernière commande
    if (!m_undoStack.isEmpty()) {
        IEditorCommand* lastCommand = m_undoStack.top();
        if (lastCommand && lastCommand->canMerge() && command->canMerge()) {
            if (lastCommand->mergeWith(command)) {
                // Fusion réussie, supprimer la nouvelle commande
                delete command;
                qCDebug(logCore()) << "CommandStack: Command merged";
                return;
            }
        }
    }

    // Exécuter la commande
    command->execute();

    // Vider la pile redo (nouveau chemin d'historique)
    while (!m_redoStack.isEmpty()) {
        IEditorCommand* cmd = m_redoStack.pop();
        delete cmd;
    }
    emit canRedoChanged(false);
    emit redoCountChanged(0);

    // Ajouter à la pile undo
    m_undoStack.push(command);

    // Appliquer la limite
    enforceUndoLimit();

    qCInfo(logCore()) << "CommandStack: Command pushed:" << command->getText();
    emit commandPushed(command);
    emit canUndoChanged(true);
    emit undoCountChanged(m_undoStack.size());
}

void CommandStack::undo()
{
    // Nettoyer les commandes obsolètes
    cleanObsoleteCommands();

    if (m_undoStack.isEmpty()) {
        qCDebug(logCore()) << "CommandStack::undo: stack is empty";
        return;
    }

    IEditorCommand* command = m_undoStack.pop();

    if (command->isObsolete()) {
        qCWarning(logCore()) << "CommandStack::undo: Command is obsolete, deleting";
        delete command;
        emit undoCountChanged(m_undoStack.size());
        emit canUndoChanged(!m_undoStack.isEmpty());
        return;
    }

    // Annuler la commande
    command->undo();

    // Ajouter à la pile redo
    m_redoStack.push(command);

    qCInfo(logCore()) << "CommandStack: Command undone:" << command->getText();
    emit commandUndone(command);
    emit undoCountChanged(m_undoStack.size());
    emit canUndoChanged(!m_undoStack.isEmpty());
    emit redoCountChanged(m_redoStack.size());
    emit canRedoChanged(true);
}

void CommandStack::redo()
{
    if (m_redoStack.isEmpty()) {
        qCDebug(logCore()) << "CommandStack::redo: stack is empty";
        return;
    }

    IEditorCommand* command = m_redoStack.pop();

    if (command->isObsolete()) {
        qCWarning(logCore()) << "CommandStack::redo: Command is obsolete, deleting";
        delete command;
        emit redoCountChanged(m_redoStack.size());
        emit canRedoChanged(!m_redoStack.isEmpty());
        return;
    }

    // Refaire la commande
    command->redo();

    // Ajouter à la pile undo
    m_undoStack.push(command);

    qCInfo(logCore()) << "CommandStack: Command redone:" << command->getText();
    emit commandRedone(command);
    emit undoCountChanged(m_undoStack.size());
    emit canUndoChanged(true);
    emit redoCountChanged(m_redoStack.size());
    emit canRedoChanged(!m_redoStack.isEmpty());
}

void CommandStack::clear()
{
    // Vider undo
    while (!m_undoStack.isEmpty()) {
        IEditorCommand* cmd = m_undoStack.pop();
        delete cmd;
    }

    // Vider redo
    while (!m_redoStack.isEmpty()) {
        IEditorCommand* cmd = m_redoStack.pop();
        delete cmd;
    }

    qCInfo(logCore()) << "CommandStack: Cleared";
    emit stackCleared();
    emit canUndoChanged(false);
    emit canRedoChanged(false);
    emit undoCountChanged(0);
    emit redoCountChanged(0);
}

// ===== État =====

bool CommandStack::canUndo() const
{
    if (m_undoStack.isEmpty()) return false;

    // Vérifier que la commande du haut n'est pas obsolète
    IEditorCommand* top = m_undoStack.top();
    return top && !top->isObsolete();
}

bool CommandStack::canRedo() const
{
    if (m_redoStack.isEmpty()) return false;

    // Vérifier que la commande du haut n'est pas obsolète
    IEditorCommand* top = m_redoStack.top();
    return top && !top->isObsolete();
}

void CommandStack::setUndoLimit(int limit)
{
    if (m_undoLimit == limit) return;
    m_undoLimit = limit;
    enforceUndoLimit();
    emit undoLimitChanged(limit);
}

QString CommandStack::undoText() const
{
    if (m_undoStack.isEmpty()) return QString();
    IEditorCommand* cmd = m_undoStack.top();
    return cmd ? cmd->getText() : QString();
}

QString CommandStack::redoText() const
{
    if (m_redoStack.isEmpty()) return QString();
    IEditorCommand* cmd = m_redoStack.top();
    return cmd ? cmd->getText() : QString();
}

// ===== Helpers privés =====

void CommandStack::cleanObsoleteCommands()
{
    // Nettoyer les commandes obsolètes en haut de la pile undo
    while (!m_undoStack.isEmpty()) {
        IEditorCommand* top = m_undoStack.top();
        if (top && top->isObsolete()) {
            m_undoStack.pop();
            delete top;
            qCDebug(logCore()) << "CommandStack: Removed obsolete command";
        } else {
            break;
        }
    }

    emit undoCountChanged(m_undoStack.size());
    emit canUndoChanged(!m_undoStack.isEmpty());
}

void CommandStack::enforceUndoLimit()
{
    if (m_undoLimit <= 0) return;  // Illimité

    while (m_undoStack.size() > m_undoLimit) {
        // Supprimer les commandes les plus anciennes (bas de la pile)
        IEditorCommand* oldest = m_undoStack.first();
        m_undoStack.removeFirst();
        delete oldest;
        qCDebug(logCore()) << "CommandStack: Removed old command (limit reached)";
    }

    emit undoCountChanged(m_undoStack.size());
}

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio
