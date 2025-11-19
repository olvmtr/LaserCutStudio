# Design Patterns

Ce répertoire contient les design patterns réutilisables utilisés dans le module Core.

## FactoryMixin - CRTP pour Factory Pattern

### Vue d'ensemble

`FactoryMixin<Base>` est un mixin template qui fournit automatiquement le Factory Pattern à n'importe quelle classe de base. Il élimine la duplication de code et offre une API cohérente similaire à `Q_OBJECT` de Qt.

### Avantages

✅ **Zéro duplication** : Le code du Factory Pattern est écrit une seule fois
✅ **Type-safe** : Chaque instanciation du template a son propre `s_factories` statique
✅ **Performance** : Résolution à la compilation (CRTP = pas de virtual calls)
✅ **Extensible** : Facile d'ajouter de nouvelles fonctionnalités au mixin
✅ **Familier** : API similaire à `Q_OBJECT` de Qt

### Utilisation

#### 1. Dans votre interface de base

```cpp
#include "../patterns/FactoryMixin.h"

class MyInterface : public QObject,
                   protected Patterns::FactoryMixin<MyInterface>
{
    Q_OBJECT
public:
    // Exposer les méthodes du Factory Pattern
    using FactoryMixin<MyInterface>::create;
    using FactoryMixin<MyInterface>::availableTypes;
    using FactoryMixin<MyInterface>::registerFactory;

    // Vos méthodes virtuelles...
    virtual QString getTypeName() const = 0;
};
```

#### 2. Dans votre classe concrète

**Header (.h)**
```cpp
class ConcreteClass : public MyInterface
{
    Q_OBJECT
public:
    // Nom de type statique (requis pour l'enregistrement)
    static QString staticTypeName() { return "ConcreteClass"; }

    // Implémentation...
    QString getTypeName() const override { return staticTypeName(); }

private:
    static const bool s_registered; // Auto-enregistrement
};
```

**Implementation (.cpp)**
```cpp
// Auto-enregistrement au démarrage de l'application
const bool ConcreteClass::s_registered =
    MyInterface::registerFactory<ConcreteClass>();
```

#### 3. Utilisation du Factory

```cpp
// Créer une instance depuis une configuration
QVariantMap config;
config["type"] = "ConcreteClass";
config["property1"] = 42;
config["property2"] = "hello";

MyInterface* obj = MyInterface::create(config);
// Les propriétés Q_PROPERTY sont automatiquement configurées !

// Lister les types disponibles
QStringList types = MyInterface::availableTypes();
// => ["ConcreteClass", "AnotherClass", ...]

// Vérifier si un type est enregistré
bool exists = MyInterface::isTypeRegistered("ConcreteClass"); // true

// Nombre de types enregistrés
int count = MyInterface::registeredTypeCount();
```

### Comment ça marche ?

**CRTP (Curiously Recurring Template Pattern)** :
```cpp
template<typename Base>
class FactoryMixin {
protected:
    static QMap<QString, FactoryFunc> s_factories;
    // ...
};

// Chaque instanciation du template a son propre s_factories statique :
// FactoryMixin<IShape>   → IShape::s_factories
// FactoryMixin<IPart>    → IPart::s_factories
// FactoryMixin<IJoint>   → IJoint::s_factories
// FactoryMixin<IProject> → IProject::s_factories
```

### Configuration automatique avec Q_PROPERTY

Le FactoryMixin utilise `QMetaObject` pour configurer automatiquement les objets créés :

```cpp
class Rectangle : public IShape
{
    Q_OBJECT
    Q_PROPERTY(double x READ getX WRITE setX)
    Q_PROPERTY(double y READ getY WRITE setY)
    Q_PROPERTY(double width READ getWidth WRITE setWidth)
    Q_PROPERTY(double height READ getHeight WRITE setHeight)
public:
    static QString staticTypeName() { return "Rectangle"; }
    // Getters/setters...
};

// Utilisation :
QVariantMap config;
config["type"] = "Rectangle";
config["x"] = 10.0;
config["y"] = 20.0;
config["width"] = 100.0;
config["height"] = 50.0;

IShape* rect = IShape::create(config);
// Rectangle créé avec x=10, y=20, width=100, height=50 !
```

### Méthodes disponibles

| Méthode | Description |
|---------|-------------|
| `registerFactory<T>()` | Enregistre une classe concrète T dans le Factory |
| `create(config)` | Crée une instance depuis QVariantMap |
| `availableTypes()` | Liste tous les types enregistrés |
| `isTypeRegistered(typeName)` | Vérifie si un type est enregistré |
| `registeredTypeCount()` | Nombre de types enregistrés |
| `clearFactories()` | Vide tous les enregistrements (pour tests) |

### Exemple complet : IShape

```cpp
// IShape hérite du mixin
class IShape : public Interface, protected Patterns::FactoryMixin<IShape>
{
    Q_OBJECT
public:
    using FactoryMixin<IShape>::create;
    using FactoryMixin<IShape>::availableTypes;
    using FactoryMixin<IShape>::registerFactory;

    virtual QString getTypeName() const = 0;
    virtual double getArea() const = 0;
    // ...
};

// Rectangle s'auto-enregistre
class Rectangle : public IShape
{
    Q_OBJECT
    Q_PROPERTY(double x READ getX WRITE setX)
    Q_PROPERTY(double y READ getY WRITE setY)
    Q_PROPERTY(double width READ getWidth WRITE setWidth)
    Q_PROPERTY(double height READ getHeight WRITE setHeight)
public:
    static QString staticTypeName() { return "Rectangle"; }
private:
    static const bool s_registered;
};

const bool Rectangle::s_registered = IShape::registerFactory<Rectangle>();

// Circle s'auto-enregistre aussi
class Circle : public IShape
{
    Q_OBJECT
    Q_PROPERTY(double centerX READ getCenterX WRITE setCenterX)
    Q_PROPERTY(double centerY READ getCenterY WRITE setCenterY)
    Q_PROPERTY(double radius READ getRadius WRITE setRadius)
public:
    static QString staticTypeName() { return "Circle"; }
private:
    static const bool s_registered;
};

const bool Circle::s_registered = IShape::registerFactory<Circle>();

// Utilisation
QStringList types = IShape::availableTypes();  // ["Rectangle", "Circle"]

QVariantMap rectConfig;
rectConfig["type"] = "Rectangle";
rectConfig["width"] = 100.0;
rectConfig["height"] = 50.0;
IShape* rect = IShape::create(rectConfig);

QVariantMap circleConfig;
circleConfig["type"] = "Circle";
circleConfig["radius"] = 25.0;
IShape* circle = IShape::create(circleConfig);
```

### Comparaison avec Q_OBJECT

| Aspect | Q_OBJECT | FactoryMixin |
|--------|----------|--------------|
| **Préprocesseur** | Oui (MOC) | Non (pure C++) |
| **Templates** | Non | Oui (CRTP) |
| **Signals/Slots** | Oui | Non (utiliser Q_OBJECT pour ça) |
| **Introspection** | Oui (QMetaObject) | Utilise QMetaObject |
| **Performance** | Runtime | Compile-time |
| **Duplication** | Automatique (MOC) | Éliminée (template) |

**Recommandation** : Utiliser **les deux ensemble** ! `Q_OBJECT` pour Signals/Slots et métadonnées, `FactoryMixin` pour Factory Pattern sans duplication.

```cpp
class IShape : public Interface,                    // Q_OBJECT hérite de QObject
              protected Patterns::FactoryMixin<IShape>  // Factory Pattern
{
    Q_OBJECT  // Pour Signals/Slots

signals:
    void geometryChanged();  // Grâce à Q_OBJECT

public:
    using FactoryMixin<IShape>::create;  // Grâce à FactoryMixin
    // ...
};
```

### Tests

Le `FactoryMixin` est testé de manière exhaustive via les tests des 4 interfaces qui l'utilisent :
- **TestShapes** : 25 tests du Factory Pattern pour IShape
- **TestPart** : 22 tests du Factory Pattern pour IPart
- **TestJoint** : 23 tests du Factory Pattern pour IJoint
- **TestProject** : 22 tests du Factory Pattern pour IProject

**Total : 92 tests couvrant le FactoryMixin** ✅

Ces tests vérifient :
- Enregistrement des factories
- Création d'instances depuis QVariantMap
- Configuration automatique des Q_PROPERTY
- Gestion des types inconnus
- Liste des types disponibles
