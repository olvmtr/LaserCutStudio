#ifndef IPROJECT_H
#define IPROJECT_H

#include "core/models/base/Interface.h"
#include "core/infrastructure/patterns/factory/FactoryMixin.h"
#include "core/infrastructure/patterns/lists/ListManagerMixin.h"
#include "core/models/parts/interfaces/IPart.h"
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
 * @brief Interface pour un projet complet de découpe laser
 *
 * Un projet (Project) représente un assemblage complet contenant :
 * - Une collection de pièces (IPart) à découper
 * - Des métadonnées (auteur, description, dates, version)
 * - Un nom de projet
 *
 * Le projet est l'unité de sauvegarde/chargement de l'application.
 *
 * ## Caractéristiques
 *
 * - **Agrégation de pièces** : Contient 0..N IPart
 * - **Métadonnées** : ProjectMetadata avec auteur, description, dates
 * - **Sérialisation** : Support JSON/XML pour sauvegarde projet
 * - **Statistiques** : Calcul automatique volume, masse, nombre de pièces
 *
 * ## Patterns Architecturaux
 *
 * - **Aggregate Pattern** : Contient et gère une collection d'IPart
 * - **Prototype Pattern** : Hérite de Interface pour clonage polymorphe
 * - **Factory Pattern** : Utilise FactoryMixin pour création depuis QVariantMap
 * - **Signals/Slots** : Notifications lors ajout/retrait pièces
 *
 * ## Factory Pattern - Utilisation
 *
 * ### Création depuis QVariantMap
 *
 * @code
 * // Créer un projet complet avec plusieurs pièces
 * QVariantMap projectData;
 * projectData["type"] = "Project";
 * projectData["name"] = "Boîte à Outils";
 *
 * // Métadonnées
 * QVariantMap metaData;
 * metaData["author"] = "Jean Dupont";
 * metaData["description"] = "Boîte de rangement avec compartiments";
 * metaData["version"] = "2.1";
 * projectData["metadata"] = metaData;
 *
 * // Liste des pièces (array)
 * QVariantList partsList;
 *
 * // Pièce 1 : Fond
 * QVariantMap part1Data;
 * part1Data["type"] = "Part";
 * part1Data["name"] = "Fond";
 * part1Data["shape"] = rectangleShapeData;  // QVariantMap
 * part1Data["thickness"] = 6.0;
 * partsList.append(part1Data);
 *
 * // Pièce 2 : Côté gauche
 * QVariantMap part2Data;
 * // ... similaire
 * partsList.append(part2Data);
 *
 * projectData["parts"] = partsList;
 *
 * IProject* project = IProject::create(projectData);
 * if (project) {
 *     qDebug() << "Projet:" << project->getName();
 *     qDebug() << "Pièces:" << project->getPartCount();
 *     qDebug() << "Volume total:" << project->getTotalVolume() << "mm³";
 * }
 * @endcode
 *
 * ### Gestion de Collection
 *
 * @code
 * IProject* project = new Project("My Box");
 *
 * // Ajouter des pièces
 * IPart* bottom = new Part("Bottom", shape1, 6.0, Material::Plywood());
 * IPart* side = new Part("Side", shape2, 6.0, Material::Plywood());
 * project->addPart(bottom);
 * project->addPart(side);
 *
 * // Itérer sur les pièces
 * for (IPart* part : project->getParts()) {
 *     qDebug() << part->getName() << ":" << part->calculateVolume() << "mm³";
 * }
 *
 * // Statistiques agrégées
 * qDebug() << "Total volume:" << project->getTotalVolume();
 * qDebug() << "Total mass:" << project->getTotalMass();
 * @endcode
 *
 * ### Sauvegarde/Chargement
 *
 * @code
 * // Sauvegarder projet en JSON
 * IProject* project = /* ... */;
 * QVariantMap data = project->toVariant();
 * QJsonDocument doc = QJsonDocument::fromVariant(data);
 * QFile file("project.json");
 * file.open(QIODevice::WriteOnly);
 * file.write(doc.toJson());
 * file.close();
 *
 * // Charger projet depuis JSON
 * QFile loadFile("project.json");
 * loadFile.open(QIODevice::ReadOnly);
 * QJsonDocument loadDoc = QJsonDocument::fromJson(loadFile.readAll());
 * QVariantMap loadedData = loadDoc.toVariant().toMap();
 * IProject* loaded = IProject::create(loadedData);
 * @endcode
 *
 * ### Lister Types Disponibles
 *
 * @code
 * QStringList projectTypes = IProject::availableTypes();
 * // => ["Project"] (actuellement 1 type, extensible via plugins)
 * @endcode
 *
 * @note Ownership : IProject possède ses IPart (destructeur nettoie automatiquement)
 * @note Sérialisation : toVariant() sérialise récursivement toutes les pièces
 * @note Performance : getTotalVolume() et getTotalMass() calculent à la demande
 *
 * @see FactoryMixin, IPart, ProjectMetadata, Interface::toVariant()
 */
class IProject : public Interface,
                 protected Patterns::FactoryMixin<IProject>,
                 protected Patterns::ListManagerMixin<IProject>
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

    // Factory Pattern fourni par FactoryMixin
    using FactoryMixin<IProject>::create;
    using FactoryMixin<IProject>::availableTypes;
    using FactoryMixin<IProject>::registerFactory;

    /**
     * @brief Retourne le nom du type (doit être implémenté par les classes concrètes)
     */
    virtual QString getTypeName() const override = 0;

    // Gestion de la liste statique (fournie par ListManagerMixin)
    using ListManagerMixin<IProject>::getAllInstances;
    using ListManagerMixin<IProject>::clearAllInstances;
    using ListManagerMixin<IProject>::instanceCount;

    static QList<IProject*> getAllProjects() { return getAllInstances(); }
    static void clearAllProjects() { clearAllInstances(); }

protected:
    IProject();
    IProject(const QString& name);
    IProject(const IProject& other);

    QString m_name;              ///< Nom du projet
    ProjectMetadata m_metadata;  ///< Métadonnées
    QList<IPart*> m_parts;       ///< Liste des pièces
};

} // namespace Core
} // namespace LaserCutStudio

#endif // IPROJECT_H
