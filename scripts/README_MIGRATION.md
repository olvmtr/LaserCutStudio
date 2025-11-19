# Guide de Migration - Architecture en Couches

Ce répertoire contient les scripts pour migrer LaserCutStudio vers une **architecture en couches** (models / services / infrastructure / utils) inspirée du projet Python AI-Front-Portal.

## 🎯 Objectif

Passer d'une organisation plate :
```
core/
├── config/
├── di/
├── interface/
├── joints/
├── logging/
├── parts/
├── patterns/
├── plugins/
├── projects/
├── shapes/
├── types/
└── utils/
```

À une organisation hiérarchique par couches :
```
core/
├── models/              # Domaine métier (formes, pièces, joints, projets)
│   ├── base/
│   ├── shapes/
│   ├── parts/
│   ├── joints/
│   └── projects/
│
├── services/            # Logique applicative (géométrie, validation)
│   ├── geometry/
│   ├── validation/
│   └── serialization/
│
├── infrastructure/      # Aspects techniques (patterns, config, logging, DI, plugins)
│   ├── patterns/
│   ├── config/
│   ├── logging/
│   ├── di/
│   └── plugins/
│
└── utils/               # Utilitaires transverses
```

## 📋 Scripts Disponibles

### 1️⃣ `01_create_new_structure.sh`
Crée l'arborescence de la nouvelle structure (répertoires vides).

**Usage** :
```bash
./scripts/01_create_new_structure.sh
```

**Sortie** :
- Crée `models/`, `services/`, `infrastructure/`, `utils_new/` avec sous-packages

---

### 2️⃣ `02_migrate_files.sh`
Déplace tous les fichiers vers la nouvelle structure avec `git mv` (préserve l'historique Git).

**Usage** :
```bash
./scripts/02_migrate_files.sh
```

**Prérequis** :
- ✅ Dépôt Git propre (pas de modifications non commitées)
- ✅ Script 1 exécuté

**Actions** :
- Déplace tous les `.h` et `.cpp` vers les nouveaux emplacements
- Utilise `git mv` pour préserver l'historique
- Supprime les anciens répertoires vides
- Renomme `utils_new/` → `utils/`

---

### 3️⃣ `03_update_includes.sh`
Met à jour tous les `#include` dans les fichiers sources pour refléter les nouveaux chemins.

**Usage** :
```bash
./scripts/03_update_includes.sh
```

**Prérequis** :
- ✅ Script 2 exécuté

**Actions** :
- Crée une sauvegarde dans `.migration_backup_YYYYMMDD_HHMMSS/`
- Remplace tous les includes relatifs par des chemins absolus depuis `src/LaserCutStudio/`
- Traite les fichiers de `core/`, `tests/`, et `main.cpp`

**Exemples de transformations** :
```cpp
// Avant
#include "../interface/Interface.h"
#include "IShape.h"
#include "../patterns/FactoryMixin.h"

// Après
#include "core/models/base/Interface.h"
#include "core/models/shapes/interfaces/IShape.h"
#include "core/infrastructure/patterns/factory/FactoryMixin.h"
```

---

### 4️⃣ `04_update_cmake.sh`
Génère un nouveau `CMakeLists.txt` pour la nouvelle structure et teste la compilation.

**Usage** :
```bash
./scripts/04_update_cmake.sh
```

**Prérequis** :
- ✅ Script 3 exécuté

**Actions** :
- Sauvegarde l'ancien `CMakeLists.txt`
- Génère un nouveau fichier organisé par couches
- Teste la configuration CMake
- Restaure l'ancien en cas d'erreur

---

### 🚀 `run_full_migration.sh` (Script Master)
Orchestre toute la migration en exécutant les 4 scripts dans l'ordre.

**Usage** :
```bash
./scripts/run_full_migration.sh
```

**Actions** :
1. Vérifie que tous les scripts sont présents
2. Crée un commit de sauvegarde Git
3. Demande confirmation
4. Exécute les 4 scripts séquentiellement
5. Affiche les instructions post-migration

**Sécurité** :
- ✅ Commit de sauvegarde automatique avant migration
- ✅ Rollback possible avec `git reset --hard <commit>`
- ✅ Arrêt immédiat en cas d'erreur (`set -e`)

---

### ✅ `validate_migration.sh`
Valide que la migration s'est bien déroulée en exécutant 7 checks.

**Usage** :
```bash
./scripts/validate_migration.sh
```

**Checks effectués** :
1. ✓ Structure des répertoires (models/, services/, infrastructure/, utils/)
2. ✓ Nettoyage des anciens répertoires
3. ✓ Migration complète des fichiers
4. ✓ Compilation Debug
5. ✓ Tests unitaires
6. ✓ Compilation Release
7. ✓ Compilation des benchmarks

**Sortie** :
```
Checks réussis : 7/7
✅ ✅ ✅  VALIDATION COMPLÈTE RÉUSSIE ! ✅ ✅ ✅
```

## 🛠️ Workflow Complet

### Migration Automatique (Recommandé)

```bash
# 1. Sauvegarder l'état actuel
git add -A
git commit -m "chore: état avant migration architecture"

# 2. Lancer la migration complète
./scripts/run_full_migration.sh

# 3. Valider la migration
./scripts/validate_migration.sh

# 4. Si tout est OK, commiter
git add -A
git commit -m "refactor: migration architecture en couches (models/services/infrastructure)"

# 5. Si problème, rollback
git reset --hard <commit-de-sauvegarde>
```

### Migration Manuelle (Étape par Étape)

```bash
# 1. Créer la structure
./scripts/01_create_new_structure.sh

# 2. Migrer les fichiers
./scripts/02_migrate_files.sh
git status  # Vérifier les modifications

# 3. Mettre à jour les includes
./scripts/03_update_includes.sh

# 4. Mettre à jour CMake et compiler
./scripts/04_update_cmake.sh

# 5. Valider
./scripts/validate_migration.sh

# 6. Commit
git add -A
git commit -m "refactor: migration architecture en couches"
```

## ⚠️ Points d'Attention

### Avant de Migrer

1. **Commiter tout changement en cours** :
   ```bash
   git add -A
   git commit -m "wip: travail en cours"
   ```

2. **Vérifier que les tests passent** :
   ```bash
   cd build/Desktop-Debug
   ctest --output-on-failure
   ```

3. **Créer une branche dédiée** (optionnel) :
   ```bash
   git checkout -b refactor/architecture-layers
   ```

### Pendant la Migration

- Les scripts s'arrêtent immédiatement en cas d'erreur (`set -e`)
- Une sauvegarde est créée dans `.migration_backup_YYYYMMDD_HHMMSS/`
- L'historique Git est préservé avec `git mv`

### Après la Migration

1. **Compiler en Debug et Release** :
   ```bash
   cd build/Desktop-Debug && cmake --build . --clean-first
   cd build/Desktop-Release && cmake --build . --clean-first
   ```

2. **Exécuter tous les tests** :
   ```bash
   cd build/Desktop-Debug
   ctest --output-on-failure
   ```

3. **Exécuter les benchmarks** :
   ```bash
   ./tests/benchmarks/LaserCutStudioBenchmarks
   ```

4. **Mettre à jour la documentation** :
   - `CLAUDE.md` : Mettre à jour la section "Structure du projet"
   - `docs/notes/development-plan.md` : Marquer la migration comme complétée

## 🔄 Rollback

En cas de problème, revenir à l'état précédent :

```bash
# Option 1 : Rollback Git (si commit non poussé)
git log --oneline -5  # Trouver le commit de sauvegarde
git reset --hard <commit-de-sauvegarde>

# Option 2 : Restaurer depuis la sauvegarde
ls -d .migration_backup_*  # Trouver la sauvegarde
cp -r .migration_backup_YYYYMMDD_HHMMSS/core/* src/LaserCutStudio/core/
```

## 📊 Résumé de la Migration

| Aspect | Avant | Après |
|--------|-------|-------|
| **Niveaux hiérarchiques** | 2 (core/module/) | 4 (core/couche/domaine/impl/) |
| **Répertoires** | 13 au même niveau | 3 couches + 15 sous-packages |
| **Chemins include** | Relatifs (`../interface/`) | Absolus (`core/models/base/`) |
| **Dépendances** | Implicites | Explicites par couche |
| **Évolutivité** | Limitée | Excellente (ajout facile de sous-packages) |

## 📚 Documentation Associée

- **Plan de migration** : `docs/notes/migration-plan.md`
- **Architecture Python** : `ai-front-portal-main/src/` (référence)
- **CLAUDE.md** : Instructions Claude Code (à mettre à jour après migration)

## ❓ Questions Fréquentes

**Q : Puis-je annuler la migration après exécution ?**
R : Oui, avec `git reset --hard <commit-de-sauvegarde>`. Le commit de sauvegarde est créé automatiquement.

**Q : Les tests vont-ils passer après migration ?**
R : Oui, si les includes sont correctement mis à jour. Le script `validate_migration.sh` le vérifie.

**Q : L'historique Git est-il préservé ?**
R : Oui, grâce à `git mv`. Git suit les déplacements de fichiers.

**Q : Combien de temps prend la migration ?**
R : Environ 2-5 minutes (dépend de la taille du projet).

**Q : Puis-je exécuter les scripts plusieurs fois ?**
R : Non, les scripts ne sont pas idempotents. Rollback Git requis pour réexécuter.

## ✅ Checklist Post-Migration

- [ ] Compilation Debug réussie
- [ ] Compilation Release réussie
- [ ] Tous les tests unitaires passent
- [ ] Benchmarks compilent et s'exécutent
- [ ] Documentation mise à jour (CLAUDE.md)
- [ ] Commit de migration créé
- [ ] Tests manuels de l'application effectués

---

**Auteur** : Claude Code
**Date** : 2025-11-19
**Version** : 1.0
