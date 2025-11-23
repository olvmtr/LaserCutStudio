#!/usr/bin/env python3
"""
Test architectural générique : Détection des violations du Factory Pattern

Ce script vérifie que toutes les classes concrètes sont créées via le Factory Pattern
(IInterface::create()) et non directement avec 'new ConcreteClass()'.

Principe:
1. Découvre toutes les interfaces (I*.h)
2. Pour chaque interface, trouve les classes concrètes enregistrées
3. Recherche les instantiations directes 'new ConcreteClass()' dans le code
4. Signale les violations

Usage:
    python3 check_factory_violations.py [--fix]
"""

import os
import re
import sys
from pathlib import Path
from typing import Dict, List, Set, Tuple

# Configuration
PROJECT_ROOT = Path(__file__).parent.parent.parent / "src" / "LaserCutStudio"
CORE_DIR = PROJECT_ROOT / "core"
EXCLUDE_PATTERNS = [
    "tests/",
    "test_",
    ".cpp~",
    ".h~",
    "moc_",
    "qrc_"
]


class FactoryViolationChecker:
    """Détecteur de violations du Factory Pattern"""

    def __init__(self):
        self.interfaces: Dict[str, Path] = {}  # nom interface → fichier .h
        self.concrete_classes: Dict[str, List[str]] = {}  # interface → [classes concrètes]
        self.violations: List[Tuple[str, str, int, str]] = []  # (file, class, line, code)

    def discover_interfaces(self) -> None:
        """Découvre toutes les interfaces (I*.h)"""
        print("🔍 Découverte des interfaces...")

        for h_file in CORE_DIR.rglob("I*.h"):
            if any(pattern in str(h_file) for pattern in EXCLUDE_PATTERNS):
                continue

            interface_name = h_file.stem  # IShape, IPart, etc.
            self.interfaces[interface_name] = h_file

        print(f"   Trouvé {len(self.interfaces)} interfaces: {', '.join(sorted(self.interfaces.keys()))}")

    def discover_concrete_classes(self) -> None:
        """Pour chaque interface, découvre les classes concrètes enregistrées"""
        print("\n🔍 Découverte des classes concrètes...")

        for interface_name, interface_file in self.interfaces.items():
            concrete_classes = set()

            # Chercher dans le même dossier et sous-dossiers
            interface_dir = interface_file.parent

            for cpp_file in interface_dir.rglob("*.cpp"):
                if any(pattern in str(cpp_file) for pattern in EXCLUDE_PATTERNS):
                    continue

                content = cpp_file.read_text(encoding='utf-8', errors='ignore')

                # Chercher les auto-enregistrements
                # Pattern 1: AutoRegister<ClassName>
                matches = re.findall(rf'AutoRegister<(\w+)>\s+\w+;', content)
                concrete_classes.update(matches)

                # Pattern 2: registerFactory<ClassName>()
                matches = re.findall(rf'registerFactory<(\w+)>\s*\(\)', content)
                concrete_classes.update(matches)

            self.concrete_classes[interface_name] = sorted(concrete_classes)

            if concrete_classes:
                print(f"   {interface_name}: {len(concrete_classes)} classes → {', '.join(sorted(concrete_classes))}")
            else:
                print(f"   {interface_name}: ⚠️  AUCUNE classe enregistrée")

    def check_violations(self) -> None:
        """Recherche les instantiations directes au lieu du Factory Pattern"""
        print("\n🔍 Recherche des violations...")

        total_classes_checked = 0

        for interface_name, concrete_classes in self.concrete_classes.items():
            if not concrete_classes:
                continue  # Pas de classes = pas de violations possibles

            total_classes_checked += len(concrete_classes)

            # Pour chaque classe concrète, chercher 'new ClassName('
            for class_name in concrete_classes:
                pattern = rf'\bnew\s+(?:\w+::)*{class_name}\s*\('

                # Chercher dans tous les fichiers .cpp et .h (sauf tests)
                for source_file in CORE_DIR.rglob("*.[ch]pp"):
                    if any(excl in str(source_file) for excl in EXCLUDE_PATTERNS):
                        continue

                    try:
                        content = source_file.read_text(encoding='utf-8', errors='ignore')
                        lines = content.split('\n')

                        for line_num, line in enumerate(lines, 1):
                            # Ignorer les commentaires
                            if '//' in line:
                                code_part = line.split('//')[0]
                            else:
                                code_part = line

                            if re.search(pattern, code_part):
                                # Exceptions légitimes (ne pas signaler)

                                # 1. Méthode clone() - Pattern Prototype (légitime)
                                if 'clone()' in content[max(0, content.find(line)-200):content.find(line)+200]:
                                    # Vérifier si on est dans une méthode clone()
                                    context_start = max(0, content.rfind('\n', 0, content.find(line)-1) - 500)
                                    context = content[context_start:content.find(line)]
                                    if re.search(r'\bclone\s*\(\s*\)\s*(const)?\s*{', context):
                                        continue  # Clone() est légitime

                                # 2. return new ConcreteClass(*this) - Pattern Prototype
                                if 'return new' in code_part and '*this' in code_part:
                                    continue  # Clone pattern légitime

                                # Violation trouvée !
                                relative_path = source_file.relative_to(PROJECT_ROOT)
                                self.violations.append((
                                    str(relative_path),
                                    class_name,
                                    line_num,
                                    line.strip()
                                ))

                    except Exception as e:
                        print(f"⚠️  Erreur lecture {source_file}: {e}")

        print(f"   Vérifié {total_classes_checked} classes concrètes")

    def report(self) -> int:
        """Affiche le rapport des violations"""
        print("\n" + "="*80)
        print("📊 RAPPORT DES VIOLATIONS DU FACTORY PATTERN")
        print("="*80)

        if not self.violations:
            print("\n✅ AUCUNE VIOLATION DÉTECTÉE")
            print("\nToutes les classes concrètes sont créées via le Factory Pattern.")
            print("Architecture respectée ! 🎉")
            return 0

        print(f"\n❌ {len(self.violations)} VIOLATION(S) DÉTECTÉE(S)\n")

        # Grouper par fichier
        violations_by_file: Dict[str, List] = {}
        for file_path, class_name, line_num, code in self.violations:
            if file_path not in violations_by_file:
                violations_by_file[file_path] = []
            violations_by_file[file_path].append((class_name, line_num, code))

        for file_path, violations in sorted(violations_by_file.items()):
            print(f"\n📄 {file_path}")
            for class_name, line_num, code in violations:
                print(f"   ❌ Ligne {line_num}: new {class_name}()")
                print(f"      Code: {code}")
                print(f"      → Utiliser: IInterface::create() au lieu de new {class_name}()")

        print("\n" + "="*80)
        print(f"Total: {len(self.violations)} violation(s) à corriger")
        print("="*80)

        return 1  # Code d'erreur

    def run(self) -> int:
        """Exécute la vérification complète"""
        print("\n" + "="*80)
        print("🏗️  VÉRIFICATION ARCHITECTURALE : FACTORY PATTERN")
        print("="*80)

        self.discover_interfaces()
        self.discover_concrete_classes()
        self.check_violations()

        return self.report()


def main():
    """Point d'entrée du script"""
    checker = FactoryViolationChecker()
    exit_code = checker.run()

    sys.exit(exit_code)


if __name__ == "__main__":
    main()
