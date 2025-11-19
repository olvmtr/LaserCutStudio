# Changelog

Tous les changements notables de LaserCutStudio sont documentés dans ce fichier.

Le format est basé sur [Keep a Changelog](https://keepachangelog.com/fr/1.0.0/),
et ce projet adhère au [Semantic Versioning](https://semver.org/lang/fr/).

## [Unreleased]

### Added (2025-11-19)
- Documentation complète mise à jour dans tous les fichiers
- Section "État actuel" dans TESTS_ARCHITECTURE_SOLID.md
- Tableau récapitulatif des phases de développement dans development-plan.md
- Documentation détaillée de tous les design patterns dans architecture-patterns.md

### Fixed (2025-11-19)
- **Bug critique dans IJoint::connectToPart()** : Ajout de l'assignation manquante `partMember = newPart`
  - **Impact** : 3 tests échouaient (testTabJointConstruction, testTabJointConnect, testSignalsA)
  - **Cause** : Les pointeurs `m_partA` et `m_partB` restaient à `nullptr` après connexion
  - **Fichier** : `core/models/joints/interfaces/IJoint.cpp:70`
  - **Tests** : 134/134 tests passent maintenant (100%)

### Changed (2025-11-19)
- Mise à jour complète de toute la documentation du projet
- Refonte du development-plan.md avec statut détaillé de chaque phase
- Architecture-patterns.md réécrit avec tous les patterns modernes
- CLAUDE.md enrichi avec section sur la correction du bug IJoint

## [0.2.5] - Phase 2.5 complète

### Added
- Patterns CRTP : FactoryMixin, ListManagerMixin, PropertyMixin, ClonableMixin
- Utilitaires : GeometryUtils, DebugMacros.h
- Macros : DECLARE_TYPE_NAME, IMPLEMENT_CLONE
- Helpers : connectToPart/disconnectFromPart pour IJoint
- ConfigManager avec QSettings (18 tests)
- Logging structuré avec 7 catégories (9 tests)
- LogManager avec 4 formats
- Tests de performance (17 benchmarks)
- Système de plugins Qt (14 tests)
- PluginManager avec QPluginLoader
- Dependency Injection avec ServiceLocator (22 tests)
- Documentation Doxygen complète
- Tests d'architecture SOLID (7 règles)

### Performance
- Optimisations Release : 20-40% amélioration
- Code conditionnel #if DEBUG (0 overhead en Release)
- Macros : LIKELY, UNLIKELY, FORCE_INLINE
- QT_NO_DEBUG_OUTPUT / QT_NO_INFO_OUTPUT

### Removed
- ~480 lignes de duplication éliminées (100%)
  - FactoryMixin : 208 lignes
  - ListManagerMixin : 60 lignes
  - PropertyMixin : 42 lignes
  - GeometryUtils : 40 lignes
  - IMPLEMENT_CLONE : 24 lignes
  - DECLARE_TYPE_NAME : 16 lignes
  - AutoRegister : 16 lignes
  - Helpers aboutToBeDestroyed : 40 lignes

## [0.2.0] - Phase 2 complète

### Added
- Types utilitaires : Point2D, Point3D, Material, JointType
- Interface de base avec Prototype Pattern et UUID
- Factory Pattern avec QVariant/QMetaObject
- Signals/Slots Qt pour communication découplée
- IShape interface avec Rectangle et Circle
- IPart interface avec Part
- IJoint interface avec TabJoint et FingerJoint
- IProject interface avec Project
- Tests unitaires complets :
  - TestTypes : 11 tests
  - TestShapes : 20 tests
  - TestPart : 11 tests
  - TestJoint : 16 tests
  - TestProject : 12 tests

### Architecture
- Hiérarchie complète Interface → I* → Implémentations
- Pattern Prototype avec clone()
- Factory Pattern avec create()
- Sérialisation automatique avec toVariant()
- Gestion de listes par interface

## [0.1.0] - Phase 1 complète

### Added
- Configuration CMake
- Structure de dossiers du projet
- Squelette Qt Quick avec QML
- Configuration de build Debug/Release
- Tests de base avec QTest
- Configuration Qt Creator
- Git repository initialisé

### Infrastructure
- CMakeLists.txt principal
- Options de build (BUILD_BENCHMARKS, etc.)
- Support de compilation multi-plateforme
- Qt 6.4+ requis

---

## Légende

- **Added** : Nouvelles fonctionnalités
- **Changed** : Modifications de fonctionnalités existantes
- **Deprecated** : Fonctionnalités obsolètes (à retirer prochainement)
- **Removed** : Fonctionnalités retirées
- **Fixed** : Corrections de bugs
- **Security** : Corrections de sécurité
- **Performance** : Améliorations de performance
- **Architecture** : Changements architecturaux

---

**Progression globale** : Phases 1-2.5 complètes (fondation solide pour la suite)

**Tests** : 134/134 (100%)
**Architecture** : 7/7 règles SOLID respectées
**Documentation** : 100% à jour

---

*Ce changelog est maintenu manuellement pour documenter tous les changements significatifs du projet LaserCutStudio.*
