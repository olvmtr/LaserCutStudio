# DevTools - Outils de développement LaserCutStudio

Outils de développement pour maintenir la qualité et l'architecture du projet.

## Structure

```
DevTools/
├── architecture/           # Tests d'architecture SOLID
│   ├── test_architecture_solid.py   # Script Python de tests
│   └── test_architecture.sh         # Script bash complet
└── README.md              # Ce fichier
```

## Tests d'architecture SOLID

### Utilisation

```bash
# Commande Claude (recommandé)
/test-arch

# Ligne de commande
python3 DevTools/architecture/test_architecture_solid.py

# Script complet avec clang-tidy
./DevTools/architecture/test_architecture.sh
```

### 7 règles SOLID vérifiées

1. **Pas de dépendances entre classes concrètes**
2. **Seules les interfaces entre classes**
3. **Toutes implémentations héritent d'interface**
4. **Patterns utilisés (Mixins, Macros)**
5. **Hiérarchie des packages respectée**
6. **Pas de dépendances circulaires**
7. **Mixins indépendants**

### Documentation

- Guide complet : `docs/architecture/TESTS_ARCHITECTURE_SOLID.md`
- Configuration : `.clang-tidy`
- CLAUDE.md : Instructions pour Claude

## Commandes Claude

| Commande | Description |
|----------|-------------|
| `/test-arch` | Exécute les tests d'architecture |
| `/fix-arch` | Aide à corriger les violations |

## Workflow

```bash
# Avant chaque commit
/test-arch

# Si violations détectées
/fix-arch

# Vérifier que tout passe
/test-arch
```
