# LibInterface

**Bibliothèque C++17/Qt6 réutilisable** fournissant des patterns CRTP, une classe de base Interface, et des utilitaires pour la Dependency Injection.

## 📦 Contenu

### Interface & Prototype Pattern
- **Interface** : Classe de base abstraite avec UUID unique
- Pattern Prototype : `clone()` pour duplication polymorphe
- Sérialisation automatique : `toVariant()` via `Q_PROPERTY`

### Patterns CRTP (Curiously Recurring Template Pattern)

#### FactoryMixin<Base>
Factory Pattern automatique avec :
- `create(QVariantMap)` : Création depuis configuration
- `registerFactory<T>()` : Enregistrement de types
- `AutoRegister<T>` : Auto-enregistrement au démarrage

#### ListManagerMixin<T>
Gestion de listes statiques avec :
- `getAllInstances()` : Liste de toutes les instances
- `clearAllInstances()` : Vide la liste
- `instanceCount()` : Comptage automatique

#### PropertyMixin<Derived>
Setters Qt simplifiés avec :
- `updateProperty()` : Setter automatique avec signaux
- Support : `double`, `int`, `QString`
- Comparaison automatique (`qFuzzyCompare` pour `double`)

### Macros

#### IMPLEMENT_CLONE(ClassName, BaseClass)
Implémente automatiquement `clone()` :
```cpp
IMPLEMENT_CLONE(Rectangle, IShape)
// Génère : IShape* Rectangle::clone() const { return new Rectangle(*this); }
```

#### DECLARE_TYPE_NAME(TypeName)
Déclare `staticTypeName()` et `getTypeName()` :
```cpp
DECLARE_TYPE_NAME(Rectangle)
// Génère les deux méthodes automatiquement
```

### Dependency Injection

#### ServiceLocator (Singleton)
- `registerSingleton<T>()` : Enregistre un service singleton
- `registerTransient<T>()` : Enregistre un service transient
- `resolve<T>()` : Résout une dépendance
- Thread-safe avec Meyers Singleton

### Debug & Performance

#### DebugMacros.h
- `DEBUG` : Défini automatiquement en mode Debug
- `LIKELY(x)` / `UNLIKELY(x)` : Optimisations branchement
- `FORCE_INLINE` : Force l'inlining
- Code conditionnel : `#if DEBUG ... #endif`

## 🚀 Utilisation

### CMake

```cmake
find_package(LibInterface 1.0 REQUIRED)

add_executable(MyApp main.cpp)
target_link_libraries(MyApp PRIVATE LibInterface::LibInterface)
```

### Exemple : Interface avec Factory Pattern

```cpp
#include <LibInterface/Interface.h>
#include <LibInterface/Patterns/FactoryMixin.h>
#include <LibInterface/Patterns/ListManagerMixin.h>

namespace MyApp {

class IShape : public LibInterface::Interface,
               protected LibInterface::Patterns::FactoryMixin<IShape>,
               protected LibInterface::Patterns::ListManagerMixin<IShape>
{
    Q_OBJECT
public:
    using FactoryMixin<IShape>::create;
    using FactoryMixin<IShape>::registerFactory;
    using ListManagerMixin<IShape>::getAllInstances;

    IShape* clone() const override = 0;
    virtual double getArea() const = 0;

protected:
    IShape() : Interface() { registerInstance(this); }
    ~IShape() override { unregisterInstance(this); }
};

class Rectangle : public IShape,
                  protected LibInterface::Patterns::PropertyMixin<Rectangle>
{
    Q_OBJECT
    Q_PROPERTY(double width READ getWidth WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(double height READ getHeight WRITE setHeight NOTIFY heightChanged)

signals:
    void widthChanged(double);
    void heightChanged(double);
    void geometryChanged();

public:
    DECLARE_TYPE_NAME(Rectangle)

    Rectangle() : m_width(0), m_height(0) {}

    IShape* clone() const override { return new Rectangle(*this); }
    double getArea() const override { return m_width * m_height; }

    double getWidth() const { return m_width; }
    double getHeight() const { return m_height; }

    void setWidth(double w) {
        updateProperty(m_width, w, &Rectangle::widthChanged, &Rectangle::geometryChanged);
    }

    void setHeight(double h) {
        updateProperty(m_height, h, &Rectangle::heightChanged, &Rectangle::geometryChanged);
    }

private:
    double m_width, m_height;
};

// Auto-enregistrement
namespace {
    LibInterface::Patterns::FactoryMixin<IShape>::AutoRegister<Rectangle> g_rectangleReg;
}

} // namespace MyApp
```

### Exemple : Dependency Injection

```cpp
#include <LibInterface/DI/ServiceLocator.h>

using LibInterface::DI::ServiceLocator;

// Enregistrement
ServiceLocator::instance().registerSingleton<ConfigManager>([]() {
    return new ConfigManager();
});

// Résolution
ConfigManager* config = ServiceLocator::instance().resolve<ConfigManager>();
```

## 📋 Prérequis

- **C++17** ou supérieur
- **Qt 6.4+** (Qt6::Core)
- **CMake 3.16+**

## 🔧 Compilation

```bash
cd LibInterface
mkdir build && cd build
cmake ..
cmake --build .
sudo cmake --install .
```

## 📝 Licence

À définir (voir projet parent LaserCutStudio)

## 🤝 Contribution

Cette bibliothèque est extraite du projet **LaserCutStudio** et contient tous les patterns réutilisables développés pour l'architecture Core.

## 🎯 Avantages

✅ **Zéro duplication** : Patterns CRTP éliminent le code boilerplate
✅ **Type-safe** : Vérifications à la compilation
✅ **Performance** : Templates résolus à la compilation
✅ **Qt-friendly** : Intégration native avec Qt (Signals/Slots, Q_PROPERTY)
✅ **Moderne** : C++17, idiomes modernes
✅ **Réutilisable** : Aucune dépendance métier

---

**Version** : 1.0.0
**Auteur** : Extrait de LaserCutStudio
**Qt Version** : 6.4+
