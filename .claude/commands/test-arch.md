# Test Architecture SOLID

Lance automatiquement les tests d'architecture SOLID pour vérifier la conformité du code C++/Qt.

**Action automatique:**
Claude lance immédiatement `python3 DevTools/architecture/test_architecture_solid.py` et affiche les résultats.

**Caractéristiques:**
- Rapide (~1-2 secondes)
- Analyse statique du code (pas de compilation)
- Détection automatique des violations
- Rapport détaillé généré

**Ce qui est testé:**
- ✅ RÈGLE 1: Pas de dépendances entre classes concrètes
- ✅ RÈGLE 2: Seules les interfaces utilisées entre classes
- ✅ RÈGLE 3: Toutes implémentations héritent d'interface
- ✅ RÈGLE 4: Patterns utilisés (DECLARE_TYPE_NAME, IMPLEMENT_CLONE, Mixins)
- ✅ RÈGLE 5: Hiérarchie des packages respectée (models/ ← services/ ← infrastructure/)
- ✅ RÈGLE 6: Pas de dépendances circulaires
- ✅ RÈGLE 7: Mixins totalement indépendants (Qt/stdlib uniquement)

**Rapport généré:**
- `generated/reports/architecture/architecture_report_YYYYMMDD_HHMMSS.txt`
- `generated/reports/architecture/latest.txt` (lien vers le dernier)

Le dossier `generated/` est dans `.gitignore` et ne sera pas commité.

**Usage recommandé:**
- Avant chaque commit
- Après ajout de nouvelles classes
- Après refactoring architecture
- Validation continue en développement

**Exit codes:**
- 0 : ✅ Tous les tests passent
- 1 : ❌ Des violations détectées

**Corriger les violations:**
Si des violations sont détectées, utilise `/fix-arch` pour obtenir de l'aide.

---

## Rapport d'amélioration de la commande

Après l'exécution, **demander à l'utilisateur** : "Veux-tu un rapport d'amélioration de la commande ? (oui/non)"

Si **oui**, générer un rapport dans `generated/reports/architecture/rapport-commande-{timestamp}.md` contenant :
- Efficacité de la commande
- Problèmes rencontrés
- Clarté des instructions
- Améliorations proposées
