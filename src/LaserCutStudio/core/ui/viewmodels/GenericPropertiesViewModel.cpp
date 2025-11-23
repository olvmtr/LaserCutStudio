/**
 * @file GenericPropertiesViewModel.cpp
 * @brief Implémentation du ViewModel générique basé sur introspection Qt
 */

#include "core/ui/viewmodels/GenericPropertiesViewModel.h"
#include "core/models/editor/interfaces/ISelection.h"
#include "core/infrastructure/logging/LogCategories.h"
#include <QDebug>
#include <QMetaObject>
#include <QMetaProperty>

// Include IShape.h pour accès aux formes
#include "core/models/shapes/interfaces/IShape.h"

namespace LaserCutStudio {
namespace Core {
namespace UI {

GenericPropertiesViewModel::GenericPropertiesViewModel(QObject* parent)
    : QObject(parent)
    , m_selection(nullptr)
    , m_hasSelection(false)
{
    qCDebug(logCore()) << "GenericPropertiesViewModel created (introspection-based)";
}

GenericPropertiesViewModel::~GenericPropertiesViewModel()
{
    qCDebug(logCore()) << "GenericPropertiesViewModel destroyed";
}

// ===== Configuration =====

void GenericPropertiesViewModel::setSelection(Editor::ISelection* selection)
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
                this, &GenericPropertiesViewModel::onSelectionChanged);
        connect(m_selection, &Editor::ISelection::shapeAdded,
                this, &GenericPropertiesViewModel::onSelectionChanged);
        connect(m_selection, &Editor::ISelection::shapeRemoved,
                this, &GenericPropertiesViewModel::onSelectionChanged);
        connect(m_selection, &Editor::ISelection::cleared,
                this, &GenericPropertiesViewModel::onSelectionChanged);

        qCDebug(logCore()) << "GenericPropertiesViewModel: Connected to ISelection";
    }

    // Rafraîchir immédiatement
    refreshProperties();
}

// ===== Modification de propriétés =====

bool GenericPropertiesViewModel::setProperty(const QString& name, const QVariant& value)
{
    QObject* target = getSelectedObject();
    if (!target) {
        qCWarning(logCore()) << "GenericPropertiesViewModel: Cannot set property"
                             << name << "- no object selected";
        return false;
    }

    // ✅ INTROSPECTION : Utiliser QMetaObject pour écrire la propriété
    bool success = target->setProperty(name.toUtf8().constData(), value);

    if (success) {
        qCDebug(logCore()) << "GenericPropertiesViewModel: Property" << name
                           << "set to" << value;
        // Rafraîchir pour mettre à jour les bindings QML
        refreshProperties();
    } else {
        qCWarning(logCore()) << "GenericPropertiesViewModel: Failed to set property"
                             << name << "to" << value;
    }

    return success;
}

// ===== Slots privés =====

void GenericPropertiesViewModel::onSelectionChanged()
{
    qCDebug(logCore()) << "GenericPropertiesViewModel: Selection changed, refreshing";
    refreshProperties();
}

// ===== Helpers privés =====

void GenericPropertiesViewModel::refreshProperties()
{
    QObject* target = getSelectedObject();

    if (!target) {
        // Pas de sélection : vider les propriétés
        if (m_hasSelection) {
            m_hasSelection = false;
            emit hasSelectionChanged();
        }

        if (!m_properties.isEmpty()) {
            m_properties.clear();
            emit propertiesChanged();
        }

        qCDebug(logCore()) << "GenericPropertiesViewModel: Properties cleared (no selection)";
        return;
    }

    // ✅ INTROSPECTION : Utiliser QMetaObject pour lire TOUTES les propriétés
    QVariantMap newProps;
    const QMetaObject* meta = target->metaObject();

    for (int i = 0; i < meta->propertyCount(); ++i) {
        QMetaProperty prop = meta->property(i);

        // Ignorer les propriétés systèmes Qt
        QString propName = QString::fromUtf8(prop.name());
        if (propName == "objectName") {
            continue;
        }

        // Lire la valeur via introspection
        if (prop.isReadable()) {
            QVariant value = prop.read(target);
            newProps[propName] = value;
        }
    }

    // Mettre à jour hasSelection
    if (!m_hasSelection) {
        m_hasSelection = true;
        emit hasSelectionChanged();
    }

    // Mettre à jour properties si changées
    if (m_properties != newProps) {
        m_properties = newProps;
        emit propertiesChanged();

        qCDebug(logCore()) << "GenericPropertiesViewModel: Properties refreshed"
                           << "count:" << m_properties.size()
                           << "type:" << m_properties.value("type", "").toString();
    }
}

QObject* GenericPropertiesViewModel::getSelectedObject() const
{
    if (!m_selection || m_selection->getCount() != 1) {
        return nullptr;
    }

    QVector<IShape*> shapes = m_selection->getSelectedShapes();
    if (shapes.isEmpty()) {
        return nullptr;
    }

    // IShape hérite de Interface qui hérite de QObject
    return shapes.first();
}

} // namespace UI
} // namespace Core
} // namespace LaserCutStudio
