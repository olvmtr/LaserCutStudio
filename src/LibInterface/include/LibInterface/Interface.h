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
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
signals:
    /**
     * @brief Signal émis juste avant la destruction de l'objet
     * Permet aux observateurs de se déconnecter proprement
     */
    void aboutToBeDestroyed(Interface* self);

    /**
     * @brief Signal émis quand le nom change
     */
    void nameChanged(const QString& name);

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
    Q_INVOKABLE virtual QString getTypeName() const = 0;

    /**
     * @brief Obtient le nom de l'objet
     * @return Nom de l'objet
     */
    QString getName() const { return m_name; }

    /**
     * @brief Définit le nom de l'objet
     * @param name Nouveau nom
     */
    void setName(const QString& name) {
        if (m_name != name) {
            m_name = name;
            emit nameChanged(m_name);
        }
    }

    /**
     * @brief Sérialise l'objet en QVariantMap via introspection Qt
     *
     * Utilise le système Q_PROPERTY de Qt pour sérialiser automatiquement
     * toutes les propriétés de l'objet. Les propriétés "id" et "objectName"
     * sont exclues de la sérialisation.
     *
     * @details
     * ## Fonctionnement Automatique
     *
     * Cette méthode utilise le système de réflexion Qt (QMetaObject) pour
     * parcourir automatiquement toutes les Q_PROPERTY et les sérialiser :
     *
     * 1. **Ajoute le champ "type"** : Appelle getTypeName() pour identifier le type concret
     * 2. **Parcourt toutes les Q_PROPERTY** : Via metaObject()->propertyCount()
     * 3. **Exclut propriétés système** : Ignore "id" et "objectName" (gérés séparément)
     * 4. **Sérialise chaque propriété** : Utilise property.read(this) pour obtenir la valeur
     * 5. **Support types Qt natifs** : int, double, QString, QColor, QRectF, etc.
     *
     * ## Exemple Automatique
     *
     * @code
     * // Classe avec Q_PROPERTY (définition)
     * class Rectangle : public IShape {
     *     Q_OBJECT
     *     Q_PROPERTY(double x READ getX WRITE setX)
     *     Q_PROPERTY(double width READ getWidth WRITE setWidth)
     * public:
     *     DECLARE_TYPE_NAME(Rectangle)
     *     // getX, setX, getWidth, setWidth...
     * };
     *
     * // Utilisation (sérialisation automatique)
     * Rectangle* rect = new Rectangle(10.0, 20.0, 100.0, 50.0);
     * QVariantMap data = rect->toVariant();
     * // => { "type": "Rectangle", "x": 10.0, "y": 20.0, "width": 100.0, "height": 50.0 }
     *
     * // Round-trip : désérialisation via Factory Pattern
     * IShape* clone = IShape::create(data);  // Recrée un Rectangle identique !
     * @endcode
     *
     * ## Symétrie avec Factory Pattern
     *
     * Cette méthode est **symétrique** avec FactoryMixin::create(QVariantMap) :
     * - toVariant() : Object → QVariantMap (sérialisation)
     * - create(map) : QVariantMap → Object (désérialisation)
     *
     * Cela permet le round-trip complet : clone == original
     *
     * ## Sérialisation Personnalisée
     *
     * Les classes dérivées peuvent surcharger pour ajouter des propriétés calculées :
     *
     * @code
     * QVariantMap Rectangle::toVariant() const {
     *     QVariantMap data = IShape::toVariant();  // Appel parent
     *     data["area"] = getArea();                // Propriété calculée
     *     data["perimeter"] = 2 * (width + height);
     *     return data;
     * }
     * @endcode
     *
     * @return QVariantMap contenant le type et toutes les propriétés Q_PROPERTY
     *
     * @note Zéro duplication : Pas besoin d'implémenter toVariant() dans chaque classe !
     * @note Performance : Introspection Qt est optimisée (cache métadonnées)
     * @note Limitation : Seuls les types supportés par QVariant sont sérialisables
     *
     * @see FactoryMixin::create(), DECLARE_TYPE_NAME, QMetaObject, Q_PROPERTY
     */
    Q_INVOKABLE virtual QVariantMap toVariant() const;

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

    /**
     * @brief Nom de l'objet (éditable par l'utilisateur)
     */
    QString m_name;
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
