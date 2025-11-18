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
# Depuis le répertoire src/LaserCutStudio
mkdir -p build/Desktop-Debug
cd build/Desktop-Debug
cmake ../..
cmake --build .

# Exécuter l'application
./appLaserCutStudio
```

L'exécutable se nomme `appLaserCutStudio`.

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

## Améliorations suggérées

### Améliorations prioritaires pour LaserCutStudio C++

1. **Configuration externe**
   - Ajouter système QSettings ou YAML pour configuration (matériaux, unités, préférences)
   - Remplacer les valeurs hardcodées par configuration externe
   - Fichier type : `config/materials.yaml`, `config/app.ini`

2. **Logging structuré**
   - Implémenter logging avec catégories Qt (`qCDebug`, `qCInfo`, `qCWarning`)
   - Créer catégories : `core.shapes`, `core.parts`, `core.joints`, `core.projects`
   - Ajouter niveaux de verbosité configurables

3. **Tests de performance**
   - Ajouter benchmarks avec `QTest::qBenchmark()` pour opérations critiques
   - Tester : clonage d'objets, calculs géométriques, sérialisation
   - Créer suite `tests/benchmarks/` séparée

4. **Système de plugins**
   - Implémenter `QPluginLoader` pour formes/joints extensibles
   - Interface plugin : `IShapePlugin`, `IJointPlugin`
   - Permettre ajout de nouvelles formes sans recompilation

5. **Dependency Injection**
   - Créer Service Locator pattern pour découpler dépendances
   - Faciliter tests unitaires avec mocks
   - Remplacer construction directe par injection

6. **Documentation auto-générée**
   - Configurer Doxygen pour génération automatique
   - Ajouter commentaires Doxygen aux interfaces principales
   - Générer documentation HTML/PDF

**Note** : Ces améliorations ne bloquent pas le développement des phases suivantes (UI, 3D, Export). Elles peuvent être implémentées progressivement pour améliorer la maintenabilité.
