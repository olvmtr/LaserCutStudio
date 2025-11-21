/**
 * @file EditorService.h
 * @brief Service métier pour l'éditeur 2D
 *
 * EditorService gère la logique métier de l'éditeur : gestion des outils,
 * de la sélection, du stack de commandes (Undo/Redo), et coordination
 * entre les différents composants de l'éditeur.
 */

#ifndef EDITORSERVICE_H
#define EDITORSERVICE_H

#include <QObject>
#include <QVector>
#include "core/models/editor/interfaces/ITool.h"
#include "core/models/editor/interfaces/ISelection.h"
#include "core/models/editor/interfaces/IEditorCommand.h"
#include "core/models/editor/implementations/CommandStack.h"
#include "core/models/shapes/interfaces/IShape.h"

namespace LaserCutStudio {
namespace Core {

// Forward declarations pour MOC
namespace Editor {
    class ITool;
    class ISelection;
    class IEditorCommand;
}

namespace Services {

/**
 * @class EditorService
 * @brief Service central de gestion de l'éditeur 2D
 *
 * EditorService coordonne tous les aspects de l'édition 2D :
 * - Gestion des outils actifs (RectangleTool, CircleTool, etc.)
 * - Gestion de la sélection d'objets
 * - Stack Undo/Redo avec IEditorCommand
 * - Liste des formes dans l'éditeur
 * - Configuration et préférences
 *
 * ## Architecture
 * - **Service** (couche services/) : Logique métier pure
 * - Utilise **Dependency Injection** via ServiceLocator
 * - Envoie des **signaux Qt** pour notification de changements
 * - Pas de dépendance UI (découplage total)
 *
 * ## Utilisation
 * ```cpp
 * EditorService* editor = new EditorService();
 * editor->setActiveTool(rectangleTool);
 * editor->addShape(myRectangle);
 * editor->undo();  // Annule la dernière commande
 * ```
 *
 * @note Thread-safe pour les opérations de lecture
 */
class EditorService : public QObject
{
    Q_OBJECT

    Q_PROPERTY(LaserCutStudio::Core::Editor::ITool* activeTool READ getActiveTool NOTIFY activeToolChanged)
    Q_PROPERTY(LaserCutStudio::Core::Editor::ISelection* selection READ getSelection CONSTANT)
    Q_PROPERTY(int shapeCount READ getShapeCount NOTIFY shapeCountChanged)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY undoStackChanged)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY undoStackChanged)
    Q_PROPERTY(int undoStackSize READ getUndoStackSize NOTIFY undoStackChanged)
    Q_PROPERTY(QStringList availableShapeTypes READ getAvailableShapeTypes CONSTANT)

public:
    explicit EditorService(QObject* parent = nullptr);
    ~EditorService() override;

    // ===== Gestion des outils =====

    /**
     * @brief Outil actuellement actif
     */
    Editor::ITool* getActiveTool() const { return m_activeTool; }

    /**
     * @brief Active un nouvel outil
     * @param tool Outil à activer (nullptr pour désactiver)
     */
    void setActiveTool(Editor::ITool* tool);

    /**
     * @brief Liste de tous les outils disponibles
     */
    QVector<Editor::ITool*> getAvailableTools() const { return m_availableTools; }

    /**
     * @brief Enregistre un nouvel outil
     */
    void registerTool(Editor::ITool* tool);

    /**
     * @brief Active un outil par son nom de forme
     * @param shapeType Nom du type de forme (ex: "Rectangle", "Circle")
     * @return true si l'outil a été trouvé et activé, false sinon
     */
    Q_INVOKABLE bool activateToolByShapeType(const QString& shapeType);

    /**
     * @brief Active l'outil de sélection
     * @return true si SelectionTool a été activé, false sinon
     */
    Q_INVOKABLE bool activateSelectionTool();

    // ===== Gestion de la sélection =====

    /**
     * @brief Accès au gestionnaire de sélection
     */
    Editor::ISelection* getSelection() const { return m_selection; }

    // ===== Gestion des formes =====

    /**
     * @brief Liste de toutes les formes dans l'éditeur
     */
    QVector<IShape*> getShapes() const { return m_shapes; }

    /**
     * @brief Nombre de formes
     */
    int getShapeCount() const { return m_shapes.size(); }

    /**
     * @brief Types de formes disponibles (enregistrés dans le Factory)
     * @return Liste des noms de types (ex: "Rectangle", "Circle")
     */
    QStringList getAvailableShapeTypes() const;

    /**
     * @brief Ajoute une forme à l'éditeur
     * @param shape Forme à ajouter
     * @param createCommand Si true, crée une commande Undo/Redo
     */
    void addShape(IShape* shape, bool createCommand = true);

    /**
     * @brief Ajoute une forme directement sans créer de commande
     * @param shape Forme à ajouter
     * @note Utilisé par les commandes Undo/Redo
     */
    void addShapeDirect(IShape* shape);

    /**
     * @brief Retire une forme de l'éditeur
     * @param shape Forme à retirer
     * @param createCommand Si true, crée une commande Undo/Redo
     */
    void removeShape(IShape* shape, bool createCommand = true);

    /**
     * @brief Retire une forme directement sans créer de commande
     * @param shape Forme à retirer
     * @note Utilisé par les commandes Undo/Redo
     */
    void removeShapeDirect(IShape* shape);

    /**
     * @brief Supprime toutes les formes
     */
    void clear();

    // ===== Undo/Redo =====

    /**
     * @brief Vérifie si Undo est possible
     */
    bool canUndo() const;

    /**
     * @brief Vérifie si Redo est possible
     */
    bool canRedo() const;

    /**
     * @brief Taille de la pile Undo
     */
    int getUndoStackSize() const;

    /**
     * @brief Taille de la pile Redo
     */
    int getRedoStackSize() const;

    /**
     * @brief Annule la dernière commande
     */
    Q_INVOKABLE void undo();

    /**
     * @brief Refait la dernière commande annulée
     */
    Q_INVOKABLE void redo();

    /**
     * @brief Supprime les formes sélectionnées avec Undo/Redo
     */
    Q_INVOKABLE void deleteSelectedShapes();

    /**
     * @brief Copie les formes sélectionnées dans le presse-papier
     * @return Nombre de formes copiées
     */
    Q_INVOKABLE int copySelectedShapes();

    /**
     * @brief Colle les formes du presse-papier avec Undo/Redo
     * @return Nombre de formes collées
     */
    Q_INVOKABLE int pasteShapes();

    /**
     * @brief Vérifie si le presse-papier contient des formes
     */
    Q_INVOKABLE bool hasClipboardData() const;

    /**
     * @brief Récupère les propriétés de la sélection pour édition
     * @return QVariantMap avec les propriétés (vide si sélection multiple ou vide)
     */
    Q_INVOKABLE QVariantMap getSelectionProperties() const;

    /**
     * @brief Modifie une propriété de la sélection avec Undo/Redo
     * @param propertyName Nom de la propriété (ex: "x", "y", "width")
     * @param value Nouvelle valeur
     * @return true si la modification a réussi
     */
    Q_INVOKABLE bool setSelectionProperty(const QString& propertyName, const QVariant& value);

    /**
     * @brief Ajoute une commande au stack
     * @param command Commande à ajouter (ownership transféré)
     */
    void pushCommand(Editor::IEditorCommand* command);

    /**
     * @brief Vide les stacks Undo/Redo
     */
    void clearUndoStack();

    /**
     * @brief Limite maximale de commandes dans le stack
     */
    int getUndoLimit() const;
    void setUndoLimit(int limit);

signals:
    /**
     * @brief Émis quand l'outil actif change
     */
    void activeToolChanged(LaserCutStudio::Core::Editor::ITool* tool);

    /**
     * @brief Émis quand une forme est ajoutée
     */
    void shapeAdded(LaserCutStudio::Core::IShape* shape);

    /**
     * @brief Émis quand une forme est retirée
     */
    void shapeRemoved(LaserCutStudio::Core::IShape* shape);

    /**
     * @brief Émis quand le nombre de formes change
     */
    void shapeCountChanged(int count);

    /**
     * @brief Émis quand le stack Undo/Redo change
     */
    void undoStackChanged();

    /**
     * @brief Émis après un Undo
     */
    void undone();

    /**
     * @brief Émis après un Redo
     */
    void redone();

    /**
     * @brief Émis pour afficher un message de statut
     */
    void statusMessage(const QString& message);

    /**
     * @brief Émis quand la sélection change
     */
    void selectionChanged();

private:
    // Outil actif
    Editor::ITool* m_activeTool = nullptr;

    // Tous les outils disponibles
    QVector<Editor::ITool*> m_availableTools;

    // Gestionnaire de sélection
    Editor::ISelection* m_selection = nullptr;

    // Formes dans l'éditeur
    QVector<IShape*> m_shapes;

    // Stack Undo/Redo (remplace QStack manuel par CommandStack)
    Editor::CommandStack* m_commandStack = nullptr;

    // Presse-papier (stocke QVariantMap pour sérialisation)
    QVector<QVariantMap> m_clipboard;
};

} // namespace Services
} // namespace Core
} // namespace LaserCutStudio

#endif // EDITORSERVICE_H
