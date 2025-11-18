#include "IShape.h"

namespace LaserCutStudio {
namespace Core {

// Initialisation des listes statiques
QList<IShape*> IShape::s_shapes;
QMap<QString, IShape::FactoryFunc> IShape::s_factories;

IShape::IShape()
    : Interface()
{
    // Ajoute automatiquement cette forme à la liste
    addShape(this);
}

IShape::~IShape()
{
    // Retire automatiquement cette forme de la liste
    removeShape(this);
}

void IShape::addShape(IShape* shape)
{
    if (shape && !s_shapes.contains(shape)) {
        s_shapes.append(shape);
    }
}

void IShape::removeShape(IShape* shape)
{
    s_shapes.removeAll(shape);
}

void IShape::clearAllShapes()
{
    s_shapes.clear();
}

// ===== Factory Pattern =====

IShape* IShape::create(const QVariantMap& config)
{
    QString type = config.value("type").toString();

    if (!s_factories.contains(type)) {
        qWarning() << "Unknown shape type:" << type;
        return nullptr;
    }

    return s_factories[type](config);
}

QStringList IShape::availableTypes()
{
    return s_factories.keys();
}

QVariantMap IShape::toVariant() const
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
