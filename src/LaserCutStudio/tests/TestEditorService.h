/**
 * @file TestEditorService.h
 * @brief Tests unitaires pour EditorService
 *
 * Ces tests vérifient les fonctionnalités utilisées par le panneau de liste
 * des formes (ShapesListPanel.qml).
 */

#ifndef TESTEDITORSERVICE_H
#define TESTEDITORSERVICE_H

#include <QObject>
#include <QTest>
#include <QSignalSpy>
#include "core/services/editor/EditorService.h"
#include "core/models/shapes/implementations/Rectangle.h"
#include "core/models/shapes/implementations/Circle.h"

namespace LaserCutStudio {
namespace Core {
namespace Tests {

/**
 * @class TestEditorService
 * @brief Suite de tests pour EditorService
 *
 * Teste les fonctionnalités suivantes :
 * - Gestion des formes (add, remove, getShapes, count)
 * - Signaux (shapeAdded, shapeRemoved, shapeCountChanged)
 * - Gestion de la sélection (ISelection)
 * - Intégration avec le panneau de liste des formes
 */
class TestEditorService : public QObject
{
    Q_OBJECT

private slots:
    // Setup et cleanup
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Tests gestion des formes
    void testAddShape();
    void testRemoveShape();
    void testGetShapes();
    void testShapeCount();
    void testClear();

    // Tests signaux
    void testShapeAddedSignal();
    void testShapeRemovedSignal();
    void testShapeCountChangedSignal();

    // Tests sélection
    void testSelectionExists();
    void testSelectionAddRemove();
    void testSelectionClear();
    void testSelectionIsSelected();
    void testSelectionToggle();
    void testSelectionCount();

    // Tests intégration panneau de liste
    void testGetShapesReturnsValidPointers();
    void testShapeToVariantForDisplay();
    void testDeleteSelectedShapes();

    // Tests pour les problèmes identifiés (TDD)
    void testShapeHasNameProperty();
    void testSetSelectionPropertyModifiesShape();
    void testSelectionFromListActivatesSelectionTool();

private:
    Services::EditorService* m_editorService = nullptr;
};

} // namespace Tests
} // namespace Core
} // namespace LaserCutStudio

#endif // TESTEDITORSERVICE_H
