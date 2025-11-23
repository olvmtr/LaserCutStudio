/**
 * @file TransformationCommandBase.h
 * @brief Classe de base pour les commandes de transformation géométrique
 *
 * TransformationCommandBase factorise le code commun aux commandes
 * MoveCommand, RotateCommand et ScaleCommand pour réduire la duplication.
 */

#ifndef TRANSFORMATIONCOMMANDBASE_H
#define TRANSFORMATIONCOMMANDBASE_H

#include "core/models/editor/command/IEditorCommand.h"
#include "core/models/shapes/IShape.h"
#include <QVector>
#include <QMap>
#include <QPair>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

/**
 * @class TransformationCommandBase
 * @brief Classe de base abstraite pour les commandes de transformation
 *
 * Fournit l'implémentation commune pour :
 * - Gestion de la liste des formes transformées
 * - Validation d'état (executed/non-executed)
 * - Détection d'obsolescence (formes détruites)
 * - Comparaison de listes de formes (pour fusion)
 * - Enregistrement des positions initiales
 *
 * ## Architecture
 * Les classes dérivées (MoveCommand, RotateCommand, ScaleCommand) doivent :
 * - Implémenter les méthodes abstraites de transformation
 * - Appeler les helpers de validation avant transformation
 * - Utiliser les helpers de comparaison pour la fusion
 *
 * ## Avantages
 * - Élimine ~75 lignes de duplication entre les 3 commandes
 * - Code de validation centralisé et cohérent
 * - Facilite l'ajout de nouvelles commandes de transformation
 */
class TransformationCommandBase : public IEditorCommand
{
    Q_OBJECT

public:
    explicit TransformationCommandBase(const QVector<IShape*>& shapes,
                                      QObject* parent = nullptr);
    ~TransformationCommandBase() override;

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

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio

#endif // TRANSFORMATIONCOMMANDBASE_H
