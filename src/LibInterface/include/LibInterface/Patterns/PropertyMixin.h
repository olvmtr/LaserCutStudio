#ifndef LIBINTERFACE_PROPERTYMIXIN_H
#define LIBINTERFACE_PROPERTYMIXIN_H

#include <QtCore/qglobal.h>

namespace LibInterface {

namespace Patterns {

/**
 * @brief Mixin CRTP pour faciliter l'implémentation de setters avec signaux Qt
 *
 * Ce template fournit des helpers pour implémenter le pattern standard
 * de setter Qt avec émission de signaux et validation de changement.
 *
 * Pattern typique :
 * @code
 * void MyClass::setX(double x) {
 *     if (!qFuzzyCompare(m_x, x)) {
 *         m_x = x;
 *         emit xChanged(x);
 *         emit geometryChanged();
 *     }
 * }
 * @endcode
 *
 * Avec PropertyMixin :
 * @code
 * void MyClass::setX(double x) {
 *     updateProperty(m_x, x, &MyClass::xChanged, &MyClass::geometryChanged);
 * }
 * @endcode
 *
 * @tparam Derived Classe dérivée (CRTP pattern)
 */
template<typename Derived>
class PropertyMixin
{
protected:
    /**
     * @brief Met à jour une propriété double et émet les signaux si changée
     *
     * Utilise qFuzzyCompare pour comparer les valeurs flottantes.
     * Si la valeur change, met à jour le membre et émet les signaux.
     *
     * @tparam Signal1 Type du signal spécifique (ex: void (MyClass::*)(double))
     * @tparam Signal2 Type du signal général (ex: void (MyClass::*)())
     * @param member Référence au membre de classe à mettre à jour
     * @param newValue Nouvelle valeur
     * @param specificSignal Signal spécifique à émettre (ex: xChanged)
     * @param generalSignal Signal général à émettre (ex: geometryChanged)
     *
     * @note Le signal spécifique reçoit la nouvelle valeur en paramètre
     */
    template<typename Signal1, typename Signal2>
    void updateProperty(double& member, double newValue,
                       Signal1 specificSignal, Signal2 generalSignal)
    {
        if (!qFuzzyCompare(member, newValue)) {
            member = newValue;
            // CRTP: this pointe vers PropertyMixin<Derived>, mais l'objet réel est Derived
            // reinterpret_cast est sûr ici car PropertyMixin est toujours une base de Derived
            Derived* derived = reinterpret_cast<Derived*>(this);
            emit (derived->*specificSignal)(newValue);
            emit (derived->*generalSignal)();
        }
    }

    /**
     * @brief Met à jour une propriété int et émet les signaux si changée
     *
     * Utilise la comparaison directe pour les entiers.
     * Si la valeur change, met à jour le membre et émet les signaux.
     *
     * @tparam Signal1 Type du signal spécifique
     * @tparam Signal2 Type du signal général
     * @param member Référence au membre de classe à mettre à jour
     * @param newValue Nouvelle valeur
     * @param specificSignal Signal spécifique à émettre
     * @param generalSignal Signal général à émettre
     */
    template<typename Signal1, typename Signal2>
    void updateProperty(int& member, int newValue,
                       Signal1 specificSignal, Signal2 generalSignal)
    {
        if (member != newValue) {
            member = newValue;
            Derived* derived = reinterpret_cast<Derived*>(this);
            emit (derived->*specificSignal)(newValue);
            emit (derived->*generalSignal)();
        }
    }

    /**
     * @brief Met à jour une propriété QString et émet les signaux si changée
     *
     * @tparam Signal1 Type du signal spécifique
     * @tparam Signal2 Type du signal général
     * @param member Référence au membre de classe à mettre à jour
     * @param newValue Nouvelle valeur
     * @param specificSignal Signal spécifique à émettre
     * @param generalSignal Signal général à émettre
     */
    template<typename Signal1, typename Signal2>
    void updateProperty(QString& member, const QString& newValue,
                       Signal1 specificSignal, Signal2 generalSignal)
    {
        if (member != newValue) {
            member = newValue;
            Derived* derived = reinterpret_cast<Derived*>(this);
            emit (derived->*specificSignal)(newValue);
            emit (derived->*generalSignal)();
        }
    }

    /**
     * @brief Met à jour une propriété double et émet un seul signal si changée
     *
     * Version simplifiée pour les cas où il n'y a qu'un signal à émettre.
     *
     * @tparam Signal Type du signal
     * @param member Référence au membre de classe à mettre à jour
     * @param newValue Nouvelle valeur
     * @param signal Signal à émettre
     */
    template<typename Signal>
    void updateProperty(double& member, double newValue, Signal signal)
    {
        if (!qFuzzyCompare(member, newValue)) {
            member = newValue;
            Derived* derived = reinterpret_cast<Derived*>(this);
            emit (derived->*signal)(newValue);
        }
    }

    /**
     * @brief Met à jour une propriété int et émet un seul signal si changée
     *
     * @tparam Signal Type du signal
     * @param member Référence au membre de classe à mettre à jour
     * @param newValue Nouvelle valeur
     * @param signal Signal à émettre
     */
    template<typename Signal>
    void updateProperty(int& member, int newValue, Signal signal)
    {
        if (member != newValue) {
            member = newValue;
            Derived* derived = reinterpret_cast<Derived*>(this);
            emit (derived->*signal)(newValue);
        }
    }

    /**
     * @brief Met à jour une propriété QString et émet un seul signal si changée
     *
     * @tparam Signal Type du signal
     * @param member Référence au membre de classe à mettre à jour
     * @param newValue Nouvelle valeur
     * @param signal Signal à émettre
     */
    template<typename Signal>
    void updateProperty(QString& member, const QString& newValue, Signal signal)
    {
        if (member != newValue) {
            member = newValue;
            Derived* derived = reinterpret_cast<Derived*>(this);
            emit (derived->*signal)(newValue);
        }
    }
};

} // namespace Patterns

} // namespace LibInterface

#endif // LIBINTERFACE_PROPERTYMIXIN_H
