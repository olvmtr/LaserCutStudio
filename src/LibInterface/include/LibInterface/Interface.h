#ifndef LIBINTERFACE_INTERFACE_H
#define LIBINTERFACE_INTERFACE_H

#include <QObject>
#include <QString>
#include <QUuid>
#include <QVariantMap>
#include <memory>

namespace LibInterface {

/**
 * @brief Classe de base abstraite pour le pattern Prototype
 *
 * Cette classe implémente le pattern Prototype permettant de cloner
 * des objets sans connaître leur type concret. Toutes les interfaces
 * du système héritent de cette classe.
 *
 * Hérite de QObject pour bénéficier du système de Signals/Slots,
 * des propriétés Qt (Q_PROPERTY) et du système de métadonnées (QMetaObject).
 */
class Interface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUuid id READ getId CONSTANT)
signals:
    /**
     * @brief Signal émis juste avant la destruction de l'objet
     * Permet aux observateurs de se déconnecter proprement
     */
    void aboutToBeDestroyed(Interface* self);

public:
    /**
     * @brief Destructeur virtuel public (permet la destruction polymorphe)
     * Émet le signal aboutToBeDestroyed avant destruction
     */
    virtual ~Interface();

    /**
     * @brief Clone l'objet (Pattern Prototype)
     * @return Pointeur vers une nouvelle instance clonée
     */
    virtual Interface* clone() const = 0;

    /**
     * @brief Obtient l'UUID unique de l'instance
     * @return UUID de l'instance
     */
    QUuid getId() const { return m_id; }

    /**
     * @brief Obtient l'UUID sous forme de chaîne
     * @return UUID formaté en QString
     */
    QString getIdAsString() const { return m_id.toString(); }

    /**
     * @brief Retourne le nom du type de l'objet
     *
     * Cette méthode doit être implémentée par toutes les classes dérivées
     * pour retourner leur nom de type (ex: "Rectangle", "Circle", "TabJoint", etc.)
     *
     * @return Nom du type de l'objet
     */
    virtual QString getTypeName() const = 0;

    /**
     * @brief Sérialise l'objet en QVariantMap
     *
     * Utilise le système Q_PROPERTY de Qt pour sérialiser automatiquement
     * toutes les propriétés de l'objet. Les propriétés "id" et "objectName"
     * sont exclues de la sérialisation.
     *
     * @return QVariantMap contenant le type et toutes les propriétés
     *
     * @note Les classes dérivées peuvent surcharger cette méthode pour
     *       ajouter une sérialisation personnalisée
     */
    virtual QVariantMap toVariant() const;

protected:
    /**
     * @brief Constructeur par défaut
     * Génère automatiquement un UUID unique pour l'instance
     * @param parent Parent QObject (nullptr par défaut pour gestion manuelle)
     */
    explicit Interface(QObject* parent = nullptr);

    /**
     * @brief Constructeur de copie
     * Note: QObject n'est pas copiable, donc on crée un nouvel objet indépendant
     * Seul l'UUID est régénéré (nouveau UUID pour le clone)
     * @param other Instance à copier
     */
    Interface(const Interface& other);

    /**
     * @brief UUID unique de l'instance
     */
    QUuid m_id;
};

/**
 * @brief Macro pour déclarer automatiquement staticTypeName() et getTypeName()
 *
 * Cette macro génère les deux méthodes requises pour le Factory Pattern :
 * - staticTypeName() : retourne le nom de type en tant que méthode statique
 * - getTypeName() : retourne le nom de type pour l'instance (appelle staticTypeName())
 *
 * Usage :
 * @code
 * class Rectangle : public IShape {
 * public:
 *     DECLARE_TYPE_NAME(Rectangle)
 *     // ... reste de la classe
 * };
 * @endcode
 *
 * Cette macro génère :
 * @code
 * static QString staticTypeName() { return "Rectangle"; }
 * QString getTypeName() const override { return staticTypeName(); }
 * @endcode
 *
 * @param TypeName Nom du type (sans guillemets)
 *
 * @note La macro doit être placée dans la partie publique de la classe
 * @note Similaire au système Q_OBJECT de Qt
 */
#define DECLARE_TYPE_NAME(TypeName) \
    static QString staticTypeName() { return #TypeName; } \
    QString getTypeName() const override { return staticTypeName(); }

} // namespace LibInterface

#endif // LIBINTERFACE_INTERFACE_H
