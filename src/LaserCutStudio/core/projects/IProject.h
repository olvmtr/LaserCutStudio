#ifndef IPROJECT_H
#define IPROJECT_H

#include "../interface/Interface.h"
#include "../parts/IPart.h"
#include <QString>
#include <QList>
#include <QDateTime>
#include <QMap>
#include <QVariantMap>
#include <QMetaObject>
#include <QMetaProperty>
#include <functional>

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Métadonnées d'un projet
 */
struct ProjectMetadata
{
    QString author;                  ///< Auteur du projet
    QString description;             ///< Description
    QDateTime creationDate;          ///< Date de création
    QDateTime lastModifiedDate;      ///< Date de dernière modification
    QString version;                 ///< Version du projet

    ProjectMetadata()
        : author("Unknown")
        , description("")
        , creationDate(QDateTime::currentDateTime())
        , lastModifiedDate(QDateTime::currentDateTime())
        , version("1.0")
    {}
};

/**
 * @brief Interface pour un projet complet
 *
 * Un projet contient toutes les pièces et les informations
 * nécessaires pour un assemblage complet.
 * Hérite de Interface (donc QObject) pour bénéficier des Signals/Slots.
 */
class IProject : public Interface
{
    Q_OBJECT

signals:
    /**
     * @brief Signal émis lorsqu'une pièce est ajoutée au projet
     */
    void partAdded(IPart* part);

    /**
     * @brief Signal émis lorsqu'une pièce est retirée du projet
     */
    void partRemoved(IPart* part);

    /**
     * @brief Signal émis lorsque les métadonnées changent
     */
    void metadataChanged();

public:
    /**
     * @brief Destructeur virtuel public (permet la destruction polymorphe)
     */
    virtual ~IProject();

    /**
     * @brief Clone le projet
     */
    virtual IProject* clone() const override = 0;

    /**
     * @brief Obtient le nom du projet
     */
    QString getName() const { return m_name; }

    /**
     * @brief Définit le nom du projet
     */
    void setName(const QString& name) { m_name = name; }

    /**
     * @brief Obtient les métadonnées
     */
    ProjectMetadata getMetadata() const { return m_metadata; }

    /**
     * @brief Définit les métadonnées
     */
    void setMetadata(const ProjectMetadata& metadata) { m_metadata = metadata; }

    /**
     * @brief Ajoute une pièce au projet
     */
    void addPart(IPart* part);

    /**
     * @brief Retire une pièce du projet
     */
    void removePart(IPart* part);

    /**
     * @brief Obtient toutes les pièces du projet
     */
    QList<IPart*> getParts() const { return m_parts; }

    /**
     * @brief Vérifie si le projet est vide
     */
    bool isEmpty() const { return m_parts.isEmpty(); }

    /**
     * @brief Obtient le nombre de pièces
     */
    int getPartCount() const { return m_parts.count(); }

    /**
     * @brief Calcule le volume total du projet
     */
    double getTotalVolume() const;

    /**
     * @brief Calcule la masse totale du projet
     */
    double getTotalMass() const;

    /**
     * @brief Sauvegarde le projet
     */
    virtual bool save(const QString& filePath) const;

    /**
     * @brief Charge le projet
     */
    virtual bool load(const QString& filePath);

    // Factory Pattern avec QVariant
    /**
     * @brief Crée un projet depuis une configuration QVariant
     * @param config Configuration avec au minimum la clé "type"
     * @return Nouveau projet ou nullptr si type inconnu
     */
    static IProject* create(const QVariantMap& config);

    /**
     * @brief Liste tous les types de projets disponibles
     */
    static QStringList availableTypes();

    /**
     * @brief Sérialise le projet en QVariantMap pour sauvegarde/réseau
     */
    virtual QVariantMap toVariant() const;

    /**
     * @brief Retourne le nom du type (doit être implémenté par les classes concrètes)
     */
    virtual QString getTypeName() const = 0;

    // Gestion de la liste statique
    static QList<IProject*> getAllProjects() { return s_projects; }
    static void addProject(IProject* project);
    static void removeProject(IProject* project);
    static void clearAllProjects();

protected:
    IProject();
    IProject(const QString& name);
    IProject(const IProject& other);

    QString m_name;              ///< Nom du projet
    ProjectMetadata m_metadata;  ///< Métadonnées
    QList<IPart*> m_parts;       ///< Liste des pièces

    static QList<IProject*> s_projects; ///< Liste statique de tous les projets

    // Factory Pattern infrastructure
    using FactoryFunc = std::function<IProject*(const QVariantMap&)>;
    static QMap<QString, FactoryFunc> s_factories;

    /**
     * @brief Enregistre une classe concrète dans le Factory Pattern
     * Utilisé par les classes dérivées pour s'auto-enregistrer
     */
    template<typename T>
    static bool registerFactory() {
        s_factories[T::staticTypeName()] = [](const QVariantMap& params) {
            auto* obj = new T();
            // Utilise Q_PROPERTY pour configurer l'objet depuis params
            const QMetaObject* meta = obj->metaObject();
            for (auto it = params.constBegin(); it != params.constEnd(); ++it) {
                int propIndex = meta->indexOfProperty(it.key().toUtf8().constData());
                if (propIndex >= 0) {
                    QMetaProperty prop = meta->property(propIndex);
                    if (prop.isWritable()) {
                        prop.write(obj, it.value());
                    }
                }
            }
            return obj;
        };
        return true;
    }
};

} // namespace Core
} // namespace LaserCutStudio

#endif // IPROJECT_H
