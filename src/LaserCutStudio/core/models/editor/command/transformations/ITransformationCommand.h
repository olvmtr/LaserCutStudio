/**
 * @file ITransformationCommand.h
 * @brief Interface pour les commandes de transformation géométrique
 *
 * ITransformationCommand est une sous-interface de IEditorCommand
 * qui factorise le code commun aux commandes MoveCommand, RotateCommand
 * et ScaleCommand pour réduire la duplication.
 *
 * ## Pattern Architectural
 * - Hérite de IEditorCommand (interface principale)
 * - Utilise FactoryMixin pour création polymorphe (Factory Pattern)
 * - Utilise ListManagerMixin pour introspection des instances
 * - Fournit des helpers protected pour validation et état
 * - Organisée dans le namespace Transformations pour refléter la hiérarchie
 *
 * ## Cohérence avec autres interfaces
 * Comme IShape, IPart, IJoint, IProject, cette interface :
 * - Suit la convention de nommage "I*"
 * - Possède FactoryMixin pour création polymorphe (create, availableTypes, registerFactory)
 * - Possède ListManagerMixin pour tracking des instances
 * - Permet l'introspection (instanceCount, getAllInstances, clearAllInstances)
 */

#ifndef ITRANSFORMATIONCOMMAND_H
#define ITRANSFORMATIONCOMMAND_H

#include "core/models/editor/command/IEditorCommand.h"
#include "core/models/shapes/IShape.h"
#include "core/models/patterns/factory/FactoryMixin.h"
#include "core/models/patterns/lists/ListManagerMixin.h"
#include <QVector>
#include <QMap>
#include <QPair>

namespace LaserCutStudio {
namespace Core {
namespace Editor {
namespace Transformations {  // ⭐ Nouveau sous-namespace pour sous-interface

/**
 * @class ITransformationCommand
 * @brief Interface pour les commandes de transformation géométrique
 *
 * Fournit l'implémentation commune pour :
 * - Gestion de la liste des formes transformées
 * - Validation d'état (executed/non-executed)
 * - Détection d'obsolescence (formes détruites)
 * - Comparaison de listes de formes (pour fusion)
 * - Enregistrement des positions initiales
 * - **Introspection via ListManagerMixin** (tracking automatique des instances)
 *
 * ## Architecture
 * Les classes dérivées (MoveCommand, RotateCommand, ScaleCommand) doivent :
 * - Implémenter les méthodes abstraites de transformation (execute, undo, redo)
 * - Appeler les helpers de validation avant transformation
 * - Utiliser les helpers de comparaison pour la fusion
 *
 * ## Factory Pattern (création polymorphe)
 * ITransformationCommand supporte maintenant le Factory Pattern :
 * @code
 * using namespace LaserCutStudio::Core::Editor::Transformations;
 *
 * // Créer une commande depuis un QVariantMap
 * QVariantMap params;
 * params["type"] = "MoveCommand";
 * params["dx"] = 10.0;
 * params["dy"] = 20.0;
 * // Note: shapes et service injectés via ServiceLocator
 *
 * ITransformationCommand* cmd = ITransformationCommand::create(params);
 *
 * // Lister les types disponibles
 * QStringList types = ITransformationCommand::availableTypes();
 * // => ["MoveCommand", "RotateCommand", "ScaleCommand"]
 * @endcode
 *
 * ## Introspection
 * Comme toutes les interfaces principales, ITransformationCommand permet :
 * @code
 * using namespace LaserCutStudio::Core::Editor::Transformations;
 *
 * // Compter les commandes de transformation actives
 * int count = ITransformationCommand::instanceCount();
 *
 * // Lister toutes les instances
 * QList<ITransformationCommand*> commands = ITransformationCommand::getAllInstances();
 *
 * // Nettoyer toutes les instances
 * ITransformationCommand::clearAllInstances();
 * @endcode
 *
 * ## Avantages
 * - Élimine ~75 lignes de duplication entre les 3 commandes
 * - Code de validation centralisé et cohérent
 * - **Tracking automatique des instances** (pattern cohérent avec IShape, IPart, etc.)
 * - Facilite l'ajout de nouvelles commandes de transformation
 * - Permet la gestion globale des commandes de transformation
 * - **Organisation hiérarchique claire** via namespace Transformations
 */
class ITransformationCommand : public IEditorCommand,
                                protected Patterns::FactoryMixin<ITransformationCommand>,
                                protected Patterns::ListManagerMixin<ITransformationCommand>
{
    Q_OBJECT

public:
    explicit ITransformationCommand(const QVector<IShape*>& shapes,
                                   QObject* parent = nullptr);
    ~ITransformationCommand() override;

    // ===== IEditorCommand interface (implémentation commune) =====

    /**
     * @brief Vérifie si la commande est obsolète
     *
     * Une commande de transformation est obsolète si toutes ses formes
     * ont été détruites.
     *
     * @return true si toutes les formes sont nullptr
     */
    bool isObsolete() const override;

    /**
     * @brief Toutes les commandes de transformation supportent la fusion
     * @return true
     */
    bool canMerge() const override { return true; }

    // ===== FactoryMixin - Factory Pattern =====

    /**
     * @brief Expose les méthodes de FactoryMixin pour création polymorphe
     *
     * Ces méthodes permettent de :
     * - Créer des commandes de transformation depuis QVariantMap (create)
     * - Lister les types enregistrés (availableTypes)
     * - Enregistrer de nouveaux types (registerFactory)
     */
    using FactoryMixin<ITransformationCommand>::create;
    using FactoryMixin<ITransformationCommand>::availableTypes;
    using FactoryMixin<ITransformationCommand>::registerFactory;

    /**
     * @brief Enregistre une factory function personnalisée
     *
     * Utilisé pour les commandes qui ne peuvent pas utiliser le Factory Pattern
     * standard (dépendances runtime complexes).
     *
     * @param typeName Nom du type
     * @param factory Factory function personnalisée
     * @return true
     */
    static bool registerCustomFactory(const QString& typeName,
                                      std::function<ITransformationCommand*(const QVariantMap&)> factory);

    // ===== ListManagerMixin - Introspection des instances =====

    /**
     * @brief Expose les méthodes de ListManagerMixin pour introspection publique
     *
     * Ces méthodes permettent de :
     * - Compter les commandes de transformation actives (instanceCount)
     * - Lister toutes les instances (getAllInstances)
     * - Nettoyer la liste (clearAllInstances)
     */
    using ListManagerMixin<ITransformationCommand>::getAllInstances;
    using ListManagerMixin<ITransformationCommand>::clearAllInstances;
    using ListManagerMixin<ITransformationCommand>::instanceCount;

    /**
     * @brief Alias pour compatibilité : obtient toutes les commandes de transformation
     * @return Liste de toutes les commandes de transformation actives
     */
    static QList<ITransformationCommand*> getAllTransformationCommands() {
        return getAllInstances();
    }

    /**
     * @brief Alias pour compatibilité : vide la liste des commandes
     */
    static void clearAllTransformationCommands() {
        clearAllInstances();
    }

protected:
    // ===== État de la commande =====

    QVector<IShape*> m_shapes;  ///< Formes à transformer
    bool m_executed;            ///< État d'exécution (true après execute/redo)

    /// Positions initiales (coin supérieur gauche de bbox) pour undo
    QMap<IShape*, QPair<double, double>> m_initialPositions;

    // ===== Helpers de validation =====

    /**
     * @brief Valide l'état avant execute() ou redo()
     *
     * Vérifie que :
     * - La commande n'a pas déjà été exécutée (pour execute)
     * - Ou a déjà été exécutée (pour redo)
     * - Les formes ne sont pas vides
     *
     * @param isRedo true si appelé depuis redo(), false si depuis execute()
     * @return true si l'état est valide, false sinon (avec log d'erreur)
     */
    bool validateTransformState(bool isRedo = false) const;

    /**
     * @brief Valide l'état avant undo()
     *
     * Vérifie que :
     * - La commande a été exécutée
     * - Les formes ne sont pas vides
     *
     * @return true si l'état est valide, false sinon (avec log d'erreur)
     */
    bool validateUndoState() const;

    // ===== Helpers de comparaison =====

    /**
     * @brief Compare deux listes de formes
     *
     * Vérifie que les deux listes contiennent exactement les mêmes formes
     * (même taille, mêmes pointeurs).
     *
     * @param other Autre liste à comparer
     * @return true si les listes sont identiques
     */
    bool shapesMatch(const QVector<IShape*>& other) const;

    // ===== Helpers d'enregistrement =====

    /**
     * @brief Enregistre les positions initiales de toutes les formes
     *
     * Stocke le coin supérieur gauche de la bounding box de chaque forme.
     * Utilisé pour restaurer la position lors de undo().
     *
     * @note Appelé automatiquement par le constructeur
     */
    void recordInitialPositions();

    /**
     * @brief Marque la commande comme exécutée et émet le signal
     *
     * Appelé à la fin de execute() et redo().
     */
    void markExecuted();

    /**
     * @brief Marque la commande comme non-exécutée et émet le signal
     *
     * Appelé à la fin de undo().
     */
    void markUndone();
};

} // namespace Transformations
} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio

#endif // ITRANSFORMATIONCOMMAND_H
