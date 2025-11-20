# ⚠️ TODO : Révision ISelection

**Date**: 2025-11-20
**Priorité**: Moyenne (Phase 4+)
**Statut**: 🟡 À Examiner en Détail

---

## 📋 Contexte

L'interface `ISelection` contient actuellement **24 méthodes publiques**, ce qui dépasse légèrement la recommandation ISP (Interface Segregation Principle) de < 15 méthodes.

**Fichier concerné** : `src/LaserCutStudio/core/models/editor/interfaces/ISelection.h`

---

## 🔍 Analyse Rapide

### Répartition des Méthodes

| Groupe | Méthodes | Rôle |
|--------|----------|------|
| 1. État et requêtes | 6 | getCount, isEmpty, getSelectedShapes, getBoundingRect, getCenter, isSelected |
| 2. Gestion sélection | 6 | addShape, removeShape, toggleShape, selectAll, clear, selectInRect |
| 3. Opérations groupées | 5 | moveBy, rotateBy, scaleBy, deleteSelection, duplicate |
| 4. Alignement | 6 | alignLeft/Right/Top/Bottom/CenterH/CenterV |
| 5. Distribution | 2 | distributeHorizontally, distributeVertically |
| **Total** | **24** | |

### Observation Initiale

- ✅ Tous les clients (Canvas2DView, EditorService) utilisent **TOUTES** les méthodes
- ✅ Cohésion forte : l'alignement nécessite toujours une sélection active
- ✅ Standards industriels (Adobe, Qt, Figma) ne séparent pas alignement/sélection
- ⚠️ Nombre de méthodes au-dessus de la recommandation (24 vs 15)

---

## 🎯 Questions à Examiner en Détail

### 1. Split ISelection + ISelectionAlignment ?

**Option A** : Séparer en 2 interfaces
```cpp
ISelection (17 méthodes) : État + Gestion + Opérations
ISelectionAlignment (8 méthodes) : Alignement + Distribution
```

**Avantages** :
- ✅ Respect strict ISP (< 15 méthodes chacune)
- ✅ Séparation conceptuelle claire

**Inconvénients** :
- ❌ Alignement dépend fortement de sélection (couplage inévitable)
- ❌ API moins ergonomique : `aligner->alignLeft(selection)` vs `selection->alignLeft()`
- ❌ Complexité accrue (+20% code) sans bénéfice tangible

### 2. Garder Interface Unifiée ?

**Arguments POUR** :
- Tous les clients utilisent toutes les méthodes (pas de violation ISP détectée)
- Cohésion fonctionnelle forte entre groupes
- Standards industriels font pareil
- API intuitive et simple

**Arguments CONTRE** :
- Nombre de méthodes élevé (peut intimider nouveaux développeurs)
- Croissance potentielle si fonctionnalités ajoutées

### 3. Alternative : Helper Optionnel ?

**Option B** : ISelection inchangée + SelectionAlignmentHelper (Phase 4+)

```cpp
// API de base (simple)
selection->alignLeft();

// Opérations avancées (helper optionnel)
SelectionAlignmentHelper helper(selection);
helper.alignToGrid(10);
helper.distributeWithSpacing(5.0);
```

**Avantages** :
- ✅ ISelection reste simple
- ✅ Extensibilité sans modifier interface
- ✅ Helper utilisé uniquement si nécessaire

---

## 📝 Actions Recommandées

### Court Terme (Immédiat)

1. ✅ **NE PAS modifier ISelection pour l'instant**
   - Architecture actuelle est fonctionnelle
   - ISP n'est pas violé (tous les clients utilisent tout)
   - Score actuel : 9/10 (excellent)

2. ✅ **Améliorer documentation** (déjà fait)
   - Groupes fonctionnels clairement documentés dans ISelection.h
   - Rapport complet dans `docs/architecture/EVALUATION_ISELECTION_SPLIT.md`

### Moyen Terme (Phase 4+)

3. **Réévaluer si :**
   - Interface dépasse 30-35 méthodes
   - Des clients n'utilisent QUE certains groupes (violation ISP détectée)
   - Besoin d'opérations avancées (alignement sur grille, distribution circulaire, etc.)

4. **Considérer SelectionAlignmentHelper** si :
   - Besoin d'alignement avancé (grille, chemins, distribution avec espacement)
   - ISelection doit rester stable (pas de breaking changes)

### Long Terme (Phase 6+)

5. **Si split devient nécessaire** :
   - Utiliser pattern Composition (pas Héritage)
   - Maintenir rétro-compatibilité avec wrapper
   - Migrer progressivement (deprecate old API)

---

## 📚 Documentation Complète

**Rapport d'Évaluation Complet** (500+ lignes) :
👉 `docs/architecture/EVALUATION_ISELECTION_SPLIT.md`

**Contenu** :
- Analyse détaillée des 3 options de split
- Comparaison avec standards industriels (Adobe, Qt, Figma)
- Analyse coût/bénéfice (+20% complexité pour 0 bénéfice)
- 3 scénarios d'usage comparés
- Graphe de dépendances montrant couplage fort
- Recommandation finale : **NE PAS SPLITTER**

---

## ✅ Critères de Réévaluation

**Déclencher révision si :**

| Critère | Valeur Actuelle | Seuil d'Alerte |
|---------|-----------------|----------------|
| Nombre de méthodes | 24 | > 35 |
| Clients utilisant tout | 100% | < 80% |
| Complexité cyclomatique | Faible | Élevée |
| Demandes de split | 0 | > 2 demandes |

**Prochaine révision recommandée** : Fin Phase 4 (après intégration éditeur 2D complet)

---

## 🔗 Fichiers Liés

- `src/LaserCutStudio/core/models/editor/interfaces/ISelection.h` - Interface actuelle
- `docs/architecture/EVALUATION_ISELECTION_SPLIT.md` - Rapport d'évaluation complet
- `docs/architecture/AUDIT_ARCHITECTURE.md` - Audit global architecture (section ISP)

---

**Statut actuel** : ✅ **Architecture saine, pas d'action immédiate requise**

**Note** : Ce fichier est un rappel pour révision future. L'architecture actuelle obtient **9/10** en ISP.

---

*Créé le 2025-11-20 suite à audit architecture complet*
*À supprimer après révision complète en Phase 4+*
