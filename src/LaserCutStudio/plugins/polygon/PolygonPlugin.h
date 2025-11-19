#ifndef POLYGONPLUGIN_H
#define POLYGONPLUGIN_H

#include <QObject>
#include "../../core/plugins/IShapePlugin.h"
#include "Polygon.h"

namespace LaserCutStudio {
namespace Plugins {

/**
 * @class PolygonPlugin
 * @brief Plugin exemple pour une forme polygone régulier
 *
 * Ce plugin démontre comment créer un plugin de forme personnalisée.
 * Il ajoute la possibilité de créer des polygones réguliers (triangle,
 * pentagone, hexagone, etc.).
 *
 * @note Ceci est un exemple pédagogique de plugin
 */
class PolygonPlugin : public QObject, public Core::Plugins::IShapePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.lasercutstudio.IShapePlugin" FILE "polygon.json")
    Q_INTERFACES(LaserCutStudio::Core::Plugins::IShapePlugin)

public:
    PolygonPlugin() = default;
    ~PolygonPlugin() override = default;

    // Implémentation de IShapePlugin
    QString shapeName() const override { return "Polygon"; }

    QString shapeDescription() const override {
        return "Polygone régulier à N côtés";
    }

    QString version() const override { return "1.0.0"; }

    QString author() const override { return "LaserCutStudio Team"; }

    Core::IShape* createShape(const QVariant& params) const override
    {
        QVariantMap map = params.toMap();

        // Valider les paramètres
        if (!map.contains("centerX") || !map.contains("centerY") ||
            !map.contains("radius") || !map.contains("sides")) {
            qCWarning(Core::logShapes) << "Missing required parameters for Polygon";
            return nullptr;
        }

        double centerX = map["centerX"].toDouble();
        double centerY = map["centerY"].toDouble();
        double radius = map["radius"].toDouble();
        int sides = map["sides"].toInt();

        // Validation
        if (sides < 3 || sides > 100) {
            qCWarning(Core::logShapes) << "Invalid sides count:" << sides
                                       << "(must be 3-100)";
            return nullptr;
        }

        if (radius <= 0.0) {
            qCWarning(Core::logShapes) << "Invalid radius:" << radius;
            return nullptr;
        }

        return new Polygon(centerX, centerY, radius, sides);
    }

    QVariant defaultParameters() const override
    {
        QVariantMap params;
        params["centerX"] = 0.0;
        params["centerY"] = 0.0;
        params["radius"] = 50.0;
        params["sides"] = 6;  // Hexagone par défaut
        return params;
    }

    QVariant parameterSchema() const override
    {
        QVariantMap schema;

        QVariantMap centerX;
        centerX["type"] = "double";
        centerX["description"] = "Position X du centre";
        schema["centerX"] = centerX;

        QVariantMap centerY;
        centerY["type"] = "double";
        centerY["description"] = "Position Y du centre";
        schema["centerY"] = centerY;

        QVariantMap radius;
        radius["type"] = "double";
        radius["min"] = 0.0;
        radius["description"] = "Rayon du polygone";
        schema["radius"] = radius;

        QVariantMap sides;
        sides["type"] = "int";
        sides["min"] = 3;
        sides["max"] = 100;
        sides["description"] = "Nombre de côtés (3-100)";
        schema["sides"] = sides;

        return schema;
    }
};

} // namespace Plugins
} // namespace LaserCutStudio

#endif // POLYGONPLUGIN_H
