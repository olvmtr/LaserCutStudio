# Corriger Violations Architecture

Aide à corriger les violations d'architecture SOLID détectées par `/test-arch`.

**Prérequis:**
Lance d'abord `/test-arch` pour identifier les violations.

**Action:**
Claude analyse le dernier rapport dans `generated/reports/architecture/latest.txt` et propose des corrections pour chaque violation.

**Types de corrections:**
- ✅ Ajouter macros manquantes (DECLARE_TYPE_NAME, IMPLEMENT_CLONE)
- ✅ Remplacer includes de classes concrètes par interfaces
- ✅ Réorganiser packages (déplacer code)
- ✅ Supprimer dépendances circulaires
- ✅ Rendre mixins indépendants

**Workflow:**
1. `/test-arch` - Détecter les violations
2. `/fix-arch` - Analyser et proposer corrections
3. Appliquer les corrections (automatique ou manuel)
4. `/test-arch` - Vérifier que tout passe

**Exemple:**
```
Utilisateur: /test-arch
❌ 3 violations détectées

Utilisateur: /fix-arch
Claude: J'ai analysé les 3 violations :
1. Rectangle.h manque DECLARE_TYPE_NAME
   → J'ajoute la macro dans la classe
2. IShape.h dépend de infrastructure/
   → Je déplace FactoryMixin dans models/patterns/
3. ...
```

**Note:**
Claude applique les corrections automatiquement si possible.
Pour les changements complexes, il proposera un plan d'action.
