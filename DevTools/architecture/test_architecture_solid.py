#!/usr/bin/env python3
"""
Tests automatisés de vérification de l'architecture SOLID pour LaserCutStudio (C++/Qt).

Ces tests garantissent que l'architecture respecte les règles suivantes :
- RÈGLE 1: Pas de dépendances entre classes concrètes (Rectangle ne doit pas inclure Circle)
- RÈGLE 2: Seules les interfaces (I*) peuvent être utilisées entre classes
- RÈGLE 3: Toutes les implémentations héritent d'une interface
- RÈGLE 4: Tous les patterns sont utilisés (FactoryMixin, PropertyMixin, macros)
- RÈGLE 5: Hiérarchie des packages respectée
- RÈGLE 6: Pas de dépendances circulaires entre packages
- RÈGLE 7: Les mixins sont totalement indépendants (pas d'includes du projet)
- RÈGLE 8: Les interfaces doivent être purement abstraites
- RÈGLE 9: Toutes les classes concrètes doivent être dans des sous-packages implementation/
- RÈGLE 10: Aucune classe ne doit importer depuis les packages implementation/
- RÈGLE 11: Pas de dépendances circulaires entre fichiers individuels

Inspiré des tests Python du projet ai-front-portal-main.
"""

import re
import sys
from pathlib import Path
from typing import Set, Dict, List, Tuple

# Racine du projet
# __file__ = /home/vm-mint/Projet/LaserCutStudio/DevTools/architecture/test_architecture_solid.py
# parent = /home/vm-mint/Projet/LaserCutStudio/DevTools/architecture
# parent.parent = /home/vm-mint/Projet/LaserCutStudio/DevTools
# parent.parent.parent = /home/vm-mint/Projet/LaserCutStudio (RACINE)
PROJECT_ROOT = Path(__file__).parent.parent.parent
CORE_PATH = PROJECT_ROOT / "src" / "LaserCutStudio" / "core"


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

        # Liste des classes concrètes à détecter
        concrete_classes = ['Rectangle', 'Circle', 'Triangle', 'Part', 'TabJoint', 'FingerJoint', 'Project']

        # Vérifier les includes
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

        # Vérifier les déclarations de types concrets dans le code (.h et .cpp)
        for file_path in list(CORE_PATH.rglob("*.h")) + list(CORE_PATH.rglob("*.cpp")):
            if 'test' in str(file_path).lower() or 'moc_' in str(file_path):
                continue

            try:
                content = file_path.read_text(encoding='utf-8')
                lines = content.split('\n')

                for line_num, line in enumerate(lines, 1):
                    # Skip commentaires
                    if '//' in line:
                        line = line[:line.index('//')]

                    # Chercher des déclarations de types concrets
                    for concrete in concrete_classes:
                        # Pattern: Type* variable, Type& variable, Type variable, function(Type* param)
                        import re
                        patterns = [
                            rf'\b{concrete}\s*\*\s+\w+',  # Rectangle* var
                            rf'\b{concrete}\s*&\s+\w+',   # Rectangle& var
                            rf'\({concrete}\s*\*',        # function(Rectangle*
                            rf'\({concrete}\s*&',         # function(Rectangle&
                        ]

                        for pattern in patterns:
                            if re.search(pattern, line):
                                # Vérifier que ce n'est pas dans le fichier de définition de la classe elle-même
                                if concrete not in file_path.stem:
                                    self.log_error(
                                        f"VIOLATION RÈGLE 2: Utilisation de type concret {concrete}\n"
                                        f"   Fichier: {file_path}\n"
                                        f"   Ligne {line_num}: {line.strip()}\n"
                                        f"   INTERDIT: Déclarer des variables/paramètres avec des types concrets\n"
                                        f"   SOLUTION: Utiliser l'interface correspondante (IShape*, IPart*, IJoint*, IProject*)"
                                    )
                                    break

            except Exception as e:
                continue

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
        entre packages et sous-packages.
        """
        print("\n🔍 Test 6: Détection des dépendances circulaires entre packages...")

        # Découvrir tous les packages et sous-packages automatiquement
        all_packages = set()

        # Packages principaux
        main_packages = ['models', 'services', 'infrastructure', 'ui', 'plugins', 'di', 'patterns', 'utils']

        for main_pkg in main_packages:
            main_path = CORE_PATH / main_pkg
            if main_path.exists() and main_path.is_dir():
                all_packages.add(main_pkg)

                # Découvrir les sous-packages (shapes, parts, joints, geometry, etc.)
                for subdir in main_path.iterdir():
                    if subdir.is_dir() and not subdir.name.startswith('.') and subdir.name != 'implementation':
                        subpkg_name = f"{main_pkg}/{subdir.name}"
                        all_packages.add(subpkg_name)

        # Construire le graphe de dépendances
        dependency_graph: Dict[str, Set[str]] = {pkg: set() for pkg in all_packages}
        dependency_details: Dict[str, Dict[str, List[str]]] = {pkg: {} for pkg in all_packages}

        for package in all_packages:
            package_path = CORE_PATH / package
            if not package_path.exists():
                continue

            for file in self.analyzer.get_all_headers(package_path):
                includes = self.analyzer.get_includes(file)

                for include in includes:
                    # Vérifier les dépendances vers d'autres packages
                    for other_package in all_packages:
                        if other_package != package and other_package in include:
                            dependency_graph[package].add(other_package)

                            # Enregistrer les détails pour le rapport
                            if other_package not in dependency_details[package]:
                                dependency_details[package][other_package] = []
                            dependency_details[package][other_package].append(
                                f"{file.relative_to(CORE_PATH)} → {include}"
                            )

        # Afficher le graphe de dépendances (mode verbose)
        print(f"\n   📊 Graphe de dépendances ({len(all_packages)} packages analysés):")
        for pkg, deps in sorted(dependency_graph.items()):
            if deps:
                print(f"      {pkg} → {', '.join(sorted(deps))}")

        # Fonction pour vérifier si un package est parent d'un autre
        def is_parent_package(parent: str, child: str) -> bool:
            """Vérifie si parent est un package parent de child."""
            return child.startswith(parent + '/')

        # Détecter les cycles avec DFS amélioré
        def has_cycle(node: str, visited: Set[str], rec_stack: Set[str], path: List[str]) -> Tuple[bool, List[str]]:
            """Détection de cycles par DFS avec reconstruction du chemin complet."""
            visited.add(node)
            rec_stack.add(node)
            path.append(node)

            for neighbor in dependency_graph.get(node, set()):
                # Ignorer les dépendances entre package parent et sous-package
                # (models/ → models/constraints est OK, models/constraints → models/ est OK)
                if is_parent_package(node, neighbor) or is_parent_package(neighbor, node):
                    continue

                if neighbor not in visited:
                    has_cycle_result, cycle_path = has_cycle(neighbor, visited, rec_stack, path[:])
                    if has_cycle_result:
                        return True, cycle_path
                elif neighbor in rec_stack:
                    # Cycle détecté ! Reconstruire le chemin du cycle
                    cycle_start_idx = path.index(neighbor)
                    return True, path[cycle_start_idx:] + [neighbor]

            rec_stack.remove(node)
            return False, []

        visited = set()
        cycles_found = []

        for node in sorted(dependency_graph.keys()):
            if node not in visited:
                cycle_found, cycle_path = has_cycle(node, visited, set(), [])
                if cycle_found and cycle_path not in cycles_found:
                    cycles_found.append(cycle_path)

                    # Afficher les détails du cycle
                    cycle_str = ' → '.join(cycle_path)
                    error_msg = f"VIOLATION RÈGLE 6: Dépendance circulaire détectée !\n"
                    error_msg += f"   Cycle: {cycle_str}\n"
                    error_msg += f"   Longueur du cycle: {len(cycle_path) - 1} dépendances\n\n"

                    # Afficher les fichiers impliqués dans le cycle
                    error_msg += "   Détails des dépendances:\n"
                    for i in range(len(cycle_path) - 1):
                        from_pkg = cycle_path[i]
                        to_pkg = cycle_path[i + 1]

                        if to_pkg in dependency_details.get(from_pkg, {}):
                            files = dependency_details[from_pkg][to_pkg]
                            error_msg += f"   • {from_pkg} → {to_pkg}:\n"
                            for file_info in files[:3]:  # Limiter à 3 exemples
                                error_msg += f"      - {file_info}\n"
                            if len(files) > 3:
                                error_msg += f"      ... et {len(files) - 3} autres fichiers\n"

                    error_msg += "\n   INTERDIT: Les packages ne doivent pas avoir de dépendances circulaires\n"
                    error_msg += "   SOLUTION: Restructurer pour casser le cycle (extraire interface, inverser dépendance, ou fusionner packages)"

                    self.log_error(error_msg)

        if not any("RÈGLE 6" in err for err in self.errors):
            self.log_success(f"RÈGLE 6: Aucune dépendance circulaire détectée ({len(all_packages)} packages vérifiés)")

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

    def test_interfaces_are_pure(self):
        """
        RÈGLE 8: Les interfaces doivent être purement abstraites.

        Les interfaces (IShape, IPart, IJoint, IProject) ne doivent contenir :
        - AUCUN membre de données (m_name, m_shape, etc.)
        - SEULEMENT des méthodes virtuelles pures (= 0)
        - PAS d'implémentation inline (sauf destructeur virtuel et using declarations)

        Les données et implémentations doivent être dans les classes concrètes.
        Utiliser Q_PROPERTY + introspection au lieu de getters/setters inline.
        """
        print("\n🔍 Test 8: Vérification que les interfaces sont purement abstraites...")

        interface_files = {
            'IShape': CORE_PATH / "models" / "shapes" / "IShape.h",
            'IPart': CORE_PATH / "models" / "parts" / "IPart.h",
            'IJoint': CORE_PATH / "models" / "joints" / "IJoint.h",
            'IProject': CORE_PATH / "models" / "projects" / "IProject.h"
        }

        for interface_name, interface_file in interface_files.items():
            if not interface_file.exists():
                continue

            try:
                content = interface_file.read_text(encoding='utf-8')
                lines = content.split('\n')

                in_interface_class = False
                in_protected_section = False
                brace_depth = 0

                for line_num, line in enumerate(lines, 1):
                    stripped = line.strip()

                    # Détecter début de la classe interface
                    if f'class {interface_name}' in line and ':' in line and not stripped.startswith('//'):
                        in_interface_class = True
                        brace_depth = 0
                        continue

                    if not in_interface_class:
                        continue

                    # Compter les accolades pour détecter fin de classe
                    brace_depth += line.count('{') - line.count('}')

                    # Si on trouve la fermeture finale de la classe
                    if stripped == '};' and brace_depth <= 0:
                        in_interface_class = False
                        in_protected_section = False
                        continue

                    # Détecter section protected
                    if stripped == 'protected:':
                        in_protected_section = True
                        continue

                    # Détecter sortie de section protected (public, private, ou signals)
                    if stripped in ['public:', 'private:', 'signals:']:
                        in_protected_section = False
                        continue

                    # Dans section protected, chercher membres m_*
                    if in_protected_section:
                        # Pattern simplifié: cherche toute ligne avec "m_" suivi d'un identificateur et se terminant par ;
                        # Ignore les constructeurs/méthodes
                        if 'm_' in line and ';' in line and '(' not in line:
                            # Vérifie que c'est bien une déclaration de membre (pas un commentaire)
                            if not stripped.startswith('//') and not stripped.startswith('*') and not stripped.startswith('///'):
                                self.log_error(
                                    f"VIOLATION RÈGLE 8: {interface_name} contient un membre de données\n"
                                    f"   Fichier: {interface_file}\n"
                                    f"   Ligne {line_num}: {stripped}\n"
                                    f"   INTERDIT: Les interfaces ne doivent pas contenir de membres de données\n"
                                    f"   SOLUTION: Déplacer ce membre dans la classe concrète (Part, TabJoint, etc.)\n"
                                    f"   ALTERNATIVE: Utiliser Q_PROPERTY + getters virtuels purs"
                                )

                # Détecter les méthodes avec implémentation inline (return m_*)
                for line_num, line in enumerate(lines, 1):
                    stripped = line.strip()

                    # Skip commentaires et using declarations
                    if stripped.startswith('//') or stripped.startswith('*') or 'using ' in line or 'virtual ~' in line:
                        continue

                    # Chercher pattern: method() { return m_* }
                    if 'return m_' in line and '{' in line and '}' in line:
                        self.log_error(
                            f"VIOLATION RÈGLE 8: {interface_name} contient une méthode avec implémentation inline\n"
                            f"   Fichier: {interface_file}\n"
                            f"   Ligne {line_num}: {stripped}\n"
                            f"   INTERDIT: Les interfaces ne doivent pas contenir d'implémentation\n"
                            f"   SOLUTION: Rendre la méthode virtuelle pure (= 0) ou la déplacer dans la classe concrète\n"
                            f"   ALTERNATIVE: Utiliser Q_PROPERTY pour l'introspection"
                        )

            except Exception as e:
                print(f"⚠️  Erreur analyse {interface_file}: {e}")

        if not any("RÈGLE 8" in err for err in self.errors):
            self.log_success("RÈGLE 8: Toutes les interfaces sont purement abstraites")

    def test_concrete_classes_in_implementation(self):
        """
        RÈGLE 9: Toutes les classes concrètes doivent être dans des sous-packages implementation/.

        Les interfaces (I*) restent dans le package parent.
        Les implémentations concrètes doivent être dans implementation/.
        """
        print("\n🔍 RÈGLE 9: Vérification de l'organisation des classes concrètes...")

        # Parcourir tous les headers dans core/models/
        models_path = CORE_PATH / "models"
        if not models_path.exists():
            print(f"⚠️  Répertoire models/ introuvable: {models_path}")
            return

        for header_file in models_path.rglob("*.h"):
            # Ignorer les fichiers dans patterns/, base/, utils/
            relative_path = header_file.relative_to(CORE_PATH)
            path_parts = relative_path.parts

            if any(x in path_parts for x in ['patterns', 'base', 'infrastructure']):
                continue

            # Vérifier si c'est une interface
            class_name = header_file.stem
            is_interface = self.analyzer.is_interface(class_name)

            # Vérifier si le fichier est dans un sous-dossier implementation/
            is_in_implementation = 'implementation' in path_parts

            # VIOLATION: Classe concrète HORS de implementation/
            if not is_interface and not is_in_implementation:
                # Exceptions: types de base, contraintes sont OK
                if class_name in ['Point2D', 'Point3D', 'Material', 'JointType']:
                    continue
                if 'Constraint' in class_name or class_name in ['ConstraintSolver', 'ConstraintSketch']:
                    continue

                self.log_error(
                    f"VIOLATION RÈGLE 9: Classe concrète hors du package implementation/\n"
                    f"   Fichier: {header_file.relative_to(PROJECT_ROOT)}\n"
                    f"   Classe: {class_name}\n"
                    f"   PROBLÈME: Les classes concrètes doivent être dans implementation/\n"
                    f"   SOLUTION: Déplacer dans {header_file.parent}/implementation/{header_file.name}\n"
                    f"   ARCHITECTURE: Seules les interfaces (I*) restent dans le package parent"
                )

            # VIOLATION: Interface DANS implementation/
            elif is_interface and is_in_implementation:
                self.log_error(
                    f"VIOLATION RÈGLE 9: Interface dans le package implementation/\n"
                    f"   Fichier: {header_file.relative_to(PROJECT_ROOT)}\n"
                    f"   Interface: {class_name}\n"
                    f"   PROBLÈME: Les interfaces doivent rester dans le package parent\n"
                    f"   SOLUTION: Déplacer dans {header_file.parent.parent}/{header_file.name}\n"
                    f"   ARCHITECTURE: implementation/ est réservé aux classes concrètes"
                )

        if not any("RÈGLE 9" in err for err in self.errors):
            self.log_success("RÈGLE 9: Toutes les classes concrètes sont dans implementation/")

    def test_no_imports_from_implementation(self):
        """
        RÈGLE 10: Aucune classe ne doit importer depuis les packages implementation/.

        Les classes doivent dépendre des interfaces (dans le package parent), jamais des
        implémentations concrètes (dans implementation/).

        Exceptions:
        - Un fichier dans implementation/ peut inclure d'autres fichiers du MÊME package implementation/
        - Les fichiers de test peuvent inclure les implémentations pour les tester
        """
        print("\n🔍 Test 10: Vérification qu'aucune classe n'importe depuis implementation/...")

        # Parcourir tous les fichiers sources (header et cpp)
        for file_path in list(CORE_PATH.rglob("*.h")) + list(CORE_PATH.rglob("*.cpp")):
            # Ignorer les fichiers de test et générés
            if 'test' in str(file_path).lower() or 'moc_' in str(file_path) or 'qrc_' in str(file_path):
                continue

            # Déterminer si le fichier est lui-même dans implementation/
            relative_path = file_path.relative_to(CORE_PATH)
            path_parts = relative_path.parts
            file_is_in_implementation = 'implementation' in path_parts

            # Extraire les includes
            includes = self.analyzer.get_includes(file_path)

            for include in includes:
                # Vérifier si l'include pointe vers implementation/ ou implementations/
                if 'implementation' in include.lower():
                    # Exception : Si le fichier est lui-même dans implementation/, il peut inclure
                    # d'autres fichiers du MÊME package implementation/
                    if file_is_in_implementation:
                        # Vérifier que c'est bien le même package
                        # Exemple : geometry/implementation/GeometricPoint.h peut inclure geometry/implementation/GeometricSegment.h
                        # mais PAS shapes/implementation/Rectangle.h

                        # Extraire le package parent de l'include
                        include_parts = Path(include).parts
                        if len(include_parts) >= 2:
                            # Trouver l'index de 'implementation' dans l'include
                            try:
                                impl_index = include_parts.index('implementation')
                                if impl_index > 0:
                                    include_package = include_parts[impl_index - 1]
                                else:
                                    include_package = None
                            except ValueError:
                                include_package = None

                            # Extraire le package parent du fichier
                            try:
                                file_impl_index = path_parts.index('implementation')
                                if file_impl_index > 0:
                                    file_package = path_parts[file_impl_index - 1]
                                else:
                                    file_package = None
                            except ValueError:
                                file_package = None

                            # Si les packages sont différents, c'est une violation
                            if include_package and file_package and include_package != file_package:
                                self.log_error(
                                    f"VIOLATION RÈGLE 10: Import depuis implementation/ d'un autre package\n"
                                    f"   Fichier: {file_path.relative_to(PROJECT_ROOT)}\n"
                                    f"   Package du fichier: {file_package}/\n"
                                    f"   Include: {include}\n"
                                    f"   Package de l'include: {include_package}/\n"
                                    f"   INTERDIT: Importer depuis implementation/ d'un autre package\n"
                                    f"   SOLUTION: Importer l'interface depuis {include_package}/ (ex: I*.h)\n"
                                    f"   PRINCIPE: Dépendre des abstractions, pas des implémentations"
                                )
                        continue

                    # Si le fichier n'est PAS dans implementation/, c'est une violation claire
                    self.log_error(
                        f"VIOLATION RÈGLE 10: Import depuis un package implementation/\n"
                        f"   Fichier: {file_path.relative_to(PROJECT_ROOT)}\n"
                        f"   Include interdit: {include}\n"
                        f"   INTERDIT: Importer depuis implementation/ (classes concrètes)\n"
                        f"   SOLUTION: Importer l'interface correspondante depuis le package parent (I*.h)\n"
                        f"   EXEMPLE: Au lieu de 'shapes/implementation/Rectangle.h', utiliser 'shapes/IShape.h'\n"
                        f"   PRINCIPE: Dependency Inversion Principle (DIP) - Dépendre des abstractions"
                    )

        if not any("RÈGLE 10" in err for err in self.errors):
            self.log_success("RÈGLE 10: Aucun import depuis implementation/ détecté")

    def test_no_circular_file_dependencies(self):
        """
        RÈGLE 11: Pas de dépendances circulaires entre fichiers individuels.

        Détecte les includes mutuels entre fichiers (A.h inclut B.h, B.h inclut A.h).
        Utilise un algorithme DFS pour trouver tous les cycles dans le graphe d'includes.
        """
        print("\n🔍 Test 11: Détection des dépendances circulaires entre fichiers...")

        # Construire le graphe d'includes entre tous les fichiers
        file_graph: Dict[Path, Set[Path]] = {}
        all_files = list(CORE_PATH.rglob("*.h"))

        # Ignorer les fichiers de test et générés
        all_files = [f for f in all_files if 'test' not in str(f).lower() and 'moc_' not in str(f)]

        print(f"   📁 Analyse de {len(all_files)} fichiers headers...")

        # Créer un mapping nom de fichier → chemin complet pour résolution
        file_name_to_path: Dict[str, List[Path]] = {}
        for file_path in all_files:
            file_name = file_path.name
            if file_name not in file_name_to_path:
                file_name_to_path[file_name] = []
            file_name_to_path[file_name].append(file_path)

        # Construire le graphe
        for file_path in all_files:
            includes = self.analyzer.get_includes(file_path)
            file_graph[file_path] = set()

            for include in includes:
                # Résoudre l'include vers un chemin de fichier
                include_name = Path(include).name

                # Chercher le fichier inclus
                if include_name in file_name_to_path:
                    for candidate in file_name_to_path[include_name]:
                        # Vérifier que c'est bien un fichier du projet (pas Qt ou système)
                        if candidate in all_files:
                            file_graph[file_path].add(candidate)

        # Détecter les cycles avec DFS
        def find_cycles_from_node(start: Path, current: Path, visited: Set[Path], path: List[Path]) -> List[List[Path]]:
            """Trouve tous les cycles partant d'un nœud."""
            cycles = []
            visited.add(current)
            path.append(current)

            for neighbor in file_graph.get(current, set()):
                if neighbor == start and len(path) > 1:
                    # Cycle trouvé !
                    cycles.append(path[:] + [neighbor])
                elif neighbor not in visited:
                    cycles.extend(find_cycles_from_node(start, neighbor, visited, path[:]))

            return cycles

        # Chercher tous les cycles
        all_cycles = []
        visited_starts = set()

        for start_node in sorted(file_graph.keys(), key=lambda p: str(p)):
            if start_node not in visited_starts:
                cycles = find_cycles_from_node(start_node, start_node, set(), [])
                for cycle in cycles:
                    # Normaliser le cycle (toujours commencer par le plus petit élément)
                    min_idx = cycle.index(min(cycle[:-1], key=lambda p: str(p)))
                    normalized = cycle[min_idx:-1] + cycle[:min_idx] + [cycle[min_idx]]

                    # Vérifier si ce cycle n'a pas déjà été trouvé
                    if normalized not in all_cycles:
                        all_cycles.append(normalized)
                        visited_starts.update(cycle[:-1])

        # Rapporter les cycles
        if all_cycles:
            print(f"   ⚠️  {len(all_cycles)} cycle(s) détecté(s) !\n")

            for idx, cycle in enumerate(all_cycles, 1):
                cycle_str = ' → '.join([f.name for f in cycle])
                error_msg = f"VIOLATION RÈGLE 11: Dépendance circulaire entre fichiers (cycle #{idx})\n"
                error_msg += f"   Cycle: {cycle_str}\n"
                error_msg += f"   Longueur: {len(cycle) - 1} fichiers\n\n"

                error_msg += "   Fichiers impliqués:\n"
                for i in range(len(cycle) - 1):
                    from_file = cycle[i]
                    to_file = cycle[i + 1]
                    error_msg += f"   • {from_file.relative_to(CORE_PATH)}\n"
                    error_msg += f"     ↓ inclut\n"
                    error_msg += f"     {to_file.relative_to(CORE_PATH)}\n"

                error_msg += "\n   INTERDIT: Les fichiers ne doivent pas s'inclure mutuellement\n"
                error_msg += "   SOLUTION:\n"
                error_msg += "   1. Utiliser des forward declarations (class ClassName;)\n"
                error_msg += "   2. Déplacer les includes dans le fichier .cpp\n"
                error_msg += "   3. Extraire une interface commune\n"
                error_msg += "   4. Restructurer pour casser la dépendance circulaire"

                self.log_error(error_msg)
        else:
            print(f"   ✓ Aucun cycle détecté\n")

        if not any("RÈGLE 11" in err for err in self.errors):
            self.log_success(f"RÈGLE 11: Aucune dépendance circulaire entre fichiers détectée ({len(all_files)} fichiers vérifiés)")

    def test_one_interface_per_package(self):
        """
        RÈGLE 12: Un seul fichier interface (I*.h) par package.

        Chaque package ne doit contenir qu'une seule interface publique.
        Les sous-interfaces doivent être dans des sous-packages dédiés.
        """
        print("\n🔍 Test 12: Vérification d'une seule interface par package...")

        # Parcourir tous les packages dans core/
        for package_dir in CORE_PATH.rglob("*"):
            if not package_dir.is_dir():
                continue

            # Skip les répertoires implementation, tests, generated
            if any(skip in str(package_dir) for skip in ['implementation', 'test', 'generated', '.git', 'build']):
                continue

            # Compter les fichiers interface (I*.h) directement dans ce répertoire
            interface_files = [f for f in package_dir.glob("I*.h") if f.is_file()]

            if len(interface_files) > 1:
                package_name = package_dir.relative_to(CORE_PATH)
                interface_names = [f.name for f in interface_files]

                error_msg = (
                    f"❌ VIOLATION RÈGLE 12: Plusieurs interfaces dans le même package\n"
                    f"   Package: {package_name}\n"
                    f"   Interfaces trouvées: {', '.join(interface_names)}\n"
                    f"   INTERDIT: Avoir plusieurs interfaces (I*.h) dans le même package\n"
                    f"   SOLUTION: Créer des sous-packages dédiés pour chaque interface\n"
                    f"   EXEMPLE: Au lieu de geometry/IPoint.h + geometry/ISegment.h\n"
                    f"           Utiliser geometry/point/IPoint.h + geometry/segment/ISegment.h\n"
                    f"   PRINCIPE: Interface Segregation Principle (ISP) - Séparation des responsabilités\n"
                )
                self.errors.append(error_msg)
                print(error_msg)

        if not any("RÈGLE 12" in err for err in self.errors):
            self.log_success("RÈGLE 12: Une seule interface par package")

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
        self.test_interfaces_are_pure()
        self.test_concrete_classes_in_implementation()
        self.test_no_imports_from_implementation()
        self.test_no_circular_file_dependencies()
        self.test_one_interface_per_package()

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
