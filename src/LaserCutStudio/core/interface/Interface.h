#ifndef INTERFACE_H
#define INTERFACE_H

#include <QObject>
#include <QString>
#include <QUuid>
#include <QVariantMap>
#include <memory>

namespace LaserCutStudio {
namespace Core {

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

} // namespace Core
} // namespace LaserCutStudio

#endif // INTERFACE_H
