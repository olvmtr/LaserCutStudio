#include "Interface.h"

namespace LaserCutStudio {
namespace Core {

Interface::Interface()
    : m_id(QUuid::createUuid())
{
}

Interface::Interface(const Interface& other)
    : m_id(QUuid::createUuid()) // Génère un nouvel UUID pour le clone
{
    // Lors du clonage, on génère un nouvel UUID unique
    // même si on copie les autres propriétés
}

} // namespace Core
} // namespace LaserCutStudio
