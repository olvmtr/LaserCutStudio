#!/bin/bash
# Script pour corriger les contraintes

cd /home/vm-mint/Projet/LaserCutStudio/src/LaserCutStudio/core/models/constraints

# DistanceConstraint.h
sed -i 's|#include "core/models/geometry/GeometricPoint.h"|#include "core/models/geometry/IGeometricElement.h"|g' DistanceConstraint.h
sed -i 's/GeometricPoint\*/IGeometricElement*/g' DistanceConstraint.h
sed -i '10a\\n// Forward declarations\nclass GeometricPoint;\nclass GeometricSegment;' DistanceConstraint.h

# DistanceConstraint.cpp
sed -i '1a\#include "core/models/geometry/implementation/GeometricPoint.h"' DistanceConstraint.cpp
sed -i 's/GeometricPoint\* point1/IGeometricElement* point1/g; s/GeometricPoint\* point2/IGeometricElement* point2/g; s/GeometricPoint\* point/IGeometricElement* point/g' DistanceConstraint.cpp

# FixedPointConstraint.h
sed -i 's|#include "core/models/geometry/GeometricPoint.h"|#include "core/models/geometry/IGeometricElement.h"|g' FixedPointConstraint.h
sed -i 's/GeometricPoint\*/IGeometricElement*/g' FixedPointConstraint.h
sed -i '10a\\n// Forward declarations\nclass GeometricPoint;\nclass GeometricSegment;' FixedPointConstraint.h

# FixedPointConstraint.cpp
sed -i '1a\#include "core/models/geometry/implementation/GeometricPoint.h"' FixedPointConstraint.cpp
sed -i 's/GeometricPoint\* point/IGeometricElement* point/g' FixedPointConstraint.cpp

echo "Contraintes modifiées avec succès"
