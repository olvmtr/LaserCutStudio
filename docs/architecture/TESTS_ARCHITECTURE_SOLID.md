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

### RÈGLE 6 : Pas de dépendances circulaires

**Principe** : Aucun cycle de dépendances entre packages.

❌ **INTERDIT** :
```
models/ → services/ → models/  (CYCLE)
```

Le test utilise un **algorithme DFS** pour détecter les cycles.

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

## Avantages de ce système

✅ **Automatique** : Détecte les violations dès le développement
✅ **Rapide** : Analyse statique (~1s pour tout le projet)
✅ **Complet** : Scanne TOUT le codebase
✅ **Documentation vivante** : Les tests documentent l'architecture
✅ **Prévention** : Empêche les violations avant le commit
✅ **Intégré Qt Creator** : Utilise les outils natifs Qt

## Références

- Projet Python inspirant : `ai-front-portal-main/tests/unit/test_dependency_constraints.py`
- Principes SOLID : https://en.wikipedia.org/wiki/SOLID
- clang-tidy : https://clang.llvm.org/extra/clang-tidy/
- Qt Creator Static Analyzer : https://doc.qt.io/qtcreator/creator-clang-tools.html
