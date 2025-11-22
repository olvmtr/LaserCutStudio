/**
 * @file SelectionPropertiesViewModel.cpp
 * @brief Implémentation du ViewModel pour les propriétés de sélection
 */

#include "core/ui/viewmodels/SelectionPropertiesViewModel.h"
#include "core/models/editor/interfaces/ISelection.h"
#include "core/infrastructure/logging/LogCategories.h"
#include <QDebug>
#include <QtMath>
#include <limits>

// Include IShape.h depuis LibInterface
#include "core/models/shapes/interfaces/IShape.h"

namespace LaserCutStudio {
namespace Core {
namespace UI {

SelectionPropertiesViewModel::SelectionPropertiesViewModel(QObject* parent)
    : QObject(parent)
    , m_selection(nullptr)
    , m_x(qQNaN())
    , m_y(qQNaN())
    , m_width(qQNaN())
    , m_height(qQNaN())
    , m_radius(qQNaN())
    , m_hasSelection(false)
    , m_hasWidth(false)
    , m_hasHeight(false)
    , m_hasRadius(false)
{
    qCDebug(logCore()) << "SelectionPropertiesViewModel created";
}

SelectionPropertiesViewModel::~SelectionPropertiesViewModel()
{
    qCDebug(logCore()) << "SelectionPropertiesViewModel destroyed";
}

// ===== Configuration =====

void SelectionPropertiesViewModel::setSelection(Editor::ISelection* selection)
{
    if (m_selection == selection) {
        return;
    }

    // Déconnecter ancienne sélection
    if (m_selection) {
        disconnect(m_selection, nullptr, this, nullptr);
    }

    m_selection = selection;

    // Connecter nouvelle sélection
    if (m_selection) {
        connect(m_selection, &Editor::ISelection::countChanged,
                this, &SelectionPropertiesViewModel::onSelectionChanged);
        connect(m_selection, &Editor::ISelection::shapeAdded,
                this, &SelectionPropertiesViewModel::onSelectionChanged);
        connect(m_selection, &Editor::ISelection::shapeRemoved,
                this, &SelectionPropertiesViewModel::onSelectionChanged);
        connect(m_selection, &Editor::ISelection::cleared,
                this, &SelectionPropertiesViewModel::onSelectionChanged);

        qCDebug(logCore()) << "SelectionPropertiesViewModel: Connected to ISelection";
    }

    // Rafraîchir immédiatement
    refreshProperties();
}

// ===== Setters avec modification de la forme =====

void SelectionPropertiesViewModel::setName(const QString& name)
{
    if (m_name == name) return;

    m_name = name;
    applyPropertyToShape("name", name);
    emit nameChanged();
}

void SelectionPropertiesViewModel::setX(double x)
{
    if (qFuzzyCompare(m_x, x)) return;

    m_x = x;
    applyPropertyToShape("x", x);
    emit xChanged();
}

void SelectionPropertiesViewModel::setY(double y)
{
    if (qFuzzyCompare(m_y, y)) return;

    m_y = y;
    applyPropertyToShape("y", y);
    emit yChanged();
}

void SelectionPropertiesViewModel::setWidth(double width)
{
    if (qFuzzyCompare(m_width, width)) return;

    m_width = width;
    applyPropertyToShape("width", width);
    emit widthChanged();
}

void SelectionPropertiesViewModel::setHeight(double height)
{
    if (qFuzzyCompare(m_height, height)) return;

    m_height = height;
    applyPropertyToShape("height", height);
    emit heightChanged();
}

void SelectionPropertiesViewModel::setRadius(double radius)
{
    if (qFuzzyCompare(m_radius, radius)) return;

    m_radius = radius;
    applyPropertyToShape("radius", radius);
    emit radiusChanged();
}

// ===== Slots privés =====

void SelectionPropertiesViewModel::onSelectionChanged()
{
    qCDebug(logCore()) << "SelectionPropertiesViewModel: Selection changed, refreshing properties";
    refreshProperties();
}

// ===== Helpers privés =====

void SelectionPropertiesViewModel::refreshProperties()
{
    IShape* shape = getSelectedShape();

    if (!shape) {
        clearProperties();
        return;
    }

    // Récupérer toutes les propriétés via QMetaObject
    QVariantMap props = shape->toVariant();

    // Mettre à jour hasSelection
    if (!m_hasSelection) {
        m_hasSelection = true;
        emit hasSelectionChanged();
    }

    // Mettre à jour name
    QString newName = props.value("name", "").toString();
    if (m_name != newName) {
        m_name = newName;
        emit nameChanged();
    }

    // Mettre à jour type
    QString newType = props.value("type", "").toString();
    if (m_type != newType) {
        m_type = newType;
        emit typeChanged();
    }

    // Mettre à jour x
    double newX = props.value("x", qQNaN()).toDouble();
    if (!qFuzzyCompare(m_x, newX)) {
        m_x = newX;
        emit xChanged();
    }

    // Mettre à jour y
    double newY = props.value("y", qQNaN()).toDouble();
    if (!qFuzzyCompare(m_y, newY)) {
        m_y = newY;
        emit yChanged();
    }

    // Mettre à jour width (si existe)
    bool hasWidth = props.contains("width");
    if (m_hasWidth != hasWidth) {
        m_hasWidth = hasWidth;
        emit hasWidthChanged();
    }
    if (hasWidth) {
        double newWidth = props.value("width", qQNaN()).toDouble();
        if (!qFuzzyCompare(m_width, newWidth)) {
            m_width = newWidth;
            emit widthChanged();
        }
    } else if (!qIsNaN(m_width)) {
        m_width = qQNaN();
        emit widthChanged();
    }

    // Mettre à jour height (si existe)
    bool hasHeight = props.contains("height");
    if (m_hasHeight != hasHeight) {
        m_hasHeight = hasHeight;
        emit hasHeightChanged();
    }
    if (hasHeight) {
        double newHeight = props.value("height", qQNaN()).toDouble();
        if (!qFuzzyCompare(m_height, newHeight)) {
            m_height = newHeight;
            emit heightChanged();
        }
    } else if (!qIsNaN(m_height)) {
        m_height = qQNaN();
        emit heightChanged();
    }

    // Mettre à jour radius (si existe)
    bool hasRadius = props.contains("radius");
    if (m_hasRadius != hasRadius) {
        m_hasRadius = hasRadius;
        emit hasRadiusChanged();
    }
    if (hasRadius) {
        double newRadius = props.value("radius", qQNaN()).toDouble();
        if (!qFuzzyCompare(m_radius, newRadius)) {
            m_radius = newRadius;
            emit radiusChanged();
        }
    } else if (!qIsNaN(m_radius)) {
        m_radius = qQNaN();
        emit radiusChanged();
    }

    qCDebug(logCore()) << "SelectionPropertiesViewModel: Properties refreshed"
                       << "type:" << m_type
                       << "name:" << m_name
                       << "x:" << m_x << "y:" << m_y;
}

void SelectionPropertiesViewModel::clearProperties()
{
    bool changed = false;

    if (m_hasSelection) {
        m_hasSelection = false;
        emit hasSelectionChanged();
        changed = true;
    }

    if (!m_name.isEmpty()) {
        m_name.clear();
        emit nameChanged();
        changed = true;
    }

    if (!m_type.isEmpty()) {
        m_type.clear();
        emit typeChanged();
        changed = true;
    }

    if (!qIsNaN(m_x)) {
        m_x = qQNaN();
        emit xChanged();
        changed = true;
    }

    if (!qIsNaN(m_y)) {
        m_y = qQNaN();
        emit yChanged();
        changed = true;
    }

    if (m_hasWidth) {
        m_hasWidth = false;
        emit hasWidthChanged();
        changed = true;
    }

    if (!qIsNaN(m_width)) {
        m_width = qQNaN();
        emit widthChanged();
        changed = true;
    }

    if (m_hasHeight) {
        m_hasHeight = false;
        emit hasHeightChanged();
        changed = true;
    }

    if (!qIsNaN(m_height)) {
        m_height = qQNaN();
        emit heightChanged();
        changed = true;
    }

    if (m_hasRadius) {
        m_hasRadius = false;
        emit hasRadiusChanged();
        changed = true;
    }

    if (!qIsNaN(m_radius)) {
        m_radius = qQNaN();
        emit radiusChanged();
        changed = true;
    }

    if (changed) {
        qCDebug(logCore()) << "SelectionPropertiesViewModel: Properties cleared (no selection)";
    }
}

IShape* SelectionPropertiesViewModel::getSelectedShape() const
{
    if (!m_selection || m_selection->getCount() != 1) {
        return nullptr;
    }

    QVector<IShape*> shapes = m_selection->getSelectedShapes();
    if (shapes.isEmpty()) {
        return nullptr;
    }

    return shapes.first();
}

void SelectionPropertiesViewModel::applyPropertyToShape(const QString& propertyName, const QVariant& value)
{
    IShape* shape = getSelectedShape();
    if (!shape) {
        qCWarning(logCore()) << "SelectionPropertiesViewModel: Cannot apply property"
                             << propertyName << "- no shape selected";
        return;
    }

    // Appliquer via setProperty (QMetaObject)
    if (!shape->setProperty(propertyName.toUtf8().constData(), value)) {
        qCWarning(logCore()) << "SelectionPropertiesViewModel: Failed to set property"
                             << propertyName << "to" << value;
        return;
    }

    qCDebug(logCore()) << "SelectionPropertiesViewModel: Applied property"
                       << propertyName << "=" << value;
}

} // namespace UI
} // namespace Core
} // namespace LaserCutStudio
