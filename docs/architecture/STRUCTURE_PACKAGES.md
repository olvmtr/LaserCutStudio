# Structure des Packages - LaserCutStudio

Ce document décrit l'organisation complète des packages et sous-packages du projet LaserCutStudio.

## 📦 Vue d'ensemble

Le projet suit une **architecture en couches** avec séparation claire des responsabilités :

```
┌─────────────────────────────────────────────────┐
│              infrastructure/                     │  Niveau 3
│  (config, logging, DI, plugins)                 │  (Peut dépendre de tout)
└─────────────────────────────────────────────────┘
                      ▲
                      │
┌─────────────────────────────────────────────────┐
│                services/                         │  Niveau 2
│  (editor, geometry, serialization)              │  (Peut dépendre de models)
└─────────────────────────────────────────────────┘
                      ▲
                      │
┌─────────────────────────────────────────────────┐
│                  models/                         │  Niveau 1
│  (shapes, parts, joints, projects, editor)      │  (Base, pas de dépendances)
└─────────────────────────────────────────────────┘
                      ▲
                      │
┌─────────────────────────────────────────────────┐
│                 patterns/                        │  Niveau 0
│  (Mixins CRTP totalement indépendants)          │  (Zéro dépendance)
└─────────────────────────────────────────────────┘
```

---

## 🏗️ Structure détaillée

### 📂 `core/models/` - Modèle de données (couche domaine)

Contient les entités métier et leur logique. **Pas de dépendances externes** sauf Qt Core.

#### `models/base/` - Types et classes de base

| Fichier | Description | Rôle |
|---------|-------------|------|
| `Interface.h/cpp` | Classe de base pour tous les objets | Pattern Prototype, UUID unique, signals Qt |
| `types/Point2D.h` | Point 2D (x, y) | Géométrie 2D |
| `types/Point3D.h` | Point 3D (x, y, z) | Géométrie 3D, assemblage |
| `types/Material.h` | Matériau (bois, acrylique...) | Propriétés matériaux |
| `types/JointType.h` | Type de joint (Tab, Finger...) | Enum joints |

#### `models/shapes/` - Formes géométriques 2D

```
shapes/
├── interfaces/
│   └── IShape.h                    # Interface des formes (getArea, getPerimeter, contains)
└── implementations/
    ├── Rectangle.h/cpp             # Rectangle (x, y, width, height)
    ├── Circle.h/cpp                # Cercle (centerX, centerY, radius)
    └── Triangle.h/cpp              # Triangle (3 points)
```

**Patterns utilisés** :
- ✅ Factory Pattern via `FactoryMixin<IShape>`
- ✅ Prototype Pattern via `ClonableMixin`
- ✅ Property Pattern via `PropertyMixin<T>`

#### `models/parts/` - Pièces à découper

```
parts/
├── interfaces/
│   └── IPart.h                     # Interface des pièces (getShape, getThickness)
└── implementations/
    └── Part.h/cpp                  # Pièce concrète (contient UNE IShape)
```

**Note importante** : Chaque `IPart` contient **UNE** `IShape` accessible via `getShape()` (pas `getShapes()` au pluriel).

#### `models/joints/` - Connexions d'assemblage

```
joints/
├── interfaces/
│   └── IJoint.h                    # Interface des joints (connect, disconnect)
└── implementations/
    ├── TabJoint.h/cpp              # Joint à tenon (perpendiculaire)
    └── FingerJoint.h/cpp           # Joint à encoches (parallèle)
```

**Gestion automatique des signaux** : Les joints écoutent `aboutToBeDestroyed` des pièces pour nettoyage automatique.

#### `models/projects/` - Projets complets

```
projects/
├── interfaces/
│   └── IProject.h                  # Interface des projets (getParts, getJoints)
└── implementations/
    └── Project.h/cpp               # Projet concret (collection de parts et joints)
```

#### `models/editor/` - Modèle éditeur 2D (Phase 3)

```
editor/
├── interfaces/
│   ├── ICommand.h                  # Interface des commandes (execute, undo)
│   ├── IEditorState.h              # État de l'éditeur (formes, sélection)
│   └── ITool.h                     # Interface des outils (onMousePress, onMouseMove)
└── implementations/
    ├── CreateShapeCommand.h/cpp    # Commande de création de forme
    ├── DeleteShapeCommand.h/cpp    # Commande de suppression
    ├── EditorState.h/cpp           # État actuel de l'éditeur
    ├── RectangleTool.h/cpp         # Outil Rectangle
    ├── CircleTool.h/cpp            # Outil Cercle
    └── TriangleTool.h/cpp          # Outil Triangle
```

**Pattern Command** : Implémentation complète Undo/Redo avec pile de commandes.

---

### 📂 `core/services/` - Services métier

Contient l'orchestration et les algorithmes métier. **Peut dépendre de `models/`**.

#### `services/editor/` - Services éditeur 2D

| Fichier | Description | Responsabilité |
|---------|-------------|----------------|
| `CommandManager.h/cpp` | Gestionnaire Undo/Redo | Pile de commandes, execute/undo/redo |
| `ToolPalette.h/cpp` | Palette d'outils dynamique | Enregistrement et sélection d'outils |

#### `services/geometry/` - Services géométriques (Phase 5+)

**Prévu pour** :
- Génération de mesh 3D (extrusion)
- Triangulation de polygones
- Transformations 3D (rotation, translation, scale)

#### `services/serialization/` - Sérialisation (Phase 6)

**Prévu pour** :
- Export SVG avec `QSvgGenerator`
- Export DXF avec bibliothèque externe
- Import/Export projets (format propriétaire)

#### `services/validation/` - Validation (Phase 5+)

**Prévu pour** :
- Validation d'assemblages 3D
- Détection de collisions
- Vérification de contraintes géométriques

---

### 📂 `core/infrastructure/` - Couche infrastructure

Contient les services techniques transverses. **Peut dépendre de tout**.

#### `infrastructure/config/` - Configuration

| Fichier | Description |
|---------|-------------|
| `ConfigManager.h/cpp` | Gestionnaire de configuration (QSettings) |
| `ConfigKeys.h` | Clés de configuration (constantes) |

**Fonctionnalités** :
- Gestion des matériaux par défaut
- Unités de mesure (mm, cm, inches)
- Préférences utilisateur
- Chemins de fichiers récents

#### `infrastructure/logging/` - Logging structuré

| Fichier | Description |
|---------|-------------|
| `LogManager.h/cpp` | Gestionnaire de logs (formats, destinations) |
| `LoggingCategories.h` | Catégories Qt (core, shapes, parts, joints...) |

**Formats disponibles** :
- Default : `[TIMESTAMP] [CATEGORY] Message`
- Detailed : + fonction, fichier, ligne
- Compact : Seulement message
- JSON : Format structuré

#### `infrastructure/di/` - Dependency Injection

| Fichier | Description |
|---------|-------------|
| `ServiceLocator.h/cpp` | Service Locator pattern (DI simple) |

**Cycles de vie** :
- **Singleton** : Une instance partagée
- **Transient** : Nouvelle instance à chaque fois
- **Instance** : Instance existante enregistrée

#### `infrastructure/plugins/` - Système de plugins

```
plugins/
├── core/
│   └── PluginManager.h/cpp         # Gestionnaire de plugins (QPluginLoader)
└── interfaces/
    ├── IShapePlugin.h              # Interface pour plugins de formes
    └── IJointPlugin.h              # Interface pour plugins de joints
```

**Fonctionnalités** :
- Découverte automatique de plugins (répertoires configurables)
- Chargement dynamique avec `QPluginLoader`
- Enregistrement automatique dans Factory Pattern
- Signaux pour suivi du chargement

#### `infrastructure/patterns/` - Patterns DI (Phase future)

Répertoire prévu pour des patterns DI plus avancés si nécessaire. Actuellement vide (les mixins CRTP sont dans `/patterns/`).

---

### 📂 `core/patterns/` - Mixins CRTP (indépendants)

Mixins réutilisables basés sur **CRTP** (Curiously Recurring Template Pattern). **Zéro dépendance** (seulement Qt Core et stdlib).

| Fichier | Description | Usage |
|---------|-------------|-------|
| `FactoryMixin.h` | Factory Pattern automatique | `class IShape : protected FactoryMixin<IShape>` |
| `PropertyMixin.h` | Setters avec signaux Qt | `class Rectangle : protected PropertyMixin<Rectangle>` |
| `ClonableMixin.h` | Macro `IMPLEMENT_CLONE` | Prototype Pattern simplifié |
| `ListManagerMixin.h` | Gestion de listes d'objets | `class IShape : protected ListManagerMixin<IShape>` |

**Avantages CRTP** :
- ✅ Zéro duplication de code
- ✅ Type-safe (résolution à la compilation)
- ✅ Header-only (pas de .cpp)
- ✅ Performance maximale (pas de virtual calls)

---

### 📂 `core/ui/` - Interface utilisateur

#### `ui/canvas/` - Canvas éditeur 2D

| Fichier | Description |
|---------|-------------|
| `EditorCanvas.h/cpp` | Canvas principal (`QQuickPaintedItem`) |

**Responsabilités** :
- Rendu des formes 2D avec `QPainter`
- Gestion des événements souris
- Intégration avec le pattern Command (Undo/Redo)
- Communication avec QML via `Q_PROPERTY` et `Q_INVOKABLE`

---

### 📂 `core/utils/` - Utilitaires

| Fichier | Description |
|---------|-------------|
| `GeometryUtils.h` | Calculs géométriques (rotation, scale, transformations 2D) |
| `DebugMacros.h` | Macros de debug (`#if DEBUG`, `LIKELY`, `UNLIKELY`, `FORCE_INLINE`) |
| `examples/DebugExample.cpp` | Exemples d'utilisation des macros |

**GeometryUtils** :
- `RotationMatrix` : Rotation 2D optimisée (calcul cos/sin une fois)
- `rotatePoint()` : Rotation d'un point autour d'un centre
- `scalePoint()` : Mise à l'échelle depuis un centre
- Conversions degrés ↔ radians

---

## 📊 Statistiques globales

| Package | Sous-packages | Fichiers .h | Fichiers .cpp | État |
|---------|---------------|-------------|---------------|------|
| **models/** | 6 | ~20 | ~15 | ✅ Complet (Phase 2) |
| **services/** | 4 | ~5 | ~5 | 🚧 Partiel (Phase 3 en cours) |
| **infrastructure/** | 5 | ~10 | ~8 | ✅ Complet (Phase 2) |
| **patterns/** | - | 4 | 0 | ✅ Complet (Phase 2) |
| **ui/** | 1 | ~2 | ~2 | 🚧 Partiel (Phase 3 en cours) |
| **utils/** | 1 | ~3 | ~1 | ✅ Complet (Phase 2) |

**Total** : ~44 fichiers headers, ~31 fichiers sources

---

## 🎯 Packages par phase du projet

### ✅ **Phase 2 (Complétée)** - Core complet

**Modèle de données** :
- `models/base/` - Interface, Point2D, Point3D, Material, JointType
- `models/shapes/` - IShape, Rectangle, Circle, Triangle
- `models/parts/` - IPart, Part
- `models/joints/` - IJoint, TabJoint, FingerJoint
- `models/projects/` - IProject, Project

**Patterns** :
- `patterns/` - FactoryMixin, PropertyMixin, ClonableMixin, ListManagerMixin

**Infrastructure** :
- `infrastructure/config/` - ConfigManager
- `infrastructure/logging/` - LogManager
- `infrastructure/di/` - ServiceLocator
- `infrastructure/plugins/` - PluginManager

**Tests** :
- 1412 lignes de tests Qt (100% couverture du core)

---

### 🚧 **Phase 3 (En cours)** - Éditeur 2D

**Modèle éditeur** :
- `models/editor/` - ICommand, ITool, IEditorState, EditorState
- `models/editor/` - CreateShapeCommand, DeleteShapeCommand
- `models/editor/` - RectangleTool, CircleTool, TriangleTool

**Services éditeur** :
- `services/editor/` - CommandManager (Undo/Redo)
- `services/editor/` - ToolPalette (palette dynamique)

**Interface utilisateur** :
- `ui/canvas/` - EditorCanvas (QQuickPaintedItem)
- QML pour l'interface déclarative

**État actuel** :
- ✅ Pattern Command implémenté
- ✅ Undo/Redo fonctionnel
- ✅ Canvas avec rendu QPainter
- ✅ 3 outils (Rectangle, Circle, Triangle)
- ✅ Factory Pattern pour outils

---

### 📅 **Phase 5 (Future)** - Visualisation 3D

**Services géométriques** :
- `services/geometry/` - MeshGenerator (extrusion formes 2D → mesh 3D)
- `services/geometry/` - Transform3DUtils (transformations 3D)
- `services/validation/` - CollisionDetector (détection de collisions)

**Interface 3D** :
- `ui/viewer3d/` - Viewer3D (Qt Quick 3D)

**Technologies** :
- Qt Quick 3D (inclus Qt 6, zéro dépendance)
- FCL (Flexible Collision Library) pour collisions précises
- Eigen3 (requis par FCL)

---

### 📅 **Phase 6 (Future)** - Export

**Services sérialisation** :
- `services/serialization/` - SVGExporter (QSvgGenerator)
- `services/serialization/` - DXFExporter (bibliothèque externe)
- `services/serialization/` - ProjectSerializer (format propriétaire)

**Technologies** :
- QSvgGenerator (export SVG, inclus Qt)
- Clipper2 (kerf compensation, offset de polygones)
- dxflib ou libdxfrw (export DXF)

---

## 🏛️ Règles d'architecture SOLID

### ✅ **RÈGLE 1** : Pas de dépendances entre classes concrètes

```cpp
// ❌ INTERDIT
#include "Rectangle.h"  // Dans Circle.cpp

// ✅ AUTORISÉ
#include "IShape.h"     // Utiliser l'interface
```

### ✅ **RÈGLE 2** : Utilisation exclusive des interfaces

```cpp
// ❌ INTERDIT
Rectangle* rect = new Rectangle();

// ✅ AUTORISÉ
IShape* shape = IShape::create("Rectangle", params);
```

### ✅ **RÈGLE 3** : Toutes les implémentations héritent d'une interface

```cpp
// ✅ OBLIGATOIRE
class Rectangle : public IShape {
    DECLARE_TYPE_NAME(Rectangle)
    // ...
};
```

### ✅ **RÈGLE 4** : Utilisation des patterns

**Macros obligatoires** :
- `DECLARE_TYPE_NAME(ClassName)` dans les headers
- `IMPLEMENT_CLONE(ClassName, BaseClass)` dans les .cpp

**Mixins CRTP** :
- `FactoryMixin<Base>` pour Factory Pattern
- `PropertyMixin<Derived>` pour setters avec signaux

### ✅ **RÈGLE 5** : Hiérarchie des packages respectée

```
infrastructure/  → peut dépendre de services/ et models/
       ↓
   services/     → peut dépendre de models/
       ↓
    models/      → pas de dépendances (sauf Qt Core)
       ↓
   patterns/     → zéro dépendance (sauf Qt Core et stdlib)
```

### ✅ **RÈGLE 6** : Pas de dépendances circulaires

Détecté automatiquement par `/test-arch`.

### ✅ **RÈGLE 7** : Mixins totalement indépendants

Les mixins dans `patterns/` ne doivent dépendre de **rien** sauf Qt Core et stdlib.

---

## 💡 Principes de conception

### 🔹 **Séparation des préoccupations**

- **models/** : Logique métier pure (entités, règles métier)
- **services/** : Orchestration (algorithmes, coordination)
- **infrastructure/** : Techniques (config, logs, DI, plugins)
- **patterns/** : Réutilisable (mixins génériques)
- **ui/** : Présentation (QML/Qt Quick)

### 🔹 **Dependency Inversion Principle (DIP)**

Les dépendances pointent vers les **interfaces**, pas les implémentations.

```cpp
// ✅ Bon (dépend de l'interface)
class Part {
private:
    IShape* m_shape;  // Dépendance vers interface
};

// ❌ Mauvais (dépend de l'implémentation)
class Part {
private:
    Rectangle* m_shape;  // Couplage fort !
};
```

### 🔹 **Open/Closed Principle (OCP)**

Ouvert à l'extension (plugins), fermé à la modification.

**Exemple** : Ajouter une nouvelle forme sans modifier `IShape` :
1. Créer `Polygon.h/cpp` héritant de `IShape`
2. Enregistrer via `IShape::registerFactory<Polygon>()`
3. Utiliser via `IShape::create("Polygon", params)`

### 🔹 **Single Responsibility Principle (SRP)**

Chaque classe a **une seule responsabilité** :
- `Rectangle` : Représenter un rectangle (géométrie)
- `CommandManager` : Gérer Undo/Redo (historique)
- `ConfigManager` : Gérer configuration (persistance)

---

## 🧪 Tests d'architecture

Pour vérifier que l'architecture respecte les règles SOLID :

```bash
# Exécuter les tests d'architecture
./DevTools/architecture/test_architecture.sh

# Ou seulement les tests Python (rapide)
python3 DevTools/architecture/test_architecture_solid.py
```

**Commandes Claude disponibles** :
- `/test-arch` - Exécute les tests d'architecture
- `/fix-arch` - Analyse et corrige les violations

**Documentation complète** : `docs/architecture/TESTS_ARCHITECTURE_SOLID.md`

---

## 📚 Références

- **CLAUDE.md** : Vue d'ensemble du projet et patterns utilisés
- **docs/architecture/TESTS_ARCHITECTURE_SOLID.md** : Documentation des tests d'architecture
- **docs/notes/qt-advanced-patterns.md** : Patterns Qt avancés (QVariant, Q_PROPERTY, CRTP)

---

## 🔄 Historique des versions

| Version | Date | Changements |
|---------|------|-------------|
| 1.0 | 2025-11-20 | Documentation initiale complète de la structure des packages |

---

**Cette architecture est prête pour les phases futures tout en restant maintenable, testable et extensible !** 🚀
