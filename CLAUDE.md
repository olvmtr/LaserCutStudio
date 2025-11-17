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

Le projet est en phase initiale de configuration. État actuel :
- Le squelette de base de l'application Qt Quick est en place
- La fenêtre principale s'affiche mais avec une fonctionnalité minimale
- Les classes du module Core (Shape, Part, Joint, Project) sont prévues mais pas encore implémentées
- L'éditeur 2D, la visionneuse 3D et les fonctionnalités d'export sont tous prévus

Consulter `docs/notes/development-plan.md` pour la feuille de route complète du développement par phases.
