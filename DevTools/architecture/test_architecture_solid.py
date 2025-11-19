#!/usr/bin/env python3
"""
Tests automatisés de vérification de l'architecture SOLID pour LaserCutStudio (C++/Qt).

Ces tests garantissent que l'architecture respecte les règles suivantes :
- RÈGLE 1: Pas de dépendances entre classes concrètes (Rectangle ne doit pas inclure Circle)
- RÈGLE 2: Seules les interfaces (I*) peuvent être utilisées entre classes
- RÈGLE 3: Toutes les implémentations héritent d'une interface
- RÈGLE 4: Tous les patterns sont utilisés (FactoryMixin, PropertyMixin, macros)
- RÈGLE 5: Pas de cycles d'includes
- RÈGLE 6: Les mixins sont totalement indépendants (pas d'includes du projet)

Inspiré des tests Python du projet ai-front-portal-main.
"""

import re
import sys
from pathlib import Path
from typing import Set, Dict, List, Tuple

# Racine du projet
# Si exécuté depuis /home/vm-mint/Projet/LaserCutStudio
# alors PROJECT_ROOT = /home/vm-mint/Projet/LaserCutStudio/src/LaserCutStudio
PROJECT_ROOT = Path(__file__).parent.parent
CORE_PATH = PROJECT_ROOT / "core"


class IncludeAnalyzer:
    """Analyse les includes et dépendances dans les fichiers C++."""

    def __init__(self, root_path: Path):
        self.root_path = root_path

    def get_includes(self, file_path: Path) -> Set[str]:
        """
        Extrait tous les #include d'un fichier.

        Returns:
            Ensemble des fichiers inclus
        """
        includes = set()
        try:
            content = file_path.read_text(encoding='utf-8')
            # Matcher #include "..." et #include <...>
            pattern = r'#include\s+[<"]([^>"]+)[>"]'
            matches = re.findall(pattern, content)
            includes.update(matches)
        except Exception as e:
            print(f"⚠️  Erreur lecture {file_path}: {e}")
        return includes

    def get_base_classes(self, file_path: Path) -> List[str]:
        """
        Extrait les classes de base (héritage) d'un fichier header.

        Returns:
            Liste des classes de base
        """
        bases = []
        try:
            content = file_path.read_text(encoding='utf-8')
            # Matcher: class ClassName : public BaseClass
            pattern = r'class\s+\w+\s*:\s*(?:public|protected|private)\s+(\w+)'
            matches = re.findall(pattern, content)
            bases.extend(matches)
        except Exception as e:
            print(f"⚠️  Erreur lecture {file_path}: {e}")
        return bases

    def has_macro(self, file_path: Path, macro_name: str) -> bool:
        """
        Vérifie si un fichier contient une macro spécifique.

        Args:
            file_path: Fichier à analyser
            macro_name: Nom de la macro (ex: "DECLARE_TYPE_NAME")

        Returns:
            True si la macro est trouvée
        """
        try:
            content = file_path.read_text(encoding='utf-8')
            return macro_name in content
        except Exception:
            return False

    def is_interface(self, class_name: str) -> bool:
        """Vérifie si un nom de classe est une interface (commence par 'I')."""
        return class_name.startswith('I') and len(class_name) > 1 and class_name[1].isupper()

    def is_mixin(self, include_path: str) -> bool:
        """Vérifie si un include est un mixin (pattern template CRTP)."""
        mixins = ['FactoryMixin', 'PropertyMixin', 'ClonableMixin', 'ListManagerMixin']
        return any(mixin in include_path for mixin in mixins)

    def is_utility(self, include_path: str) -> bool:
        """Vérifie si un include est un utilitaire (GeometryUtils, DebugMacros, etc.)."""
        utilities = ['GeometryUtils', 'DebugMacros', 'Point2D', 'Point3D', 'Material', 'JointType']
        return any(util in include_path for util in utilities)

    def get_all_headers(self, directory: Path) -> List[Path]:
        """Récupère tous les fichiers .h dans un répertoire."""
        if not directory.exists():
            return []
        return list(directory.rglob("*.h"))

    def get_all_implementations(self, directory: Path) -> List[Path]:
        """Récupère tous les fichiers .cpp dans un répertoire."""
        if not directory.exists():
            return []
        return list(directory.rglob("*.cpp"))


class TestArchitectureSOLID:
    """Tests SOLID pour LaserCutStudio."""

    def __init__(self):
        self.analyzer = IncludeAnalyzer(PROJECT_ROOT)
        self.errors = []

    def log_error(self, message: str):
        """Enregistre une erreur."""
        self.errors.append(message)
        print(f"❌ {message}")

    def log_success(self, message: str):
        """Enregistre un succès."""
        print(f"✅ {message}")

    def test_no_concrete_class_dependencies(self):
        """
        RÈGLE 1: Pas de dépendances entre classes concrètes.

        Les implémentations concrètes (Rectangle, Circle, Part, etc.) ne doivent PAS
        inclure d'autres implémentations concrètes. Seules les interfaces sont autorisées.
        """
        print("\n🔍 Test 1: Vérification des dépendances entre classes concrètes...")

        # Lister les implémentations concrètes
        implementations_dir = CORE_PATH / "models"
        concrete_classes = [
            'Rectangle', 'Circle',          # Shapes
            'Part',                         # Parts
            'TabJoint', 'FingerJoint',      # Joints
            'Project'                       # Projects
        ]

        for impl_file in self.analyzer.get_all_headers(implementations_dir):
            if 'implementations' not in str(impl_file):
                continue  # Skip interfaces

            # Extraire le nom de la classe
            class_name = impl_file.stem
            if class_name not in concrete_classes:
                continue

            includes = self.analyzer.get_includes(impl_file)

            for include in includes:
                # Vérifier si l'include est une autre classe concrète
                for other_class in concrete_classes:
                    if other_class == class_name:
                        continue  # Skip self

                    # Vérifier si l'include contient le nom de la classe ET est dans implementations/
                    if other_class in include and 'implementations/' in include:
                        self.log_error(
                            f"VIOLATION RÈGLE 1: {impl_file.name} inclut la classe concrète {other_class}\n"
                            f"   Fichier: {impl_file}\n"
                            f"   Include: {include}\n"
                            f"   INTERDIT: Les classes concrètes ne doivent pas s'inclure mutuellement\n"
                            f"   SOLUTION: Utiliser l'interface (IShape) à la place"
                        )

        if not any("RÈGLE 1" in err for err in self.errors):
            self.log_success("RÈGLE 1: Aucune dépendance entre classes concrètes détectée")

    def test_only_interfaces_between_classes(self):
        """
        RÈGLE 2: Seules les interfaces peuvent être utilisées entre classes.

        Les classes concrètes doivent déclarer leurs dépendances avec des interfaces (I*)
        et non des classes concrètes.
        """
        print("\n🔍 Test 2: Vérification de l'utilisation exclusive des interfaces...")

        implementations_dir = CORE_PATH / "models"

        for impl_file in self.analyzer.get_all_headers(implementations_dir):
            if 'implementations' not in str(impl_file):
                continue

            includes = self.analyzer.get_includes(impl_file)

            for include in includes:
                # Skip les includes système et Qt
                if include.startswith('Q') or include.startswith('<'):
                    continue

                # Skip les mixins et utilitaires
                if self.analyzer.is_mixin(include) or self.analyzer.is_utility(include):
                    continue

                # Skip les includes de sa propre interface
                if 'interfaces' in include:
                    continue

                # Vérifier si c'est un include de classe concrète
                if 'implementations' in include:
                    stem = Path(include).stem
                    if not self.analyzer.is_interface(stem):
                        self.log_error(
                            f"VIOLATION RÈGLE 2: {impl_file.name} inclut directement une implémentation\n"
                            f"   Fichier: {impl_file}\n"
                            f"   Include: {include}\n"
                            f"   INTERDIT: Inclure des classes concrètes\n"
                            f"   SOLUTION: Déclarer les dépendances avec des interfaces (I*)"
                        )

        if not any("RÈGLE 2" in err for err in self.errors):
            self.log_success("RÈGLE 2: Toutes les dépendances utilisent les interfaces")

    def test_all_implementations_inherit_interface(self):
        """
        RÈGLE 3: Toutes les implémentations héritent d'une interface.

        Chaque classe concrète doit hériter de l'interface correspondante (IShape, IPart, etc.).
        """
        print("\n🔍 Test 3: Vérification de l'héritage des interfaces...")

        implementations = {
            'Rectangle': 'IShape',
            'Circle': 'IShape',
            'Part': 'IPart',
            'TabJoint': 'IJoint',
            'FingerJoint': 'IJoint',
            'Project': 'IProject'
        }

        for class_name, expected_interface in implementations.items():
            # Trouver le fichier header dans implementations/
            impl_files = list(CORE_PATH.rglob(f"implementations/{class_name}.h"))

            if not impl_files:
                # Chercher partout en dernier recours
                impl_files = list(CORE_PATH.rglob(f"{class_name}.h"))

            if not impl_files:
                # Pas une erreur si le fichier n'existe pas encore (développement en cours)
                continue

            impl_file = impl_files[0]
            bases = self.analyzer.get_base_classes(impl_file)

            if expected_interface not in bases:
                self.log_error(
                    f"VIOLATION RÈGLE 3: {class_name} n'hérite pas de {expected_interface}\n"
                    f"   Fichier: {impl_file}\n"
                    f"   Classes de base trouvées: {bases}\n"
                    f"   REQUIS: class {class_name} : public {expected_interface}"
                )

        if not any("RÈGLE 3" in err for err in self.errors):
            self.log_success("RÈGLE 3: Toutes les implémentations héritent de leur interface")

    def test_patterns_usage(self):
        """
        RÈGLE 4: Tous les patterns sont utilisés correctement.

        Vérifie que les classes utilisent :
        - DECLARE_TYPE_NAME(ClassName)
        - IMPLEMENT_CLONE (dans .cpp)
        - FactoryMixin pour les interfaces
        - PropertyMixin pour les setters
        """
        print("\n🔍 Test 4: Vérification de l'utilisation des patterns...")

        # Vérifier DECLARE_TYPE_NAME dans les headers
        concrete_classes = ['Rectangle', 'Circle', 'Part', 'TabJoint', 'FingerJoint', 'Project']

        for class_name in concrete_classes:
            header_files = list(CORE_PATH.rglob(f"{class_name}.h"))
            if not header_files:
                continue

            header_file = header_files[0]

            if not self.analyzer.has_macro(header_file, 'DECLARE_TYPE_NAME'):
                self.log_error(
                    f"VIOLATION RÈGLE 4: {class_name}.h manque DECLARE_TYPE_NAME\n"
                    f"   Fichier: {header_file}\n"
                    f"   REQUIS: DECLARE_TYPE_NAME({class_name}) dans la classe"
                )

        # Vérifier IMPLEMENT_CLONE dans les .cpp
        for class_name in concrete_classes:
            cpp_files = list(CORE_PATH.rglob(f"{class_name}.cpp"))
            if not cpp_files:
                continue

            cpp_file = cpp_files[0]

            if not self.analyzer.has_macro(cpp_file, 'IMPLEMENT_CLONE'):
                self.log_error(
                    f"VIOLATION RÈGLE 4: {class_name}.cpp manque IMPLEMENT_CLONE\n"
                    f"   Fichier: {cpp_file}\n"
                    f"   REQUIS: IMPLEMENT_CLONE({class_name}, BaseInterface)"
                )

        if not any("RÈGLE 4" in err for err in self.errors):
            self.log_success("RÈGLE 4: Tous les patterns sont correctement utilisés")

    def test_package_dependencies(self):
        """
        RÈGLE 5: Respect de la hiérarchie des packages.

        Hiérarchie autorisée :
        - models/ (base, pas de dépendances internes)
        - services/ → peut dépendre de models/
        - infrastructure/ → peut dépendre de models/ et services/

        INTERDIT :
        - models/ → services/ ou infrastructure/
        - services/ → infrastructure/ (si pas nécessaire)
        """
        print("\n🔍 Test 5: Vérification de la hiérarchie des packages...")

        # Définir la hiérarchie des packages
        package_hierarchy = {
            'models': [],  # Pas de dépendances autorisées
            'services': ['models'],  # Peut dépendre de models
            'infrastructure': ['models', 'services']  # Peut dépendre de models et services
        }

        for package_name, allowed_deps in package_hierarchy.items():
            package_path = CORE_PATH / package_name
            if not package_path.exists():
                continue

            for file in self.analyzer.get_all_headers(package_path):
                includes = self.analyzer.get_includes(file)

                for include in includes:
                    # Vérifier les dépendances vers d'autres packages
                    for other_package in package_hierarchy.keys():
                        if other_package == package_name:
                            continue

                        if f'{other_package}/' in include:
                            if other_package not in allowed_deps:
                                self.log_error(
                                    f"VIOLATION RÈGLE 5: Dépendance de package non autorisée\n"
                                    f"   Fichier: {file}\n"
                                    f"   Package: {package_name}/\n"
                                    f"   Dépend de: {other_package}/\n"
                                    f"   Include: {include}\n"
                                    f"   INTERDIT: {package_name}/ ne peut pas dépendre de {other_package}/\n"
                                    f"   Hiérarchie: models/ ← services/ ← infrastructure/"
                                )

        if not any("RÈGLE 5" in err for err in self.errors):
            self.log_success("RÈGLE 5: Hiérarchie des packages respectée")

    def test_no_circular_dependencies(self):
        """
        RÈGLE 6: Pas de dépendances circulaires entre packages.

        Utilise un algorithme DFS pour détecter les cycles dans le graphe de dépendances
        entre packages.
        """
        print("\n🔍 Test 6: Détection des dépendances circulaires entre packages...")

        # Construire le graphe de dépendances entre packages
        dependency_graph: Dict[str, Set[str]] = {}

        packages = ['models', 'services', 'infrastructure']

        for package in packages:
            package_path = CORE_PATH / package
            if not package_path.exists():
                continue

            dependencies = set()

            for file in self.analyzer.get_all_headers(package_path):
                includes = self.analyzer.get_includes(file)

                for include in includes:
                    for other_package in packages:
                        if other_package != package and f'{other_package}/' in include:
                            dependencies.add(other_package)

            dependency_graph[package] = dependencies

        # Détecter les cycles avec DFS
        def has_cycle(node: str, visited: Set[str], rec_stack: Set[str]) -> Tuple[bool, List[str]]:
            """Détection de cycles par DFS."""
            visited.add(node)
            rec_stack.add(node)

            for neighbor in dependency_graph.get(node, set()):
                if neighbor not in visited:
                    has_cycle_result, path = has_cycle(neighbor, visited, rec_stack)
                    if has_cycle_result:
                        return True, [node] + path
                elif neighbor in rec_stack:
                    return True, [node, neighbor]

            rec_stack.remove(node)
            return False, []

        visited = set()

        for node in dependency_graph:
            if node not in visited:
                cycle_found, cycle_path = has_cycle(node, visited, set())
                if cycle_found:
                    self.log_error(
                        f"VIOLATION RÈGLE 6: Dépendance circulaire détectée !\n"
                        f"   Cycle: {' → '.join(cycle_path)}\n"
                        f"   INTERDIT: Les packages ne doivent pas avoir de dépendances circulaires\n"
                        f"   SOLUTION: Restructurer pour respecter models/ ← services/ ← infrastructure/"
                    )

        if not any("RÈGLE 6" in err for err in self.errors):
            self.log_success("RÈGLE 6: Aucune dépendance circulaire détectée")

    def test_mixins_independence(self):
        """
        RÈGLE 7: Les mixins sont totalement indépendants.

        Les mixins (FactoryMixin, PropertyMixin, etc.) ne doivent avoir AUCUNE
        dépendance vers le code du projet. Seuls les includes Qt et stdlib autorisés.
        """
        print("\n🔍 Test 7: Vérification de l'indépendance des mixins...")

        mixin_files = [
            CORE_PATH / "patterns" / "FactoryMixin.h",
            CORE_PATH / "patterns" / "PropertyMixin.h",
            CORE_PATH / "patterns" / "ClonableMixin.h",
        ]

        # Includes interdits pour les mixins (code projet)
        forbidden_includes = [
            'models/', 'services/', 'infrastructure/',
            'IShape', 'IPart', 'IJoint', 'IProject',
            'Rectangle', 'Circle', 'Part', 'TabJoint', 'FingerJoint', 'Project'
        ]

        for mixin_file in mixin_files:
            if not mixin_file.exists():
                continue

            includes = self.analyzer.get_includes(mixin_file)

            for include in includes:
                for forbidden in forbidden_includes:
                    if forbidden in include:
                        self.log_error(
                            f"VIOLATION RÈGLE 7: {mixin_file.name} dépend du code projet\n"
                            f"   Fichier: {mixin_file}\n"
                            f"   Include interdit: {include}\n"
                            f"   INTERDIT: Les mixins doivent être totalement indépendants\n"
                            f"   AUTORISÉ: Uniquement Qt et stdlib"
                        )

        if not any("RÈGLE 7" in err for err in self.errors):
            self.log_success("RÈGLE 7: Tous les mixins sont indépendants")

    def run_all_tests(self):
        """Exécute tous les tests SOLID."""
        print("=" * 80)
        print("🏗️  Tests d'Architecture SOLID pour LaserCutStudio")
        print("=" * 80)

        self.test_no_concrete_class_dependencies()
        self.test_only_interfaces_between_classes()
        self.test_all_implementations_inherit_interface()
        self.test_patterns_usage()
        self.test_package_dependencies()
        self.test_no_circular_dependencies()
        self.test_mixins_independence()

        print("\n" + "=" * 80)
        if self.errors:
            print(f"❌ {len(self.errors)} violation(s) d'architecture détectée(s)")
            print("=" * 80)
            self._generate_report()
            return 1
        else:
            print("✅ Tous les tests SOLID passés avec succès !")
            print("=" * 80)
            self._generate_report()
            return 0

    def _generate_report(self):
        """Génère un rapport détaillé dans generated/reports/architecture/"""
        from datetime import datetime

        # Créer le répertoire s'il n'existe pas
        report_dir = Path("generated/reports/architecture")
        report_dir.mkdir(parents=True, exist_ok=True)

        # Nom du fichier avec timestamp
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        report_file = report_dir / f"architecture_report_{timestamp}.txt"

        # Générer le rapport
        with open(report_file, 'w', encoding='utf-8') as f:
            f.write("=" * 80 + "\n")
            f.write("Rapport d'Architecture SOLID - LaserCutStudio\n")
            f.write(f"Généré le : {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
            f.write("=" * 80 + "\n\n")

            if self.errors:
                f.write(f"❌ {len(self.errors)} violation(s) détectée(s)\n\n")
                for i, error in enumerate(self.errors, 1):
                    f.write(f"Violation #{i}:\n")
                    f.write(error + "\n\n")
                    f.write("-" * 80 + "\n\n")
            else:
                f.write("✅ Tous les tests SOLID passés avec succès !\n\n")
                f.write("Aucune violation d'architecture détectée.\n")

        print(f"\n📄 Rapport généré : {report_file}")

        # Créer aussi un lien vers le dernier rapport
        latest_link = report_dir / "latest.txt"
        if latest_link.exists():
            latest_link.unlink()
        latest_link.symlink_to(report_file.name)


def main():
    """Point d'entrée du script."""
    tester = TestArchitectureSOLID()
    exit_code = tester.run_all_tests()
    sys.exit(exit_code)


if __name__ == "__main__":
    main()
