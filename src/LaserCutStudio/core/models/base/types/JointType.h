#ifndef JOINTTYPE_H
#define JOINTTYPE_H

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Types d'assemblages disponibles pour connexion de pièces
 *
 * Chaque type d'assemblage a ses propres caractéristiques de résistance,
 * complexité de fabrication et use cases recommandés.
 */
enum class JointType
{
    /**
     * @brief Assemblage par tenon/encoche simple (tab joint)
     *
     * Joint perpendiculaire simple avec tenon s'insérant dans une encoche.
     * Utilisé pour assemblages perpendiculaires (coins de boîtes).
     * Résistance : Moyenne. Fabrication : Simple.
     */
    TAB,

    /**
     * @brief Assemblage à doigts entrelacés (finger joint / box joint)
     *
     * Série de tenons et encoches alternés formant un peigne.
     * Utilisé pour coins de boîtes avec grande résistance.
     * Résistance : Élevée. Fabrication : Moyenne.
     */
    FINGER,

    /**
     * @brief Assemblage mortaise et tenon traditionnel
     *
     * Tenon s'insérant dans une mortaise (trou rectangulaire).
     * Utilisé pour assemblages perpendiculaires renforcés.
     * Résistance : Très élevée. Fabrication : Complexe.
     */
    MORTISE_TENON,

    /**
     * @brief Assemblage bord à bord (edge-to-edge)
     *
     * Collage simple de deux bords parallèles.
     * Utilisé pour agrandir surface ou créer panneaux.
     * Résistance : Faible (collage uniquement). Fabrication : Très simple.
     */
    EDGE_TO_EDGE
};

} // namespace Core
} // namespace LaserCutStudio

#endif // JOINTTYPE_H
