/**
 * @file ScaleCommand.h
 * @brief Commande pour mettre à l'échelle des formes avec Undo/Redo
 */

#ifndef SCALECOMMAND_H
#define SCALECOMMAND_H

#include "core/models/editor/interfaces/IEditorCommand.h"
#include "core/models/shapes/interfaces/IShape.h"
#include "core/models/base/types/Point2D.h"
#include <QVector>
#include <QMap>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

/**
 * @class ScaleCommand
 * @brief Commande pour mettre à l'échelle une ou plusieurs formes
 *
 * ScaleCommand enregistre la mise à l'échelle de formes et permet de l'annuler/refaire.
 * Supporte la mise à l'échelle multiple (plusieurs formes en même temps).
 *
 * ## Architecture
 * - Stocke les facteurs d'échelle (X, Y) et le centre de mise à l'échelle
 * - Enregistre les positions initiales de chaque forme
 * - Applique les facteurs lors de execute/redo
 * - Restaure les positions lors de undo
 *
 * ## Fusion de commandes
 * - Supporte la fusion de mises à l'échelle consécutives sur les mêmes formes
 * - Les facteurs sont multipliés lors de la fusion
 *
 * ## Utilisation
 * ```cpp
 * QVector<IShape*> shapes = selection->getSelectedShapes();
 * Point2D center = selection->getCenter();
 * auto* cmd = new ScaleCommand(shapes, 1.5, 1.5, center);
 * editorService->pushCommand(cmd);
 * ```
 */
class ScaleCommand : public IEditorCommand
{
    Q_OBJECT

public:
    /**
     * @brief Constructeur
     * @param shapes Formes à mettre à l'échelle
     * @param scaleX Facteur d'échelle horizontal (1.0 = 100%)
     * @param scaleY Facteur d'échelle vertical (1.0 = 100%)
     * @param center Centre de mise à l'échelle
     * @param parent Parent Qt
     */
    explicit ScaleCommand(const QVector<IShape*>& shapes,
                         double scaleX,
                         double scaleY,
                         const Point2D& center,
                         QObject* parent = nullptr);

    ~ScaleCommand() override;

    // ===== Type name (Factory Pattern) =====
    DECLARE_TYPE_NAME(ScaleCommand)

    // ===== Prototype Pattern =====
    IEditorCommand* clone() const override;

    // ===== IEditorCommand interface =====
    QString getText() const override;
    void execute() override;
    void undo() override;
    void redo() override;
    bool isObsolete() const override;
    bool canMerge() const override;
    bool mergeWith(IEditorCommand* other) override;
    QString getCommandId() const override;

private:
    QVector<IShape*> m_shapes;  // Formes à mettre à l'échelle
    double m_scaleX;            // Facteur d'échelle X
    double m_scaleY;            // Facteur d'échelle Y
    Point2D m_center;           // Centre de mise à l'échelle
    bool m_executed;            // État d'exécution

    // Positions et tailles initiales (pour undo)
    QMap<IShape*, QPair<double, double>> m_initialPositions;
};

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio

#endif // SCALECOMMAND_H
