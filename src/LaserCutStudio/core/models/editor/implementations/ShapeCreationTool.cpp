/**
 * @file ShapeCreationTool.cpp
 * @brief Implémentation de l'outil générique de création de formes
 */

#include "core/models/editor/implementations/ShapeCreationTool.h"
#include "core/services/editor/EditorService.h"
#include "core/infrastructure/patterns/prototype/ClonableMixin.h"
#include "core/infrastructure/logging/LogCategories.h"
#include <QDebug>
#include <QtMath>

namespace LaserCutStudio {
namespace Core {
namespace Editor {

// Auto-registration avec le Factory Pattern
namespace {
    Patterns::FactoryMixin<ITool>::AutoRegister<ShapeCreationTool> g_shapeCreationToolReg;
}

ShapeCreationTool::ShapeCreationTool(const QString& shapeType, QObject* parent)
    : ITool(parent)
    , m_isActive(false)
    , m_isDrawing(false)
    , m_shapeType(shapeType)
    , m_creationMode(CreationMode::ClickAndDrag)
    , m_previewShape(nullptr)
{
    // Enregistrement automatique dans la liste via ListManagerMixin
    registerInstance(this);

    qCInfo(logCore()) << "ShapeCreationTool created for type:" << m_shapeType;
}

ShapeCreationTool::~ShapeCreationTool()
{
    clearPreview();
    unregisterInstance(this);

    qCInfo(logCore()) << "ShapeCreationTool destroyed";
}

// ===== Macro pour clone() =====
IMPLEMENT_CLONE(ShapeCreationTool, ITool)

// ===== ITool interface =====

QString ShapeCreationTool::getName() const
{
    return QString("Create %1").arg(m_shapeType);
}

QString ShapeCreationTool::getDescription() const
{
    return QString("Create %1 shapes by clicking and dragging").arg(m_shapeType);
}

QIcon ShapeCreationTool::getIcon() const
{
    // TODO: Charger l'icône depuis les ressources
    return QIcon();
}

QCursor ShapeCreationTool::getCursor() const
{
    return Qt::CrossCursor;  // Curseur en croix pour le dessin
}

void ShapeCreationTool::setActive(bool active)
{
    if (m_isActive != active) {
        m_isActive = active;
        emit activeChanged(active);

        if (active) {
            qCInfo(logCore()) << "ShapeCreationTool activated for:" << m_shapeType;
        } else {
            qCInfo(logCore()) << "ShapeCreationTool deactivated";
        }
    }
}

void ShapeCreationTool::activate()
{
    setActive(true);
    emit statusMessage(QString("Click and drag to create %1").arg(m_shapeType));
}

void ShapeCreationTool::deactivate()
{
    // Nettoyer l'état si on était en train de dessiner
    if (m_isDrawing) {
        clearPreview();
        m_isDrawing = false;
        emit drawingStateChanged(false);
    }

    setActive(false);
}

bool ShapeCreationTool::handleMousePress(const Point2D& scenePos, Qt::MouseButton button)
{
    if (!m_isActive || button != Qt::LeftButton) {
        return false;
    }

    if (m_creationMode == CreationMode::ClickAndDrag) {
        // Début du dessin
        m_startPoint = scenePos;
        m_currentPoint = scenePos;
        m_isDrawing = true;

        emit drawingStateChanged(true);
        emit statusMessage(QString("Drag to set %1 size").arg(m_shapeType));

        qCDebug(logCore()) << "ShapeCreationTool: Start drawing at" << scenePos.x << "," << scenePos.y;
        return true;
    }
    else if (m_creationMode == CreationMode::TwoClicks) {
        if (!m_isDrawing) {
            // Premier clic
            m_startPoint = scenePos;
            m_currentPoint = scenePos;
            m_isDrawing = true;

            emit drawingStateChanged(true);
            emit statusMessage("Click again to set opposite corner");

            qCDebug(logCore()) << "ShapeCreationTool: First click at" << scenePos.x << "," << scenePos.y;
        } else {
            // Deuxième clic : créer la forme
            m_currentPoint = scenePos;
            IShape* shape = createFinalShape();

            if (shape) {
                // Ajouter la forme à EditorService
                if (auto* editorService = qobject_cast<Services::EditorService*>(parent())) {
                    editorService->addShape(shape, true);  // true = créer commande Undo/Redo
                }

                emit shapeCreated(shape);
                qCInfo(logShapes()) << "Shape created:" << shape->getTypeName();
            }

            clearPreview();
            m_isDrawing = false;
            emit drawingStateChanged(false);
            emit statusMessage(QString("%1 created").arg(m_shapeType));

            qCDebug(logCore()) << "ShapeCreationTool: Second click, shape created";
        }
        return true;
    }
    else if (m_creationMode == CreationMode::Parametric) {
        // Mode paramétrique : utiliser les dimensions par défaut
        QVariantMap params = m_defaultParams;
        params["type"] = m_shapeType;
        params["x"] = scenePos.x;
        params["y"] = scenePos.y;

        IShape* shape = IShape::create(params);
        if (shape) {
            // Ajouter la forme à EditorService
            if (auto* editorService = qobject_cast<Services::EditorService*>(parent())) {
                editorService->addShape(shape, false);  // false = pas de commande undo/redo pour l'instant
            }

            emit shapeCreated(shape);
            emit statusMessage(QString("%1 created at position").arg(m_shapeType));
            qCInfo(logShapes()) << "Shape created (parametric):" << shape->getTypeName();
        } else {
            qCWarning(logCore()) << "Failed to create shape:" << m_shapeType;
            emit statusMessage(QString("Failed to create %1").arg(m_shapeType));
        }

        return true;
    }

    return false;
}

bool ShapeCreationTool::handleMouseMove(const Point2D& scenePos)
{
    if (!m_isActive || !m_isDrawing) {
        return false;
    }

    m_currentPoint = scenePos;
    updatePreview();

    // Afficher les dimensions dans la barre de statut
    double width = qAbs(m_currentPoint.x - m_startPoint.x);
    double height = qAbs(m_currentPoint.y - m_startPoint.y);
    emit statusMessage(QString("%1: %2 × %3")
                       .arg(m_shapeType)
                       .arg(width, 0, 'f', 2)
                       .arg(height, 0, 'f', 2));

    return true;
}

bool ShapeCreationTool::handleMouseRelease(const Point2D& scenePos, Qt::MouseButton button)
{
    if (!m_isActive || button != Qt::LeftButton) {
        return false;
    }

    if (m_creationMode != CreationMode::ClickAndDrag || !m_isDrawing) {
        return false;
    }

    // Fin du dessin : créer la forme finale
    m_currentPoint = scenePos;

    IShape* shape = createFinalShape();
    if (shape) {
        // Ajouter la forme à EditorService
        if (auto* editorService = qobject_cast<Services::EditorService*>(parent())) {
            editorService->addShape(shape, false);  // false = pas de commande undo/redo pour l'instant
        }

        emit shapeCreated(shape);
        emit statusMessage(QString("%1 created").arg(m_shapeType));
        qCInfo(logShapes()) << "Shape created:" << shape->getTypeName()
                            << "at" << m_startPoint.x << "," << m_startPoint.y;
    } else {
        qCWarning(logCore()) << "Failed to create shape:" << m_shapeType;
        emit statusMessage(QString("Failed to create %1").arg(m_shapeType));
    }

    clearPreview();
    m_isDrawing = false;
    emit drawingStateChanged(false);

    return true;
}

bool ShapeCreationTool::handleKeyPress(int key, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers)

    // Échap : annuler le dessin en cours
    if (key == Qt::Key_Escape && m_isDrawing) {
        clearPreview();
        m_isDrawing = false;
        emit drawingStateChanged(false);
        emit statusMessage("Drawing cancelled");

        qCDebug(logCore()) << "ShapeCreationTool: Drawing cancelled";
        return true;
    }

    return false;
}

// ===== Configuration =====

void ShapeCreationTool::setShapeType(const QString& type)
{
    updateProperty(m_shapeType, type, &ShapeCreationTool::shapeTypeChanged);
}

void ShapeCreationTool::setCreationMode(CreationMode mode)
{
    if (m_creationMode != mode) {
        m_creationMode = mode;
        emit creationModeChanged(mode);
    }
}

void ShapeCreationTool::setDefaultParameters(const QVariantMap& params)
{
    m_defaultParams = params;
}

// ===== Helpers privés =====

QVariantMap ShapeCreationTool::calculateShapeParams(const Point2D& p1, const Point2D& p2) const
{
    QVariantMap params;

    // Calculer le rectangle englobant
    double x = qMin(p1.x, p2.x);
    double y = qMin(p1.y, p2.y);
    double width = qAbs(p2.x - p1.x);
    double height = qAbs(p2.y - p1.y);

    // Paramètres communs à toutes les formes
    params["x"] = x;
    params["y"] = y;
    params["width"] = width;
    params["height"] = height;

    // Paramètres spécifiques selon le type de forme
    if (m_shapeType == "Circle") {
        // Pour un cercle, utiliser le rayon moyen
        double radius = qMin(width, height) / 2.0;
        params["centerX"] = x + width / 2.0;
        params["centerY"] = y + height / 2.0;
        params["radius"] = radius;
    }
    else if (m_shapeType == "Triangle") {
        // Pour un triangle équilatéral, utiliser la dimension moyenne comme taille
        double size = (width + height) / 2.0;
        params["centerX"] = x + width / 2.0;
        params["centerY"] = y + height / 2.0;
        params["size"] = size;
    }

    // Fusionner avec les paramètres par défaut
    for (auto it = m_defaultParams.constBegin(); it != m_defaultParams.constEnd(); ++it) {
        if (!params.contains(it.key())) {
            params[it.key()] = it.value();
        }
    }

    return params;
}

IShape* ShapeCreationTool::createFinalShape()
{
    // Vérifier que les dimensions sont valides (pas de forme de taille nulle)
    double width = qAbs(m_currentPoint.x - m_startPoint.x);
    double height = qAbs(m_currentPoint.y - m_startPoint.y);

    if (width < 1.0 || height < 1.0) {
        qCWarning(logCore()) << "Shape too small to create:" << width << "x" << height;
        return nullptr;
    }

    // Calculer les paramètres
    QVariantMap params = calculateShapeParams(m_startPoint, m_currentPoint);
    params["type"] = m_shapeType;

    // Créer la forme via Factory Pattern
    IShape* shape = IShape::create(params);

    if (!shape) {
        qCWarning(logCore()) << "Factory failed to create shape:" << m_shapeType;
        qCWarning(logCore()) << "Available types:" << IShape::availableTypes();
    }

    return shape;
}

void ShapeCreationTool::updatePreview()
{
    // Nettoyer l'ancienne prévisualisation
    clearPreview();

    // Créer une forme temporaire pour la prévisualisation
    QVariantMap params = calculateShapeParams(m_startPoint, m_currentPoint);
    params["type"] = m_shapeType;
    m_previewShape = IShape::create(params);

    if (m_previewShape) {
        emit shapePreview(m_previewShape);
    }
}

void ShapeCreationTool::clearPreview()
{
    if (m_previewShape) {
        delete m_previewShape;
        m_previewShape = nullptr;
        emit shapePreview(nullptr);
    }
}

} // namespace Editor
} // namespace Core
} // namespace LaserCutStudio
