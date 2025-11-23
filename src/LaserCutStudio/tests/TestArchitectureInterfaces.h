/**
 * @file TestArchitectureInterfaces.h
 * @brief Tests architecturaux pour vérifier la cohérence des interfaces
 *
 * Ces tests vérifient que toutes les interfaces et sous-interfaces
 * suivent les mêmes patterns architecturaux (Factory + ListManager).
 */

#ifndef TESTARCHITECTUREINTERFACES_H
#define TESTARCHITECTUREINTERFACES_H

#include <QObject>
#include <QTest>

namespace LaserCutStudio {
namespace Core {
namespace Tests {

/**
 * @class TestArchitectureInterfaces
 * @brief Tests de cohérence architecturale des interfaces
 *
 * Vérifie que toutes les interfaces suivent les patterns établis :
 * - FactoryMixin pour la création d'instances
 * - ListManagerMixin pour l'introspection
 * - DECLARE_TYPE_NAME macro pour le type
 * - IMPLEMENT_CLONE macro pour le prototype pattern
 */
class TestArchitectureInterfaces : public QObject
{
    Q_OBJECT

private slots:
    // Setup/Cleanup
    void initTestCase();
    void cleanupTestCase();

    // Tests des interfaces principales
    void testIShapeHasListManager();
    void testIPartHasListManager();
    void testIJointHasListManager();
    void testIProjectHasListManager();
    void testIEditorCommandHasFactory();

    // Tests des sous-interfaces (spécialisations)
    void testITransformationCommandExists();
    void testITransformationCommandHasListManager();
    void testITransformationCommandHasFactory();
    void testITransformationCommandInheritsFromIEditorCommand();

    // Tests de cohérence
    void testAllSubInterfacesHaveListManager();
    void testAllSubInterfacesHaveFactory();
    void testAllInterfacesFollowNamingConvention();

    // Tests des implémentations concrètes
    void testMoveCommandInheritsFromITransformationCommand();
    void testRotateCommandInheritsFromITransformationCommand();
    void testScaleCommandInheritsFromITransformationCommand();

    // Tests d'introspection
    void testCanListAllTransformationCommands();
    void testTransformationCommandsAreTracked();
    void testTransformationCommandsCanBeCleared();

    // Test architectural générique (appelle le script Python)
    void testNoFactoryPatternViolations();
};

} // namespace Tests
} // namespace Core
} // namespace LaserCutStudio

#endif // TESTARCHITECTUREINTERFACES_H
