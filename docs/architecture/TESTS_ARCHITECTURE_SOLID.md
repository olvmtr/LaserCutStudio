# Tests d'Architecture SOLID pour LaserCutStudio

Ce document explique le système de tests automatiques de l'architecture SOLID pour LaserCutStudio.

## Vue d'ensemble

Le projet utilise des **tests automatisés** pour garantir que l'architecture respecte les principes SOLID et les règles de conception définies. Ces tests sont inspirés du projet Python `ai-front-portal-main` qui utilise des tests similaires.

## Outils utilisés

### 1. **Script Python d'analyse statique** (Prioritaire)
Fichier : `src/LaserCutStudio/tests/test_architecture_solid.py`

Analyse le code source C++ pour détecter les violations architecturales :
- Parse les fichiers `.h` et `.cpp`
- Analyse les `#include`
- Vérifie l'héritage des classes
- Détecte les dépendances circulaires

### 2. **clang-tidy** (Outil Qt Creator)
Configuration : `.clang-tidy` à la racine du projet

Analyse le code C++ compilé pour détecter :
- Problèmes de style de code
- Violations des règles cppcoreguidelines
- Problèmes de performance
- Bugs potentiels

### 3. **CMake compile_commands.json**
Génération activée dans `CMakeLists.txt` :
```cmake
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

Permet aux outils d'analyse (clang-tidy, Qt Creator) de comprendre la structure du projet.

## Règles SOLID testées

### RÈGLE 1 : Pas de dépendances entre classes concrètes

**Principe** : Les implémentations concrètes (Rectangle, Circle, Part, etc.) ne doivent PAS inclure d'autres implémentations concrètes.

❌ **INTERDIT** :
```cpp
// Rectangle.h
#include "Circle.h"  // VIOLATION : classe concrète → classe concrète
```

✅ **AUTORISÉ** :
```cpp
// Rectangle.h
#include "core/models/shapes/interfaces/IShape.h"  // OK : interface
```

### RÈGLE 2 : Seules les interfaces peuvent être utilisées

**Principe** : Les dépendances entre classes doivent utiliser les interfaces (I*), pas les implémentations.

❌ **INTERDIT** :
```cpp
// Part.h
#include "core/models/shapes/implementations/Rectangle.h"  // VIOLATION
```

✅ **AUTORISÉ** :
```cpp
// Part.h
#include "core/models/shapes/interfaces/IShape.h"  // OK
```

### RÈGLE 3 : Toutes les implémentations héritent d'une interface

**Principe** : Chaque classe concrète doit hériter de son interface.

✅ **REQUIS** :
```cpp
// Rectangle.h
class Rectangle : public IShape  // Héritage obligatoire
{
    Q_OBJECT
    DECLARE_TYPE_NAME(Rectangle)
    // ...
};
```

### RÈGLE 4 : Tous les patterns sont utilisés

**Principe** : Utiliser les patterns définis (Mixins, Macros).

✅ **Headers (.h)** :
```cpp
class Rectangle : public IShape
{
    Q_OBJECT
    DECLARE_TYPE_NAME(Rectangle)  // REQUIS : macro de type
    // ...
};
```

✅ **Implémentations (.cpp)** :
```cpp
IMPLEMENT_CLONE(Rectangle, IShape)  // REQUIS : macro de clonage
```

### RÈGLE 5 : Hiérarchie des packages respectée

**Principe** : Respecter la hiérarchie des packages.

**Hiérarchie autorisée** :
```
models/         (base, pas de dépendances internes)
  ↑
services/       (peut dépendre de models/)
  ↑
infrastructure/ (peut dépendre de models/ et services/)
```

❌ **INTERDIT** :
- `models/` → `services/` ou `infrastructure/`
- `services/` → `infrastructure/` (sauf si nécessaire)

✅ **AUTORISÉ** :
- `services/` → `models/`
- `infrastructure/` → `models/` et `services/`

### RÈGLE 6 : Pas de dépendances circulaires entre packages

**Principe** : Aucun cycle de dépendances entre packages au même niveau hiérarchique.

Le test analyse automatiquement tous les packages et sous-packages du projet :
- Packages principaux : `models`, `services`, `infrastructure`, `ui`, `plugins`, etc.
- Sous-packages : `models/shapes`, `models/geometry`, `models/constraints`, etc.

❌ **INTERDIT** :
```
models/ → services/ → models/           (CYCLE entre packages principaux)
models/shapes → models/parts → models/shapes  (CYCLE entre sous-packages)
```

✅ **AUTORISÉ** :
```
models/ → models/constraints            (Package parent → sous-package OK)
models/constraints → models/geometry    (Sous-packages du même parent OK)
```

**Détection avancée** :
- Algorithme **DFS (Depth-First Search)** pour détecter tous les cycles
- Graphe de dépendances affiché avec détails (19+ packages analysés)
- Ignore les dépendances naturelles parent ↔ sous-package
- Affiche les fichiers impliqués dans chaque cycle détecté

**Sortie exemple** :
```
📊 Graphe de dépendances (19 packages analysés):
   models → models/base, models/constraints, models/geometry
   services → models, models/base
   ui → models, models/constraints, models/geometry
```

### RÈGLE 7 : Mixins totalement indépendants

**Principe** : Les mixins (FactoryMixin, PropertyMixin, ClonableMixin) ne doivent avoir AUCUNE dépendance vers le code du projet.

❌ **INTERDIT dans les mixins** :
```cpp
// FactoryMixin.h
#include "core/models/shapes/interfaces/IShape.h"  // VIOLATION
```

✅ **AUTORISÉ dans les mixins** :
```cpp
// FactoryMixin.h
#include <QObject>
#include <QVariantMap>
#include <QString>
```

### RÈGLE 8 : Les interfaces sont purement abstraites

**Principe** : Les interfaces (IShape, IPart, IJoint, IProject) doivent être purement abstraites sans données membres ni implémentation inline.

❌ **INTERDIT dans les interfaces** :
```cpp
// IShape.h
class IShape : public Interface
{
protected:
    QString m_name;  // VIOLATION : membre de données

public:
    QString getName() const { return m_name; }  // VIOLATION : implémentation inline
};
```

✅ **AUTORISÉ dans les interfaces** :
```cpp
// IShape.h
class IShape : public Interface
{
public:
    virtual QString getName() const = 0;  // OK : méthode virtuelle pure
    virtual ~IShape() = default;          // OK : destructeur virtuel
};
```

**Les données et implémentations doivent être dans les classes concrètes.**

### RÈGLE 9 : Classes concrètes dans implementation/

**Principe** : Toutes les classes concrètes doivent être dans des sous-packages `implementation/`. Les interfaces (I*) restent dans le package parent.

✅ **Organisation correcte** :
```
core/models/shapes/
  ├── IShape.h                           # Interface dans le package parent
  └── implementation/
      ├── Rectangle.h                    # Classe concrète dans implementation/
      └── Circle.h                       # Classe concrète dans implementation/
```

❌ **Organisation incorrecte** :
```
core/models/shapes/
  ├── IShape.h
  ├── Rectangle.h                        # VIOLATION : classe concrète hors de implementation/
  └── Circle.h                           # VIOLATION : classe concrète hors de implementation/
```

**Exceptions** : Les types de base (Point2D, Point3D, Material) et les contraintes géométriques peuvent rester dans le package parent.

### RÈGLE 10 : Aucun import depuis implementation/

**Principe** : Les classes ne doivent JAMAIS importer depuis les packages `implementation/`. Elles doivent dépendre des interfaces (abstractions), pas des implémentations concrètes. C'est le **Dependency Inversion Principle (DIP)**.

❌ **INTERDIT** :
```cpp
// MyClass.h
#include "shapes/implementation/Rectangle.h"      // VIOLATION : import depuis implementation/
#include "geometry/implementation/GeometricPoint.h" // VIOLATION : import depuis implementation/
```

✅ **AUTORISÉ** :
```cpp
// MyClass.h
#include "shapes/IShape.h"                // OK : importer l'interface
#include "geometry/GeometricPoint.h"      // OK : type de base (pas dans implementation/)

// Utilisation
IShape* shape = IShape::create("Rectangle", params);  // Factory Pattern
```

**Exceptions** :
- Un fichier dans `geometry/implementation/` peut inclure d'autres fichiers du MÊME package `geometry/implementation/`
- Les fichiers de test peuvent inclure les implémentations pour les tester

**Bénéfices** :
- ✅ Découplage complet entre modules
- ✅ Facilite le remplacement d'implémentations
- ✅ Permet le chargement dynamique de plugins
- ✅ Respecte le principe SOLID (DIP)

### RÈGLE 11 : Pas de dépendances circulaires entre fichiers

**Principe** : Les fichiers ne doivent pas s'inclure mutuellement, créant des cycles d'includes.

Le test construit un graphe complet des includes et détecte tous les cycles avec un algorithme DFS.

❌ **INTERDIT** :
```cpp
// A.h
#include "B.h"  // A inclut B

// B.h
#include "A.h"  // B inclut A → CYCLE !
```

❌ **Cycle indirect** :
```cpp
// A.h → B.h → C.h → A.h  (CYCLE de 3 fichiers)
```

✅ **SOLUTION** :

**1. Forward declarations** :
```cpp
// A.h
class B;  // Forward declaration au lieu de #include "B.h"

class A {
    B* m_b;  // Pointeur ou référence seulement
};
```

**2. Déplacer les includes dans le .cpp** :
```cpp
// A.h (pas d'include B.h)
class B;
class A {
    B* m_b;
};

// A.cpp (include dans l'implémentation)
#include "B.h"
A::A() : m_b(new B()) { }
```

**3. Extraire une interface commune** :
```cpp
// ICommon.h (nouvelle interface)
class ICommon {
    virtual void doSomething() = 0;
};

// A.h et B.h incluent ICommon.h au lieu de se référencer mutuellement
```

**Détection avancée** :
- Analyse de 32+ fichiers headers du projet
- Construction d'un graphe complet d'includes
- Détection de TOUS les cycles (directs et indirects)
- Normalisation des cycles pour éviter les doublons
- Affichage détaillé du chemin de chaque cycle

**Sortie exemple** :
```
📁 Analyse de 32 fichiers headers...
✓ Aucun cycle détecté

✅ RÈGLE 11: Aucune dépendance circulaire entre fichiers détectée (32 fichiers vérifiés)
```

Si un cycle est trouvé :
```
⚠️  2 cycle(s) détecté(s) !

VIOLATION RÈGLE 11: Dépendance circulaire entre fichiers (cycle #1)
   Cycle: A.h → B.h → C.h → A.h
   Longueur: 3 fichiers

   Fichiers impliqués:
   • models/shapes/A.h
     ↓ inclut
     models/parts/B.h
   • models/parts/B.h
     ↓ inclut
     models/joints/C.h
   • models/joints/C.h
     ↓ inclut
     models/shapes/A.h

   SOLUTION:
   1. Utiliser des forward declarations (class ClassName;)
   2. Déplacer les includes dans le fichier .cpp
   3. Extraire une interface commune
   4. Restructurer pour casser la dépendance circulaire
```

## Utilisation

### Exécuter tous les tests

```bash
# Option 1 : Script bash complet (recommandé)
./scripts/test_architecture.sh

# Option 2 : Seulement les tests Python
python3 src/LaserCutStudio/tests/test_architecture_solid.py

# Option 3 : Seulement clang-tidy
cd src/LaserCutStudio/build/Desktop-Debug
clang-tidy ../core/**/*.cpp -p .
```

### Intégration dans Qt Creator

1. **Activer clang-tidy** :
   - `Tools` → `Options` → `C++` → `Code Model`
   - Activer "Use Clang Code Model"
   - Configurer `.clang-tidy`

2. **Voir les avertissements** :
   - Les violations apparaissent dans la fenêtre "Issues"
   - Survol du code pour voir les détails

3. **Générer compile_commands.json** :
   ```bash
   cd src/LaserCutStudio/build/Desktop-Debug
   cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
   ```

### Intégration CI/CD (futur)

Ajouter au pipeline :
```yaml
# .gitlab-ci.yml ou .github/workflows/architecture.yml
test-architecture:
  script:
    - ./scripts/test_architecture.sh
  rules:
    - when: always  # Toujours exécuter
```

## Sortie des tests

### Succès (exit code 0) :
```
================================================================================
🏗️  Tests d'Architecture SOLID pour LaserCutStudio
================================================================================

🔍 Test 1: Vérification des dépendances entre classes concrètes...
✅ RÈGLE 1: Aucune dépendance entre classes concrètes détectée

🔍 Test 2: Vérification de l'utilisation exclusive des interfaces...
✅ RÈGLE 2: Toutes les dépendances utilisent les interfaces

🔍 Test 3: Vérification de l'héritage des interfaces...
✅ RÈGLE 3: Toutes les implémentations héritent de leur interface

🔍 Test 4: Vérification de l'utilisation des patterns...
✅ RÈGLE 4: Tous les patterns sont correctement utilisés

🔍 Test 5: Vérification de la hiérarchie des packages...
✅ RÈGLE 5: Hiérarchie des packages respectée

🔍 Test 6: Détection des dépendances circulaires entre packages...
✅ RÈGLE 6: Aucune dépendance circulaire détectée

🔍 Test 7: Vérification de l'indépendance des mixins...
✅ RÈGLE 7: Tous les mixins sont indépendants

🔍 Test 8: Vérification que les interfaces sont purement abstraites...
✅ RÈGLE 8: Toutes les interfaces sont purement abstraites

🔍 Test 9: Vérification de l'organisation des classes concrètes...
✅ RÈGLE 9: Toutes les classes concrètes sont dans implementation/

🔍 Test 10: Vérification qu'aucune classe n'importe depuis implementation/...
✅ RÈGLE 10: Aucun import depuis implementation/ détecté

🔍 Test 11: Détection des dépendances circulaires entre fichiers...
   📁 Analyse de 32 fichiers headers...
   ✓ Aucun cycle détecté
✅ RÈGLE 11: Aucune dépendance circulaire entre fichiers détectée (32 fichiers vérifiés)

================================================================================
✅ Tous les tests SOLID passés avec succès !
================================================================================
```

### Échec (exit code 1) :
```
❌ VIOLATION RÈGLE 1: Rectangle.h inclut la classe concrète Circle
   Fichier: /path/to/Rectangle.h
   Include: core/models/shapes/implementations/Circle.h
   INTERDIT: Les classes concrètes ne doivent pas s'inclure mutuellement
   SOLUTION: Utiliser l'interface (ICircle) à la place

================================================================================
❌ 1 violation(s) d'architecture détectée(s)
================================================================================
```

## Corriger les violations

### Violation RÈGLE 1 : Dépendance entre classes concrètes

**Problème** : `Rectangle.h` inclut `Circle.h`

**Solution** :
1. Remplacer par l'interface : `#include "core/models/shapes/interfaces/IShape.h"`
2. Utiliser un pointeur d'interface : `IShape* shape;`
3. Utiliser le Factory Pattern : `IShape::create("Circle", params)`

### Violation RÈGLE 4 : Macro manquante

**Problème** : `Rectangle.h` manque `DECLARE_TYPE_NAME`

**Solution** : Ajouter dans la classe :
```cpp
class Rectangle : public IShape
{
    Q_OBJECT
    DECLARE_TYPE_NAME(Rectangle)  // Ajouter cette ligne
    // ...
};
```

### Violation RÈGLE 5 : Dépendance de package non autorisée

**Problème** : `models/` dépend de `services/`

**Solution** : Restructurer pour inverser la dépendance ou déplacer le code.

### Violation RÈGLE 10 : Import depuis implementation/

**Problème** : `MyClass.h` importe `shapes/implementation/Rectangle.h`

**Solution** :
1. Remplacer par l'interface : `#include "shapes/IShape.h"`
2. Utiliser le Factory Pattern pour créer les instances :
   ```cpp
   // Au lieu de :
   Rectangle* rect = new Rectangle(0, 0, 100, 50);

   // Utiliser :
   QVariantMap params;
   params["x"] = 0; params["y"] = 0;
   params["width"] = 100; params["height"] = 50;
   IShape* shape = IShape::create("Rectangle", params);
   ```
3. Dépendre des abstractions via les pointeurs d'interface : `IShape*`, `IPart*`, etc.

## Avantages de ce système

✅ **Automatique** : Détecte les violations dès le développement
✅ **Rapide** : Analyse statique (~1s pour tout le projet)
✅ **Complet** : Scanne TOUT le codebase
✅ **Documentation vivante** : Les tests documentent l'architecture
✅ **Prévention** : Empêche les violations avant le commit
✅ **Intégré Qt Creator** : Utilise les outils natifs Qt

## État actuel

**Dernière vérification** : 2025-11-24

### Résultats des tests d'architecture

```
✅ RÈGLE 1: Aucune dépendance entre classes concrètes détectée
✅ RÈGLE 2: Toutes les dépendances utilisent les interfaces
✅ RÈGLE 3: Toutes les implémentations héritent de leur interface
✅ RÈGLE 4: Tous les patterns sont correctement utilisés
✅ RÈGLE 5: Hiérarchie des packages respectée
✅ RÈGLE 6: Aucune dépendance circulaire détectée (19 packages vérifiés)
✅ RÈGLE 7: Tous les mixins sont indépendants
✅ RÈGLE 8: Toutes les interfaces sont purement abstraites
✅ RÈGLE 9: Toutes les classes concrètes sont dans implementation/
✅ RÈGLE 10: Aucun import depuis implementation/ détecté
✅ RÈGLE 11: Aucune dépendance circulaire entre fichiers détectée (32 fichiers vérifiés)

Tous les tests SOLID passés avec succès !
```

### Tests unitaires C++

```
✅ TestTypes:          11/11 tests
✅ TestShapes:         20/20 tests
✅ TestPart:           11/11 tests
✅ TestJoint:          16/16 tests
✅ TestProject:        12/12 tests
✅ TestConfigManager:  19/19 tests
✅ TestLogging:         9/9 tests
✅ TestPluginManager:  14/14 tests
✅ TestServiceLocator: 22/22 tests

Total: 134/134 tests réussis (100%)
```

### Corrections récentes

**Bug IJoint corrigé (2025-11-19)** :
- **Problème** : La fonction `connectToPart()` oubliait d'assigner le pointeur de membre `partMember = newPart`
- **Impact** : 3 tests échouaient dans `TestJoint` (m_partA et m_partB restaient à nullptr)
- **Solution** : Ajout de l'assignation manquante au début de `connectToPart()`
- **Fichier modifié** : `core/models/joints/interfaces/IJoint.cpp:70`

### Statut global

**Architecture** : ✅ 100% conforme aux principes SOLID
**Tests unitaires** : ✅ 134/134 passés (100%)
**Couverture de code** : ✅ Complète pour le module Core
**Documentation** : ✅ À jour avec Doxygen

## Références

- Projet Python inspirant : `ai-front-portal-main/tests/unit/test_dependency_constraints.py`
- Principes SOLID : https://en.wikipedia.org/wiki/SOLID
- clang-tidy : https://clang.llvm.org/extra/clang-tidy/
- Qt Creator Static Analyzer : https://doc.qt.io/qtcreator/creator-clang-tools.html
