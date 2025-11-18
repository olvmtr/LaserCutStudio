# TODO - Refactoring du Module Core

Ce fichier liste les duplications de code identifiées et les refactorings à réaliser.

**Date d'analyse** : 2025-11-18
**Lignes dupliquées totales** : 240-280 lignes
**Statut général** : En cours

---

## ✅ Refactorings Complétés

### 1. Factory Pattern avec FactoryMixin CRTP
- **Status** : ✅ Complété (commit f527d5b)
- **Lignes éliminées** : ~108 lignes
- **Impact** : Architecture extensible, API similaire à Q_OBJECT

### 2. Sérialisation avec toVariant() dans Interface
- **Status** : ✅ Complété (commit 08a690a)
- **Lignes éliminées** : ~100 lignes
- **Impact** : Sérialisation automatique via QMetaObject

---

## 📋 Refactorings Planifiés

### Phase 1 : Priorité HAUTE (1-2 jours)

#### 🎯 #1 - ListManagerMixin CRTP
- **Status** : ✅ COMPLÉTÉ (commit a21c557)
- **Priorité** : HAUTE
- **Lignes éliminées** : ~60 lignes
- **Effort réel** : Moyen (1.5h)
- **Impact** : Extensibilité +++

**Duplication identifiée** :
Les 4 interfaces (IShape, IPart, IJoint, IProject) implémentent le même pattern :
- `addX(X* item)` - Ajoute à la liste statique
- `removeX(X* item)` - Retire de la liste statique
- `clearAllX()` - Vide la liste statique
- `getAllX()` - Retourne la liste statique

**Fichiers concernés** :
- `core/shapes/IShape.cpp` (lignes 23-38)
- `core/parts/IPart.cpp` (lignes 82-97)
- `core/joints/IJoint.cpp` (lignes 133-148)
- `core/projects/IProject.cpp` (lignes 97-112)

**Solution proposée** :
```cpp
// core/patterns/ListManagerMixin.h
template<typename T>
class ListManagerMixin {
protected:
    static QList<T*> s_instances;
    void registerInstance(T* instance);
    void unregisterInstance(T* instance);
public:
    static QList<T*> getAllInstances();
    static void clearAllInstances();
    static int instanceCount();
};
```

**Usage** :
```cpp
class IShape : public Interface,
               protected FactoryMixin<IShape>,
               protected ListManagerMixin<IShape> {
    using ListManagerMixin<IShape>::getAllInstances;
    static QList<IShape*> getAllShapes() { return getAllInstances(); }
};
```

**Tests à ajouter** :
- Enregistrement automatique au constructeur
- Désenregistrement au destructeur
- getAllInstances() retourne toutes les instances
- clearAllInstances() vide la liste
- instanceCount() retourne le bon nombre

---

### Phase 2 : Priorité MOYENNE (2-3 jours)

#### #2 - GeometryUtils (Transformations géométriques)
- **Status** : ⏳ À faire
- **Priorité** : MOYENNE
- **Lignes dupliquées** : ~40 lignes
- **Effort estimé** : Moyen (2h)
- **Impact** : Testabilité +++

**Duplication identifiée** :
Rectangle et Circle répètent les calculs géométriques :
- Conversion angle degrés → radians
- Rotation 2D (matrice de rotation)
- Mise à l'échelle de points

**Fichiers concernés** :
- `core/shapes/Rectangle.cpp` (lignes 71-97, 99-122)
- `core/shapes/Circle.cpp` (lignes 79-91, 93-107)

**Solution proposée** :
```cpp
// core/utils/GeometryUtils.h
namespace LaserCutStudio::Core::Utils {
    struct RotationMatrix {
        double cos, sin;
        static RotationMatrix fromDegrees(double angle);
    };

    Point2D rotatePoint(const Point2D& point, const Point2D& center,
                        const RotationMatrix& rot);
    Point2D scalePoint(const Point2D& point, const Point2D& center,
                       double scaleX, double scaleY);
}
```

**Tests à ajouter** :
- Test de conversion degrés → radians (0°, 90°, 180°, 270°, 360°)
- Test de rotation de points (quadrants)
- Test de mise à l'échelle
- Test de cas limites (angle négatif, échelle 0)

#### #3 - Macros pour Setters avec Signaux
- **Status** : ⏳ À faire
- **Priorité** : MOYENNE
- **Lignes dupliquées** : ~42 lignes
- **Effort estimé** : Faible (1h)
- **Impact** : Maintenance ++

**Duplication identifiée** :
Rectangle (7 setters) et Circle (3 setters) utilisent le même pattern :
```cpp
void Rectangle::setX(double x) {
    if (!qFuzzyCompare(m_x, x)) {
        m_x = x;
        emit xChanged(x);
        emit geometryChanged();
    }
}
```

**Fichiers concernés** :
- `core/shapes/Rectangle.cpp` (lignes 124-158)
- `core/shapes/Circle.cpp` (lignes 109-134)

**Solution proposée** :
```cpp
// Option 1 : Macro
#define DEFINE_PROPERTY_SETTER(type, propName, memberVar, signalName) \
    void set##propName(type value) { \
        if (!qFuzzyCompare(memberVar, value)) { \
            memberVar = value; \
            emit signalName(value); \
            emit geometryChanged(); \
        } \
    }

// Option 2 : Template protégé dans IShape
template<typename T, typename Signal>
void updateProperty(T& member, const T& newValue, Signal signal);
```

**Tests à vérifier** :
- Les tests existants doivent continuer à passer
- Vérifier que geometryChanged() est bien émis

---

### Phase 3 : Priorité BASSE (1 jour)

#### #4 - Constructeurs de Copie par Défaut
- **Status** : ⏳ À faire
- **Priorité** : BASSE
- **Lignes dupliquées** : ~40 lignes
- **Effort estimé** : Trivial (30 min)
- **Impact** : Lisibilité +

**Duplication identifiée** :
8 classes implémentent manuellement un constructeur de copie trivial.

**Solution proposée** :
```cpp
// Remplacer par :
Rectangle(const Rectangle& other) = default;
```

**Conditions** :
- ✅ Tous les membres sont copiables par défaut
- ✅ Pas de logique custom nécessaire
- ✅ Classe de base a constructeur de copie accessible

**Fichiers concernés** :
- Rectangle, Circle, Part, TabJoint, FingerJoint, Project

#### #5 - Méthode clone() avec Macro
- **Status** : ⏳ À faire
- **Priorité** : BASSE
- **Lignes dupliquées** : ~24 lignes
- **Effort estimé** : Faible (1h)
- **Impact** : Cohérence +

**Duplication identifiée** :
8 classes implémentent `clone()` identiquement :
```cpp
IShape* Rectangle::clone() const {
    return new Rectangle(*this);
}
```

**Solution proposée** :
```cpp
// core/patterns/ClonableMixin.h
#define IMPLEMENT_CLONE(ClassName, BaseClass) \
    BaseClass* clone() const override { \
        return new ClassName(*this); \
    }

// Usage dans Rectangle.cpp
IMPLEMENT_CLONE(Rectangle, IShape)
```

#### #6 - Auto-enregistrement Factory Amélioré
- **Status** : ⏳ À faire
- **Priorité** : BASSE
- **Lignes dupliquées** : ~16 lignes
- **Effort estimé** : Moyen (1h)
- **Impact** : Robustesse ++

**Duplication identifiée** :
Chaque classe déclare et initialise `s_registered` manuellement.

**Solution proposée** :
```cpp
// Intégrer dans FactoryMixin
template<typename Base>
class FactoryMixin {
protected:
    template<typename T>
    struct AutoRegister {
        AutoRegister() {
            FactoryMixin<Base>::registerFactory<T>();
        }
    };
};

// Usage (1 ligne)
namespace {
    FactoryMixin<IShape>::AutoRegister<Rectangle> g_registration;
}
```

#### #7 - Macro DECLARE_TYPE_NAME
- **Status** : ⏳ À faire
- **Priorité** : BASSE
- **Lignes dupliquées** : ~16 lignes
- **Effort estimé** : Trivial (30 min)
- **Impact** : Cohérence +

**Duplication identifiée** :
Chaque classe implémente le même pattern :
```cpp
static QString staticTypeName() { return "Rectangle"; }
QString getTypeName() const override { return staticTypeName(); }
```

**Solution proposée** :
```cpp
// core/interface/Interface.h
#define DECLARE_TYPE_NAME(TypeName) \
    static QString staticTypeName() { return #TypeName; } \
    QString getTypeName() const override { return staticTypeName(); }

// Usage
class Rectangle : public IShape {
public:
    DECLARE_TYPE_NAME(Rectangle)
};
```

#### #8 - Helper pour Signaux aboutToBeDestroyed
- **Status** : ⏳ À faire
- **Priorité** : BASSE
- **Lignes dupliquées** : ~40 lignes
- **Effort estimé** : Faible (1h)
- **Impact** : Clarté ++

**Duplication identifiée** :
IJoint répète 4 fois la logique de connexion/déconnexion aux signaux aboutToBeDestroyed.

**Fichiers concernés** :
- `core/joints/IJoint.cpp` (lignes 36-52, 92-109, 114-125)

**Solution proposée** :
```cpp
// IJoint.h
private:
    void connectToPart(IPart*& partMember, IPart* newPart);
    void disconnectFromPart(IPart*& partMember);

// Usage
void IJoint::connect(IPart* partA, IPart* partB) {
    disconnectFromPart(m_partA);
    disconnectFromPart(m_partB);
    connectToPart(m_partA, partA);
    connectToPart(m_partB, partB);
}
```

---

## 📊 Récapitulatif

| Phase | Refactorings | Lignes | Priorité | Effort | Status |
|-------|--------------|--------|----------|--------|--------|
| ✅ Complété | FactoryMixin + toVariant | ~208 | - | - | ✅ Done |
| ✅ Phase 1 | ListManagerMixin | ~60 | HAUTE | 1.5h | ✅ Done |
| Phase 2 | GeometryUtils + Setters | ~82 | MOYENNE | 3h | ⏳ À faire |
| Phase 3 | Refactorings mineurs | ~130 | BASSE | 3h | ⏳ À faire |
| **TOTAL** | **8 refactorings** | **~480** | - | **~8h** | **56% fait** |

---

## 🎯 Objectifs

- [x] **Phase 0** : Factory Pattern + Sérialisation (208 lignes) - **COMPLÉTÉ**
- [x] **Phase 1** : Gestion des listes (60 lignes) - **COMPLÉTÉ**
- [ ] **Phase 2** : Utils géométriques (82 lignes)
- [ ] **Phase 3** : Refactorings mineurs (130 lignes)

**Objectif final** : Éliminer ~480 lignes de code dupliqué au total
**Progression actuelle** : **268/480 lignes éliminées (56%)**

---

## 📝 Notes

- Privilégier la **lisibilité** sur l'élimination absolue de duplication (principe DRY avec modération)
- Chaque refactoring doit **passer tous les tests** (134 tests actuels)
- Documenter chaque nouveau pattern dans `CLAUDE.md`
- Créer des exemples d'utilisation dans les README des patterns

---

**Dernière mise à jour** : 2025-11-18
**Étape complétée** : ListManagerMixin (Phase 1) ✅
**Prochaine étape** : GeometryUtils (Phase 2)
