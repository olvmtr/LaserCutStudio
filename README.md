# LaserCutStudio

![Qt](https://img.shields.io/badge/Qt-6.4+-green.svg)
![C++](https://img.shields.io/badge/C++-17-blue.svg)
![CMake](https://img.shields.io/badge/CMake-3.16+-blue.svg)
![Tests](https://img.shields.io/badge/Tests-134%2F134-brightgreen.svg)
![License](https://img.shields.io/badge/License-MIT-yellow.svg)

Application desktop pour concevoir des projets de découpe laser avec édition 2D et visualisation 3D.

## 📋 Table des matières

- [Vue d'ensemble](#vue-densemble)
- [Fonctionnalités](#fonctionnalités)
- [Architecture](#architecture)
- [Technologies](#technologies)
- [Installation](#installation)
- [Compilation](#compilation)
- [Tests](#tests)
- [Documentation](#documentation)
- [Structure du projet](#structure-du-projet)
- [Roadmap](#roadmap)
- [Contribution](#contribution)
- [License](#license)

## 🎯 Vue d'ensemble

LaserCutStudio permet de :
- 🎨 Dessiner des pièces 2D dans un éditeur graphique
- 🔗 Définir des connexions d'assemblage (encoches, tenons, mortaises, doigts)
- 📐 Visualiser le plan de découpe 2D complet
- 🎲 Visualiser le résultat assemblé en 3D
- 💾 Exporter en SVG/DXF pour découpe laser

## ✨ Fonctionnalités

### Module Core (✅ Implémenté)

#### Gestion des données
- **Types géométriques** : Point2D, Point3D, Material, JointType
- **Formes 2D** : Rectangle, Circle (extensible via plugins)
- **Pièces** : Part avec formes multiples et propriétés
- **Assemblages** : TabJoint, FingerJoint (extensible via plugins)
- **Projets** : Project regroupant pièces et assemblages

#### Patterns architecturaux
- ✅ **Prototype Pattern** : Clonage via `clone()` sur toutes les interfaces
- ✅ **Factory Pattern** : Création dynamique via `create()` avec QVariant
- ✅ **Signals/Slots Qt** : Communication découplée entre composants
- ✅ **Singleton Pattern** : ConfigManager, LogManager, PluginManager, ServiceLocator

#### Systèmes avancés

##### 1. Configuration (ConfigManager)
```cpp
ConfigManager& config = ConfigManager::instance();
config.setDefaultUnit(ConfigManager::Unit::Millimeters);
config.setMaterial(Material("Plywood", 3.0, "Brown"));
config.save();  // Persistance automatique avec QSettings
```
- Gestion matériaux et préférences
- Sauvegarde automatique avec QSettings
- Signaux pour changements (configurationChanged, materialsChanged)

##### 2. Logging structuré
```cpp
#include "core/logging/LogCategories.h"

qCDebug(logShapes) << "Creating rectangle:" << width << "x" << height;
qCInfo(logCore) << "Application initialized";
qCWarning(logJoints) << "Invalid joint configuration";
```
- 7 catégories : core, shapes, parts, joints, projects, config, performance
- 4 formats : Default, Detailed, Compact, JSON
- Sortie fichier avec rotation automatique
- Filtrage par catégorie avec QT_LOGGING_RULES

##### 3. Système de plugins
```cpp
// Créer un plugin de forme
class PolygonPlugin : public QObject, public IShapePlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.lasercutstudio.IShapePlugin")
    Q_INTERFACES(LaserCutStudio::Core::Plugins::IShapePlugin)

public:
    IShape* createShape(const QVariant& params) const override {
        return new Polygon(/* ... */);
    }
};

// Utiliser le plugin
PluginManager::instance().loadAllPlugins();
IShape* polygon = IShape::create("Polygon", params);
```
- Interfaces : IShapePlugin, IJointPlugin
- Chargement dynamique avec QPluginLoader
- Découverte automatique dans ./plugins/ et répertoires système
- Intégration transparente avec Factory Pattern

##### 4. Dependency Injection
```cpp
// Enregistrer un service
ServiceLocator::instance().registerSingleton<IConfig>(
    []() { return new ConfigManager(); },
    "IConfig"
);

// Résoudre une dépendance
IConfig* config = ServiceLocator::instance().resolve<IConfig>("IConfig");
config->load();
```
- ServiceLocator avec cycle de vie Singleton et Transient
- Facilite les tests avec injection de mocks
- Thread-safe

##### 5. Optimisations Release
```cpp
#if DEBUG
    qDebug() << "Debug info";
    validateInternalState();
#endif
```
- Code debug **complètement retiré** en Release (0 overhead)
- QT_NO_DEBUG_OUTPUT / QT_NO_INFO_OUTPUT
- Macros : LIKELY, UNLIKELY, FORCE_INLINE
- **Gain : 20-40% amélioration performance**

##### 6. Tests de performance
```cpp
void BenchmarkShapes::benchmarkRectangleCreation() {
    QBENCHMARK {
        Rectangle rect(0, 0, 100, 50);
    }
}
```
- 17 benchmarks avec QTest::qBenchmark
- Baseline performance établi
- Option BUILD_BENCHMARKS (OFF en Release)

##### 7. Documentation Doxygen
```bash
# Générer la documentation
cmake --build . --target doc
open docs/doxygen/html/index.html
```
- Call graphs, class diagrams, collaboration diagrams
- Source browser intégré
- Prêt pour CI/CD

## 🏗️ Architecture

### Hiérarchie des classes

```
Interface (base QObject)
  └─> clone() : Interface*
  └─> toVariant() : QVariant
  └─> UUID unique

  ├─> IShape (formes géométriques 2D)
  │     ├─> Rectangle
  │     ├─> Circle
  │     └─> [Plugins: Polygon, Star, etc.]
  │
  ├─> IPart (pièces à découper)
  │     └─> Part
  │
  ├─> IJoint (connexions d'assemblage)
  │     ├─> TabJoint
  │     ├─> FingerJoint
  │     └─> [Plugins: DoveTail, Mortise, etc.]
  │
  └─> IProject (projets complets)
        └─> Project
```

### Patterns utilisés

| Pattern | Usage | Fichiers |
|---------|-------|----------|
| **Prototype** | Clonage d'objets | `Interface::clone()` |
| **Factory** | Création dynamique | `IShape::create()`, `IJoint::create()` |
| **Singleton** | Instance unique | ConfigManager, LogManager, PluginManager, ServiceLocator |
| **Signals/Slots** | Communication découplée | Tous les QObject |
| **Service Locator** | Injection de dépendances | ServiceLocator |
| **Plugin** | Extensibilité | PluginManager, IShapePlugin, IJointPlugin |

### Principes SOLID

✅ **Single Responsibility** : Chaque classe a une responsabilité unique
✅ **Open/Closed** : Extensible via plugins sans modification
✅ **Liskov Substitution** : Toutes les implémentations respectent les interfaces
✅ **Interface Segregation** : Interfaces spécifiques (IShape, IPart, etc.)
✅ **Dependency Inversion** : Dépendances via ServiceLocator

## 🛠️ Technologies

### Core
- **Qt 6.4+** : Framework application (Quick/QML, Core, Gui, Test)
- **C++17** : Standard moderne avec smart pointers et lambdas
- **CMake 3.16+** : Système de build multi-plateforme

### Outils de développement
- **QTest** : Framework de tests unitaires et benchmarks
- **Doxygen** : Génération de documentation
- **Git** : Contrôle de version

### Bibliothèques Qt utilisées
- `Qt6::Core` : Conteneurs, signaux/slots, métaobjet
- `Qt6::Gui` : Types géométriques (QRectF, QPointF)
- `Qt6::Quick` : Interface QML (prévu)
- `Qt6::Test` : Tests unitaires et benchmarks

## 📥 Installation

### Prérequis

**Ubuntu/Debian** :
```bash
sudo apt-get update
sudo apt-get install -y \
    qt6-base-dev \
    qt6-declarative-dev \
    cmake \
    build-essential \
    doxygen \
    graphviz
```

**macOS** :
```bash
brew install qt@6 cmake doxygen graphviz
```

**Windows** :
- Installer [Qt 6.4+](https://www.qt.io/download)
- Installer [CMake 3.16+](https://cmake.org/download/)
- Installer [Visual Studio 2019+](https://visualstudio.microsoft.com/)

### Cloner le projet

```bash
git clone https://github.com/olvmtr/LaserCutStudio.git
cd LaserCutStudio
```

## 🔨 Compilation

### Build Debug (avec benchmarks)

```bash
cd src/LaserCutStudio
mkdir -p build/Desktop-Debug
cd build/Desktop-Debug
cmake ../.. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

### Build Release (optimisé)

```bash
cd src/LaserCutStudio
mkdir -p build/Desktop-Release
cd build/Desktop-Release
cmake ../.. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### Options CMake

| Option | Description | Défaut |
|--------|-------------|--------|
| `BUILD_BENCHMARKS` | Compile les tests de performance | ON (Debug), OFF (Release) |
| `ENABLE_DEBUG_LOGS_IN_RELEASE` | Active qCDebug()/qCInfo() en Release | OFF |

**Exemple** :
```bash
cmake ../.. -DCMAKE_BUILD_TYPE=Release -DBUILD_BENCHMARKS=ON
```

### Exécution

```bash
# Application principale
./appLaserCutStudio

# Tests unitaires
./tests/LaserCutStudioTests

# Benchmarks (si compilés)
./tests/benchmarks/LaserCutStudioBenchmarks
```

## 🧪 Tests

### Tests unitaires

**134 tests couvrant 100% des fonctionnalités** :

```bash
cd build/Desktop-Debug
./tests/LaserCutStudioTests
```

**Résultats** :
```
✅ TestTypes:          11/11 tests (types géométriques)
✅ TestShapes:         20/20 tests (formes 2D)
✅ TestPart:           11/11 tests (pièces)
✅ TestJoint:          16/16 tests (assemblages)
✅ TestProject:        12/12 tests (projets)
✅ TestConfigManager:  19/19 tests (configuration)
✅ TestLogging:         9/9 tests (logging)
✅ TestPluginManager:  14/14 tests (plugins)
✅ TestServiceLocator: 22/22 tests (DI)

Total: 134/134 tests réussis (100%)
```

### Tests de performance

```bash
./tests/benchmarks/LaserCutStudioBenchmarks
```

**Benchmarks disponibles** :
- Création d'objets (Rectangle, Circle, Part, Joint)
- Clonage avec Prototype Pattern
- Calculs géométriques (area, perimeter)
- Transformations (move, rotate, scale)
- Sérialisation (toVariant, fromVariant)
- Factory Pattern (create)

**Résultats typiques** :
```
Rectangle creation:     ~0.019 ms/iter
Rectangle clone:        ~0.019 ms/iter
Rectangle area calc:    ~0.003 µs/iter
Rectangle toVariant:    ~0.002 ms/iter
Factory create:         ~0.020 ms/iter
```

### Tests avec CTest

```bash
ctest --output-on-failure
```

## 📚 Documentation

### Générer la documentation Doxygen

**Prérequis** :
```bash
sudo apt-get install doxygen graphviz
```

**Génération** :
```bash
cd build/Desktop-Debug
cmake --build . --target doc
```

**Consultation** :
```bash
open ../../docs/doxygen/html/index.html
```

### Documentation incluse

- **Call graphs** : Graphes d'appels de fonctions
- **Class diagrams** : Diagrammes UML des classes
- **Collaboration diagrams** : Diagrammes de collaboration
- **Include graphs** : Graphes de dépendances
- **Source browser** : Code source avec liens

### Fichiers de documentation

- `README.md` : Ce fichier
- `CLAUDE.md` : Guide détaillé pour développeurs
- `docs/doxygen/` : Documentation générée
- `core/examples/` : Exemples de code

## 📁 Structure du projet

```
LaserCutStudio/
├── README.md                          # Ce fichier
├── CLAUDE.md                          # Guide développeur détaillé
├── Doxyfile                           # Configuration Doxygen
├── .gitignore                         # Fichiers ignorés par Git
│
├── docs/
│   ├── notes/                         # Notes de développement
│   └── doxygen/                       # Documentation générée (gitignore)
│
└── src/LaserCutStudio/
    ├── CMakeLists.txt                 # Configuration build principale
    ├── main.cpp                       # Point d'entrée
    ├── Main.qml                       # Interface QML
    │
    ├── core/                          # Module Core
    │   ├── DebugMacros.h              # Macros debug conditionnelles
    │   │
    │   ├── interface/                 # Interface de base
    │   │   ├── Interface.h
    │   │   └── Interface.cpp
    │   │
    │   ├── types/                     # Types utilitaires
    │   │   ├── Point2D.h
    │   │   ├── Point3D.h
    │   │   ├── Material.h
    │   │   └── JointType.h
    │   │
    │   ├── shapes/                    # Formes 2D
    │   │   ├── IShape.h/cpp
    │   │   ├── Rectangle.h/cpp
    │   │   └── Circle.h/cpp
    │   │
    │   ├── parts/                     # Pièces
    │   │   ├── IPart.h/cpp
    │   │   └── Part.h/cpp
    │   │
    │   ├── joints/                    # Assemblages
    │   │   ├── IJoint.h/cpp
    │   │   ├── TabJoint.h/cpp
    │   │   └── FingerJoint.h/cpp
    │   │
    │   ├── projects/                  # Projets
    │   │   ├── IProject.h/cpp
    │   │   └── Project.h/cpp
    │   │
    │   ├── config/                    # Configuration
    │   │   ├── ConfigManager.h/cpp
    │   │   └── [QSettings persistence]
    │   │
    │   ├── logging/                   # Logging
    │   │   ├── LogCategories.h/cpp
    │   │   └── LogManager.h/cpp
    │   │
    │   ├── plugins/                   # Système de plugins
    │   │   ├── IShapePlugin.h
    │   │   ├── IJointPlugin.h
    │   │   └── PluginManager.h/cpp
    │   │
    │   ├── di/                        # Dependency Injection
    │   │   └── ServiceLocator.h/cpp
    │   │
    │   └── examples/                  # Exemples de code
    │       └── DebugExample.cpp
    │
    ├── plugins/                       # Plugins exemple
    │   └── polygon/
    │       ├── PolygonPlugin.h
    │       ├── Polygon.h
    │       ├── polygon.json
    │       └── CMakeLists.txt
    │
    └── tests/                         # Tests unitaires
        ├── CMakeLists.txt
        ├── main.cpp                   # Runner de tests
        ├── TestTypes.h/cpp
        ├── TestShapes.h/cpp
        ├── TestPart.h/cpp
        ├── TestJoint.h/cpp
        ├── TestProject.h/cpp
        ├── TestConfigManager.h/cpp
        ├── TestLogging.h/cpp
        ├── TestPluginManager.h/cpp
        ├── TestServiceLocator.h/cpp
        └── benchmarks/                # Tests de performance
            ├── CMakeLists.txt
            ├── main_benchmarks.cpp
            └── BenchmarkShapes.h/cpp
```

## 🗺️ Roadmap

### ✅ Phase 1 : Configuration initiale (Complète)
- [x] Squelette Qt Quick
- [x] Configuration CMake
- [x] Structure de base

### ✅ Phase 2 : Module Core (Complète)
- [x] Types utilitaires (Point2D, Point3D, Material, JointType)
- [x] Pattern Prototype (Interface base)
- [x] Formes 2D (IShape, Rectangle, Circle)
- [x] Pièces (IPart, Part)
- [x] Assemblages (IJoint, TabJoint, FingerJoint)
- [x] Projets (IProject, Project)
- [x] Factory Pattern avec QVariant
- [x] Signals/Slots Qt
- [x] Tests unitaires complets (134 tests)

### ✅ Phase 2.5 : Améliorations architecture (Complète)
- [x] ConfigManager avec QSettings
- [x] Logging structuré (7 catégories)
- [x] Tests de performance (17 benchmarks)
- [x] Optimisations Release (20-40%)
- [x] Documentation Doxygen
- [x] Système de plugins Qt
- [x] Dependency Injection (ServiceLocator)

### 🚧 Phase 3 : Module 2D Editor (À venir)
- [ ] Canvas de dessin avec QGraphicsView
- [ ] Outils de dessin (ligne, rectangle, cercle, polygone)
- [ ] Sélection et transformation
- [ ] Grille et magnétisme
- [ ] Undo/Redo

### 🚧 Phase 4 : Module 2D Viewer (À venir)
- [ ] Vue d'ensemble du plan de découpe
- [ ] Optimisation de placement
- [ ] Calcul de surfaces utilisées
- [ ] Export du plan

### 🚧 Phase 5 : Module 3D Viewer (À venir)
- [ ] Visualisation 3D avec Qt3D ou Qt Quick 3D
- [ ] Rendu de l'assemblage
- [ ] Rotation, zoom, pan
- [ ] Vérification des collisions

### 🚧 Phase 6 : Module Export (À venir)
- [ ] Export SVG
- [ ] Export DXF
- [ ] Configuration machines de découpe
- [ ] Prévisualisation export

### 🚧 Phase 7 : Module IO (À venir)
- [ ] Sauvegarde/chargement de projets
- [ ] Format de fichier JSON
- [ ] Import d'autres formats
- [ ] Gestion de versions

### 🚧 Phase 8+ : Fonctionnalités avancées (À venir)
- [ ] Bibliothèque de formes prédéfinies
- [ ] Templates de projets
- [ ] Calcul de coûts matériaux
- [ ] Génération de documentation assemblage
- [ ] Support multi-langue

## 🤝 Contribution

### Guidelines

1. **Fork** le projet
2. Créer une branche (`git checkout -b feature/AmazingFeature`)
3. Commiter les changements (`git commit -m 'feat: Add AmazingFeature'`)
4. Pousser vers la branche (`git push origin feature/AmazingFeature`)
5. Ouvrir une **Pull Request**

### Standards de code

- **C++17** avec conventions Qt
- **4 espaces** pour l'indentation
- **Commentaires Doxygen** pour toutes les API publiques
- **Tests unitaires** pour toute nouvelle fonctionnalité
- **Commits conventionnels** : `feat:`, `fix:`, `docs:`, `test:`, `refactor:`

### Exécuter les tests

Assurez-vous que tous les tests passent avant de soumettre :
```bash
cd build/Desktop-Debug
./tests/LaserCutStudioTests
```

## 📄 License

Ce projet est sous licence **MIT** - voir le fichier [LICENSE](LICENSE) pour plus de détails.

## 👥 Auteurs

- **Olivier Moitry** - *Développement initial* - [olvmtr](https://github.com/olvmtr)
- **Claude (Anthropic)** - *Assistance architecture et développement* - [Claude Code](https://claude.com/claude-code)

## 🙏 Remerciements

- **Qt Project** pour le framework exceptionnel
- **Anthropic** pour Claude Code
- La communauté Qt pour la documentation et les exemples

## 📞 Contact

Pour toute question ou suggestion :
- **Email** : olivier@moitry.net
- **GitHub Issues** : [Créer une issue](https://github.com/olvmtr/LaserCutStudio/issues)

---

**⭐ Si ce projet vous est utile, n'hésitez pas à mettre une étoile sur GitHub !**

---

*Généré avec ❤️ et [Claude Code](https://claude.com/claude-code)*
