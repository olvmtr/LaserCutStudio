#include "Interface.h"

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

} // namespace Core
} // namespace LaserCutStudio
