#include "Interface.h"
#include <QMetaObject>
#include <QMetaProperty>

namespace LaserCutStudio {
namespace Core {

Interface::Interface(QObject* parent)
    : QObject(parent)  // Initialise QObject avec parent
    , m_id(QUuid::createUuid())
{
}

Interface::Interface(const Interface& other)
    : QObject(nullptr)  // Nouvel objet QObject indépendant (pas de copie de QObject)
    , m_id(QUuid::createUuid())  // Génère un nouvel UUID pour le clone
{
    // Lors du clonage, on génère un nouvel UUID unique
    // QObject n'est pas copié car il n'est pas copiable
    // Seules les données métier des classes dérivées seront copiées
}

Interface::~Interface()
{
    // Émet le signal avant destruction pour permettre le nettoyage
    emit aboutToBeDestroyed(this);
}

QVariantMap Interface::toVariant() const
{
    QVariantMap map;

    // Ajoute le type
    map["type"] = getTypeName();

    // Utilise le système Q_PROPERTY pour sérialiser automatiquement
    const QMetaObject* meta = metaObject();

    // Parcourt toutes les propriétés déclarées
    for (int i = meta->propertyOffset(); i < meta->propertyCount(); ++i) {
        QMetaProperty prop = meta->property(i);

        // Ne sérialise que les propriétés stockées (pas les calculées)
        if (prop.isStored()) {
            QString propName = QString::fromUtf8(prop.name());
            QVariant value = prop.read(this);

            // Exclut l'id (déjà géré par Interface) et objectName (interne Qt)
            if (propName != "id" && propName != "objectName") {
                map[propName] = value;
            }
        }
    }

    return map;
}

} // namespace Core
} // namespace LaserCutStudio
