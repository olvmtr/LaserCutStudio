#ifndef JOINTTYPE_H
#define JOINTTYPE_H

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Types d'assemblages disponibles
 */
enum class JointType
{
    TAB,            ///< Assemblage par tenon/encoche simple
    FINGER,         ///< Assemblage à doigts (finger joints)
    MORTISE_TENON,  ///< Assemblage mortaise et tenon
    EDGE_TO_EDGE    ///< Assemblage bord à bord
};

} // namespace Core
} // namespace LaserCutStudio

#endif // JOINTTYPE_H
