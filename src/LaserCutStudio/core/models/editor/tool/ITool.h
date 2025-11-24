/**
 * @file ITool.h
 * @brief Interface pour les outils d'édition 2D
 *
 * ITool définit le contrat pour tous les outils de dessin et d'édition
 * (Rectangle, Circle, Selection, etc.). Utilise le Factory Pattern via
 * FactoryMixin pour créer dynamiquement des outils depuis des configurations.
 */

#ifndef ITOOL_H
#define ITOOL_H

#include "core/models/base/Interface.h"
#include "core/models/patterns/factory/FactoryMixin.h"
#include "core/models/patterns/lists/ListManagerMixin.h"
#include "core/models/base/types/Point2D.h"
#include "core/models/shapes/IShape.h"
#include <QCursor>
#include <QIcon>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

/**
 * @class ITool
 * @brief Interface abstraite pour les outils d'édition 2D
 *
 * ITool représente un outil de dessin ou d'édition dans l'éditeur 2D.
 * Chaque outil gère les événements souris et peut créer/modifier des formes.
 *
 * ## Architecture
 * - Hérite de Interface (pattern Prototype)
 * - Utilise FactoryMixin pour le Factory Pattern
 * - Envoie des signaux Qt pour communication découplée
 *
 * ## Cycle de vie
 * 1. Activation : activate() appelé quand l'outil devient actif
 * 2. Événements : handleMousePress/Move/Release pour interactions
 * 3. Désactivation : deactivate() appelé avant changement d'outil
 *
 * @note Utilise le pattern Command pour les opérations annulables
 */
class ITool : public Interface,
              protected Patterns::FactoryMixin<ITool>,
              protected Patterns::ListManagerMixin<ITool>
{
    Q_OBJECT

    // Expose les méthodes du Factory Pattern
    using FactoryMixin<ITool>::create;
    using FactoryMixin<ITool>::availableTypes;
    using FactoryMixin<ITool>::registerFactory;

    // Expose les méthodes du List Manager
    using ListManagerMixin<ITool>::getAllInstances;
    using ListManagerMixin<ITool>::instanceCount;

    Q_PROPERTY(QString name READ getName NOTIFY nameChanged)
    Q_PROPERTY(QString description READ getDescription NOTIFY descriptionChanged)
    Q_PROPERTY(bool isActive READ isActive WRITE setActive NOTIFY activeChanged)

public:
    explicit ITool(QObject* parent = nullptr) : Interface(parent) {}
    ~ITool() override = default;

    /**
     * @brief Nom de l'outil (affiché dans l'UI)
     */
    virtual QString getName() const = 0;

    /**
     * @brief Description courte de l'outil
     */
    virtual QString getDescription() const = 0;

    /**
     * @brief Icône de l'outil pour la toolbar
     */
    virtual QIcon getIcon() const = 0;

    /**
     * @brief Curseur à afficher quand l'outil est actif
     */
    virtual QCursor getCursor() const = 0;

    /**
     * @brief Vérifie si l'outil est actif
     */
    virtual bool isActive() const = 0;

    /**
     * @brief Active ou désactive l'outil
     */
    virtual void setActive(bool active) = 0;

    /**
     * @brief Appelé quand l'outil devient actif
     */
    virtual void activate() = 0;

    /**
     * @brief Appelé quand l'outil devient inactif
     */
    virtual void deactivate() = 0;

    /**
     * @brief Gère l'événement mouse press
     * @param scenePos Position dans les coordonnées de la scène
     * @param button Bouton de souris pressé
     * @return true si l'événement est consommé
     */
    virtual bool handleMousePress(const Point2D& scenePos, Qt::MouseButton button) = 0;

    /**
     * @brief Gère l'événement mouse move
     * @param scenePos Position dans les coordonnées de la scène
     * @return true si l'événement est consommé
     */
    virtual bool handleMouseMove(const Point2D& scenePos) = 0;

    /**
     * @brief Gère l'événement mouse release
     * @param scenePos Position dans les coordonnées de la scène
     * @param button Bouton de souris relâché
     * @return true si l'événement est consommé
     */
    virtual bool handleMouseRelease(const Point2D& scenePos, Qt::MouseButton button) = 0;

    /**
     * @brief Gère l'événement key press (raccourcis clavier)
     * @param key Touche pressée
     * @param modifiers Modificateurs (Ctrl, Shift, etc.)
     * @return true si l'événement est consommé
     */
    virtual bool handleKeyPress(int key, Qt::KeyboardModifiers modifiers) = 0;

signals:
    /**
     * @brief Émis quand le nom change
     */
    void nameChanged(const QString& name);

    /**
     * @brief Émis quand la description change
     */
    void descriptionChanged(const QString& description);

    /**
     * @brief Émis quand l'état actif/inactif change
     */
    void activeChanged(bool active);

    /**
     * @brief Émis quand l'outil crée une nouvelle forme
     */
    void shapeCreated(IShape* shape);

    /**
     * @brief Émis quand l'outil modifie une forme
     */
    void shapeModified(IShape* shape);

    /**
     * @brief Émis pour afficher un message dans la barre de statut
     */
    void statusMessage(const QString& message);
};

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio

#endif // ITOOL_H
