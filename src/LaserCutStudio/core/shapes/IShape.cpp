#include "IShape.h"

namespace LaserCutStudio {
namespace Core {

// Initialisation de la liste statique
QList<IShape*> IShape::s_shapes;
// Note: s_factories est maintenant dans FactoryMixin et initialisé automatiquement

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
// Note: create(), availableTypes() et toVariant() sont maintenant fournis par FactoryMixin et Interface

} // namespace Core
} // namespace LaserCutStudio
