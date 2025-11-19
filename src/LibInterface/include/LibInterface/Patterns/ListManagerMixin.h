#ifndef LISTMANAGERMIXIN_H
#define LISTMANAGERMIXIN_H

#include <QList>

namespace LibInterface {

namespace Patterns {

/**
 * @brief Mixin CRTP pour gérer automatiquement une liste statique d'instances
 *
 * Ce template élimine la duplication du code de gestion de liste statique
 * entre les différentes interfaces (IShape, IPart, IJoint, IProject).
 *
 * Usage:
 * @code
 * class IShape : public Interface, protected ListManagerMixin<IShape> {
 * public:
 *     using ListManagerMixin<IShape>::getAllInstances;
 *
 *     // Alias optionnel pour compatibilité
 *     static QList<IShape*> getAllShapes() { return getAllInstances(); }
 *
 * protected:
 *     IShape() : Interface() {
 *         registerInstance(this);
 *     }
 *
 *     ~IShape() {
 *         unregisterInstance(this);
 *     }
 * };
 * @endcode
 *
 * @tparam T Type de l'interface (IShape, IPart, etc.)
 */
template<typename T>
class ListManagerMixin
{
protected:
    /**
     * @brief Liste statique de toutes les instances créées
     *
     * Chaque instanciation du template (ListManagerMixin<IShape>,
     * ListManagerMixin<IPart>, etc.) a sa propre liste statique.
     */
    static QList<T*> s_instances;

    /**
     * @brief Enregistre une instance dans la liste statique
     *
     * À appeler dans le constructeur de la classe dérivée.
     * Vérifie que l'instance n'est pas déjà dans la liste avant ajout.
     *
     * @param instance Pointeur vers l'instance à enregistrer
     */
    void registerInstance(T* instance)
    {
        if (instance && !s_instances.contains(instance)) {
            s_instances.append(instance);
        }
    }

    /**
     * @brief Désenregistre une instance de la liste statique
     *
     * À appeler dans le destructeur de la classe dérivée.
     * Retire toutes les occurrences de l'instance (normalement une seule).
     *
     * @param instance Pointeur vers l'instance à désenregistrer
     */
    void unregisterInstance(T* instance)
    {
        s_instances.removeAll(instance);
    }

public:
    /**
     * @brief Retourne la liste de toutes les instances enregistrées
     * @return Liste de pointeurs vers toutes les instances
     */
    static QList<T*> getAllInstances()
    {
        return s_instances;
    }

    /**
     * @brief Vide la liste de toutes les instances
     *
     * @warning Cette méthode ne détruit pas les objets, elle vide
     *          seulement la liste. À utiliser avec précaution.
     */
    static void clearAllInstances()
    {
        s_instances.clear();
    }

    /**
     * @brief Retourne le nombre d'instances enregistrées
     * @return Nombre d'instances dans la liste
     */
    static int instanceCount()
    {
        return s_instances.count();
    }

    /**
     * @brief Vérifie si une instance est enregistrée
     * @param instance Instance à vérifier
     * @return true si l'instance est dans la liste
     */
    static bool isRegistered(T* instance)
    {
        return s_instances.contains(instance);
    }
};

/**
 * @brief Définition du membre statique template
 *
 * Cette définition doit être dans le .h car c'est un template.
 * Chaque instanciation du template (ListManagerMixin<IShape>,
 * ListManagerMixin<IPart>, etc.) aura sa propre s_instances statique.
 */
template<typename T>
QList<T*> ListManagerMixin<T>::s_instances;

} // namespace Patterns

} // namespace LibInterface

#endif // LISTMANAGERMIXIN_H
