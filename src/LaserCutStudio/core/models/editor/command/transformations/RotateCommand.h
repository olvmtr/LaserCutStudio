/**
 * @file RotateCommand.h
 * @brief Commande pour faire pivoter des formes avec Undo/Redo
 */

#ifndef ROTATECOMMAND_H
#define ROTATECOMMAND_H

#include "core/models/editor/command/transformations/ITransformationCommand.h"
#include "core/models/shapes/IShape.h"
#include "core/models/base/types/Point2D.h"
#include <QVector>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

/**
 * @class RotateCommand
 * @brief Commande pour faire pivoter une ou plusieurs formes
 *
 * RotateCommand enregistre la rotation de formes et permet de l'annuler/refaire.
 * Supporte la rotation multiple (plusieurs formes en même temps).
 *
 * ## Architecture
 * - Hérite de ITransformationCommand (interface avec ListManagerMixin)
 * - Stocke l'angle de rotation et le centre de rotation
 * - Applique l'angle lors de execute/redo
 * - Restaure les positions initiales lors de undo
 *
 * ## Fusion de commandes
 * - Supporte la fusion de rotations consécutives sur les mêmes formes
 * - Les angles sont additionnés lors de la fusion
 * - Le centre de rotation doit être identique pour fusionner
 *
 * ## Utilisation
 * ```cpp
 * QVector<IShape*> shapes = selection->getSelectedShapes();
 * Point2D center = selection->getCenter();
 * auto* cmd = new RotateCommand(shapes, 45.0, center);
 * editorService->pushCommand(cmd);
 * ```
 */
class RotateCommand : public Transformations::ITransformationCommand
{
    Q_OBJECT

public:
    /**
     * @brief Constructeur
     * @param shapes Formes à faire pivoter
     * @param angleDegrees Angle de rotation en degrés (sens anti-horaire)
     * @param center Centre de rotation
     * @param parent Parent Qt
     */
    explicit RotateCommand(const QVector<IShape*>& shapes,
                          double angleDegrees,
                          const Point2D& center,
                          QObject* parent = nullptr);

    ~RotateCommand() override;

    // ===== Type name (Factory Pattern) =====
    DECLARE_TYPE_NAME(RotateCommand)

    // ===== Prototype Pattern =====
    IEditorCommand* clone() const override;

    // ===== IEditorCommand interface =====
    QString getText() const override;
    void execute() override;
    void undo() override;
    void redo() override;
    bool mergeWith(IEditorCommand* other) override;
    QString getCommandId() const override;

private:
    double m_angleDegrees;  ///< Angle de rotation (degrés)
    Point2D m_center;       ///< Centre de rotation
};

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio

#endif // ROTATECOMMAND_H
