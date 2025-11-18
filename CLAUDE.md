# CLAUDE.md

Ce fichier fournit des conseils à Claude Code (claude.ai/code) lors du travail sur le code de ce dépôt.

## Vue d'ensemble du projet

LaserCutStudio est une application desktop pour concevoir des projets de découpe laser avec édition 2D et visualisation 3D. L'application permet aux utilisateurs de :
- Dessiner des pièces 2D dans un éditeur
- Définir des connexions d'assemblage (encoches, tenons, mortaises)
- Visualiser le plan de découpe 2D complet
- Visualiser le résultat assemblé en 3D
- Exporter en SVG/DXF pour la découpe laser

## Stack technologique

- **Qt 6.4+** avec Qt Quick/QML pour l'interface utilisateur
- **C++17** standard
- **CMake 3.16+** pour la configuration de build
- Qt Quick pour une interface déclarative moderne
- Qt3D ou Qt Quick 3D (prévu) pour la visualisation 3D
- QPainter/QGraphicsView (prévu) pour l'édition 2D

## Commandes de build

Le projet utilise CMake et se compile avec Qt Creator ou en ligne de commande :

```bash
# Build Debug (avec benchmarks)
mkdir -p build/Desktop-Debug
cd build/Desktop-Debug
cmake ../.. -DCMAKE_BUILD_TYPE=Debug
cmake --build .

# Build Release (sans benchmarks par défaut)
mkdir -p build/Desktop-Release
cd build/Desktop-Release
cmake ../.. -DCMAKE_BUILD_TYPE=Release
cmake --build .

# Forcer les benchmarks en Release (optionnel)
cmake ../.. -DCMAKE_BUILD_TYPE=Release -DBUILD_BENCHMARKS=ON
cmake --build .

# Exécuter l'application
./appLaserCutStudio

# Exécuter les tests
./tests/LaserCutStudioTests

# Exécuter les benchmarks (si compilés)
./tests/benchmarks/LaserCutStudioBenchmarks
```

L'exécutable se nomme `appLaserCutStudio`.

**Options CMake :**
- `BUILD_BENCHMARKS` : Compile les tests de performance (ON en Debug, OFF en Release par défaut)
- `ENABLE_DEBUG_LOGS_IN_RELEASE` : Active qCDebug()/qCInfo() en Release (OFF par défaut)

**Niveaux de logging par build type :**

| Build Type | qDebug/qCDebug | qInfo/qCInfo | qWarning/qCWarning | qCritical |
|------------|----------------|--------------|-------------------|-----------|
| **Debug**  | ✅ Activé      | ✅ Activé    | ✅ Activé         | ✅ Activé |
| **Release**| ❌ Compilé out | ❌ Compilé out| ✅ Activé        | ✅ Activé |

Les logs de debug/info sont **complètement retirés du binaire** en Release grâce aux macros `QT_NO_DEBUG_OUTPUT` et `QT_NO_INFO_OUTPUT` (0 impact performance).

## Code debug conditionnel avec #if DEBUG

Le fichier `core/DebugMacros.h` garantit que `DEBUG` est défini en mode Debug, permettant d'utiliser `#if DEBUG` partout dans le code.

### Utilisation standard

```cpp
#include "core/DebugMacros.h"

double calculateArea(double width, double height)
{
    #if DEBUG
        // Validations en Debug uniquement
        if (width <= 0 || height <= 0) {
            qCritical() << "Invalid dimensions:" << width << "x" << height;
            return 0.0;
        }
    #endif

    double area = width * height;

    #if DEBUG
        qDebug() << "Calculated area:" << area;
    #endif

    return area;
}
```

### Classes et méthodes debug

```cpp
class MyClass
{
public:
    void process() {
        doWork();

        #if DEBUG
            validateState();
        #endif
    }

    // Méthodes disponibles uniquement en Debug
    #if DEBUG
        void validateState() {
            qDebug() << "Validating...";
        }
    #endif
};
```

### Macros d'optimisation disponibles

| Macro | Usage | Description |
|-------|-------|-------------|
| `LIKELY(x)` | `if (LIKELY(ptr)) { }` | Indique au compilateur que la condition est probable |
| `UNLIKELY(x)` | `if (UNLIKELY(err)) { }` | Indique que la condition est rare |
| `FORCE_INLINE` | `FORCE_INLINE void f() { }` | Force l'inlining d'une fonction |
| `UNUSED(x)` | `UNUSED(param);` | Évite warning variable inutilisée |

### Impact performance

**Code Debug avec #if DEBUG** :
- Code entre `#if DEBUG/#endif` : **COMPLÈTEMENT RETIRÉ** en Release
- 0 overhead en Release (pas dans le binaire)
- Gain : **20-40% amélioration performance** vs code sans conditions

Voir `core/examples/DebugExample.cpp` pour des exemples complets.

## Documentation avec Doxygen

Le projet utilise **Doxygen** pour générer automatiquement la documentation API à partir des commentaires dans le code source.

### Installation

```bash
# Ubuntu/Debian
sudo apt-get install doxygen graphviz

# macOS
brew install doxygen graphviz

# Vérifier l'installation
doxygen --version
```

**Note** : `graphviz` est nécessaire pour générer les diagrammes de classes, call graphs, etc.

### Générer la documentation

```bash
# Option 1 : Via CMake (recommandé)
cd build/Desktop-Debug
cmake --build . --target doc

# Option 2 : Directement avec Doxygen
cd /home/vm-mint/Projet/LaserCutStudio
doxygen Doxyfile
```

La documentation sera générée dans `docs/doxygen/html/`. Ouvrir `docs/doxygen/html/index.html` dans un navigateur.

### Configuration (Doxyfile)

Le fichier `Doxyfile` à la racine du projet configure Doxygen :

**Répertoires scannés** :
- `src/LaserCutStudio/core/` (récursif) : tout le module Core
- `src/LaserCutStudio/main.cpp` : point d'entrée

**Fichiers exclus** :
- `build/` et `build-*/` : fichiers générés
- `core/examples/` : fichiers d'exemples
- `moc_*`, `qrc_*` : fichiers Qt générés

**Fonctionnalités activées** :
- **HTML output** : documentation navigable avec recherche
- **Source browser** : code source inclus avec liens
- **Call graphs** : graphes d'appels de fonctions
- **Class diagrams** : diagrammes UML des classes
- **Collaboration diagrams** : diagrammes de collaboration
- **Include graphs** : graphes de dépendances d'includes

**Macros Qt prédéfinies** :
```cpp
Q_OBJECT, Q_GADGET, Q_DECLARE_LOGGING_CATEGORY, DEBUG=1
```

### Écrire des commentaires Doxygen

**Exemple standard** :
```cpp
/**
 * @brief Calcule l'aire d'un rectangle
 *
 * Cette fonction calcule l'aire en multipliant largeur et hauteur.
 * En mode Debug, elle valide les dimensions.
 *
 * @param width Largeur du rectangle (doit être > 0)
 * @param height Hauteur du rectangle (doit être > 0)
 * @return L'aire calculée, ou 0.0 si dimensions invalides
 *
 * @warning En mode Release, les validations sont retirées
 * @see Rectangle::getArea()
 */
double calculateArea(double width, double height);
```

**Exemple avec classe Qt** :
```cpp
/**
 * @class Rectangle
 * @brief Forme rectangulaire avec support de transformation
 *
 * Rectangle hérite de IShape et fournit des calculs géométriques
 * optimisés pour les rectangles.
 *
 * @note Utilise le Factory Pattern pour la création via QVariant
 */
class Rectangle : public IShape
{
    Q_OBJECT
public:
    /**
     * @brief Constructeur avec dimensions
     * @param x Position X du coin supérieur gauche
     * @param y Position Y du coin supérieur gauche
     * @param width Largeur (doit être > 0)
     * @param height Hauteur (doit être > 0)
     */
    Rectangle(double x, double y, double width, double height);
};
```

**Tags Doxygen utiles** :
- `@brief` : Description courte (1 ligne)
- `@param` : Documentation d'un paramètre
- `@return` : Description de la valeur de retour
- `@see` : Référence croisée vers autre élément
- `@note` : Note importante
- `@warning` : Avertissement
- `@deprecated` : Marque comme obsolète
- `@since` : Version d'introduction
- `@todo` : Tâche à faire

### Intégration CI/CD

Ajouter à un pipeline CI/CD :
```yaml
# Exemple GitHub Actions
- name: Generate documentation
  run: |
    sudo apt-get install -y doxygen graphviz
    cd build/Desktop-Debug
    cmake --build . --target doc

- name: Publish documentation
  uses: peaceiris/actions-gh-pages@v3
  with:
    github_token: ${{ secrets.GITHUB_TOKEN }}
    publish_dir: ./docs/doxygen/html
```

## Architecture

Le projet suit les principes SOLID avec une architecture modulaire :

### Structure modulaire prévue

- **Module Core** : Modèle de données (pièces, assemblages, projets)
  - Utilise le pattern Prototype avec une classe de base `Interface`
  - Chaque type d'interface (IShape, IPart, IJoint, IProject) possède une méthode `clone()`
  - Chaque interface gère sa propre liste d'instances

- **Module 2D Editor** : Interface de dessin et édition

- **Module 2D Viewer** : Vue d'ensemble du plan de découpe

- **Module 3D Viewer** : Visualisation de l'assemblage 3D

- **Module Export** : Génération SVG/DXF

- **Module IO** : Fonctionnalité de sauvegarde/chargement de projet

### Architecture du pattern Prototype

L'architecture centrale utilise une hiérarchie de pattern Prototype :

```
Interface (base)
  └─> clone() : Interface*

  ├─> IShape (formes géométriques)
  │     └─> clone() : IShape*
  │     └─> Gère la liste des formes
  │
  ├─> IPart (pièces à découper)
  │     └─> clone() : IPart*
  │     └─> Gère la liste des pièces
  │
  ├─> IJoint (connexions d'assemblage)
  │     └─> clone() : IJoint*
  │     └─> Gère la liste des assemblages
  │
  └─> IProject (projets complets)
        └─> clone() : IProject*
        └─> Gère la liste des projets
```

Chaque interface gère sa propre liste pour permettre la duplication et la manipulation facile d'objets similaires.

## Structure du projet

```
LaserCutStudio/
├── docs/notes/           # Documentation de planification et architecture
├── src/LaserCutStudio/   # Code source principal de l'application
│   ├── CMakeLists.txt    # Configuration de build
│   ├── main.cpp          # Point d'entrée de l'application
│   └── Main.qml          # Fenêtre principale de l'interface
```

## État du développement

Le projet est en **Phase 2 largement complète**. État actuel :
- **Module Core implémenté** avec toutes les interfaces (IShape, IPart, IJoint, IProject)
- **Pattern Prototype** : classe `Interface` avec `clone()` + UUID unique
- **Factory Pattern** : `registerFactory<T>()` avec QVariant et QMetaObject
- **Signals/Slots Qt** : découplage complet (remplace relations bidirectionnelles)
- **Implémentations concrètes** : Rectangle, Circle, Part, TabJoint, FingerJoint, Project
- **Tests complets** : 1412 lignes de tests Qt couvrant tous les composants
- **Types utilitaires** : Point2D, Point3D, Material, JointType

**Phases suivantes à implémenter** :
- Phase 3 : Module 2D Editor (édition graphique)
- Phase 4 : Module 2D Viewer (plan de découpe)
- Phase 5 : Module 3D Viewer (visualisation assemblage)
- Phase 6 : Module Export (SVG/DXF)
- Phase 7+ : IO, Undo/Redo, optimisation

Consulter `docs/notes/development-plan.md` pour la feuille de route complète du développement par phases.

## Projet Python intégré

Le répertoire `ai-front-portal-main/` contient un projet Python distinct :
- **Framework UltraNova** : Architecture SOLID inspirée de Qt (Signals/Slots)
- **API FastAPI** : Module ALPIE (agent de qualification de leads)
- **287 tests** avec ~95% de couverture (unitaires, intégration, E2E, sécurité, benchmarks)
- **Knowledge Base réutilisable** : base de connaissance structurée

Ce projet Python sert de référence pour les patterns architecturaux et la qualité des tests.

## Améliorations implémentées

### ✅ Améliorations récemment implémentées

1. **Configuration externe** ✅ **Implémenté**
   - Système `ConfigManager` avec QSettings
   - Gestion matériaux, unités, préférences, chemins récents
   - 18 tests unitaires

2. **Logging structuré** ✅ **Implémenté**
   - 7 catégories Qt : `core`, `shapes`, `parts`, `joints`, `projects`, `config`, `performance`
   - `LogManager` avec 4 formats : Default, Detailed, Compact, JSON
   - Sortie fichier avec rotation automatique
   - 9 tests unitaires

3. **Tests de performance** ✅ **Implémenté**
   - Suite `tests/benchmarks/` avec 17 benchmarks
   - Opérations testées : création, clonage, géométrie, transformations, sérialisation
   - Baseline établi pour surveillance performance
   - Option CMake `BUILD_BENCHMARKS` (OFF en Release)

4. **Optimisations Release** ✅ **Implémenté**
   - `#if DEBUG` pour code conditionnel (retrait complet en Release)
   - `QT_NO_DEBUG_OUTPUT` / `QT_NO_INFO_OUTPUT` (0 overhead logs)
   - Gain performance : **20-40%** en Release
   - Macros optimisation : `LIKELY`, `UNLIKELY`, `FORCE_INLINE`

5. **Documentation Doxygen** ✅ **Configuré**
   - `Doxyfile` complet avec call graphs, class diagrams
   - Target CMake `doc` pour génération automatique
   - Documentation complète des tags et usage
   - Prêt pour CI/CD

### ⏳ Améliorations en attente

6. **Système de plugins**
   - Implémenter `QPluginLoader` pour formes/joints extensibles
   - Interface plugin : `IShapePlugin`, `IJointPlugin`
   - Permettre ajout de nouvelles formes sans recompilation

7. **Dependency Injection**
   - Créer Service Locator pattern pour découpler dépendances
   - Faciliter tests unitaires avec mocks
   - Remplacer construction directe par injection

**Note** : Ces améliorations ne bloquent pas le développement des phases suivantes (UI, 3D, Export). Les améliorations 1-5 sont complètes et opérationnelles.
