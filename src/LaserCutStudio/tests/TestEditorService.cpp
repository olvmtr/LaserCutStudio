/**
 * @file TestEditorService.cpp
 * @brief Implémentation des tests pour EditorService
 */

#include "TestEditorService.h"
#include "core/models/editor/tool/ShapeCreationTool.h"
#include "core/models/editor/tool/SelectionTool.h"

using namespace LaserCutStudio::Core;

// ===== Setup et Cleanup =====

void Tests::TestEditorService::initTestCase()
{
    qInfo() << "=== TestEditorService: Starting test suite ===";
}

void Tests::TestEditorService::cleanupTestCase()
{
    qInfo() << "=== TestEditorService: Test suite completed ===";
}

void Tests::TestEditorService::init()
{
    // Créer un nouveau EditorService pour chaque test
    m_editorService = new Services::EditorService();
    QVERIFY(m_editorService != nullptr);
}

void Tests::TestEditorService::cleanup()
{
    // Nettoyer
    if (m_editorService) {
        m_editorService->clear();
        delete m_editorService;
        m_editorService = nullptr;
    }
}

// ===== Tests gestion des formes =====

void Tests::TestEditorService::testAddShape()
{
    // Créer une forme
    Rectangle* rect = new Rectangle(0, 0, 100, 50);
    QVERIFY(rect != nullptr);

    // Ajouter à l'éditeur (sans commande pour test simple)
    m_editorService->addShapeDirect(rect);

    // Vérifier que la forme a été ajoutée
    QCOMPARE(m_editorService->getShapeCount(), 1);

    auto shapes = m_editorService->getShapes();
    QCOMPARE(shapes.size(), 1);
    QCOMPARE(shapes[0], rect);
}

void Tests::TestEditorService::testRemoveShape()
{
    // Créer et ajouter une forme
    Rectangle* rect = new Rectangle(0, 0, 100, 50);
    m_editorService->addShapeDirect(rect);
    QCOMPARE(m_editorService->getShapeCount(), 1);

    // Retirer la forme (sans commande)
    m_editorService->removeShapeDirect(rect);

    // Vérifier que la forme a été retirée
    QCOMPARE(m_editorService->getShapeCount(), 0);
    QVERIFY(m_editorService->getShapes().isEmpty());

    // Nettoyer manuellement (forme retirée mais pas détruite)
    delete rect;
}

void Tests::TestEditorService::testGetShapes()
{
    // Créer plusieurs formes
    Rectangle* rect1 = new Rectangle(0, 0, 100, 50);
    Rectangle* rect2 = new Rectangle(10, 10, 50, 50);
    Circle* circle = new Circle(100, 100, 25);

    // Ajouter à l'éditeur
    m_editorService->addShapeDirect(rect1);
    m_editorService->addShapeDirect(rect2);
    m_editorService->addShapeDirect(circle);

    // Vérifier getShapes()
    auto shapes = m_editorService->getShapes();
    QCOMPARE(shapes.size(), 3);
    QVERIFY(shapes.contains(rect1));
    QVERIFY(shapes.contains(rect2));
    QVERIFY(shapes.contains(circle));
}

void Tests::TestEditorService::testShapeCount()
{
    // Initialement vide
    QCOMPARE(m_editorService->getShapeCount(), 0);

    // Ajouter des formes
    m_editorService->addShapeDirect(new Rectangle(0, 0, 100, 50));
    QCOMPARE(m_editorService->getShapeCount(), 1);

    m_editorService->addShapeDirect(new Circle(50, 50, 25));
    QCOMPARE(m_editorService->getShapeCount(), 2);

    m_editorService->addShapeDirect(new Rectangle(20, 20, 30, 30));
    QCOMPARE(m_editorService->getShapeCount(), 3);
}

void Tests::TestEditorService::testClear()
{
    // Ajouter des formes
    m_editorService->addShapeDirect(new Rectangle(0, 0, 100, 50));
    m_editorService->addShapeDirect(new Circle(50, 50, 25));
    QCOMPARE(m_editorService->getShapeCount(), 2);

    // Clear
    m_editorService->clear();

    // Vérifier que tout est vide
    QCOMPARE(m_editorService->getShapeCount(), 0);
    QVERIFY(m_editorService->getShapes().isEmpty());
}

// ===== Tests signaux =====

void Tests::TestEditorService::testShapeAddedSignal()
{
    // Créer un spy pour le signal shapeAdded
    QSignalSpy spy(m_editorService, &Services::EditorService::shapeAdded);
    QVERIFY(spy.isValid());

    // Ajouter une forme
    Rectangle* rect = new Rectangle(0, 0, 100, 50);
    m_editorService->addShapeDirect(rect);

    // Vérifier que le signal a été émis
    QCOMPARE(spy.count(), 1);

    // Vérifier les arguments du signal
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.size(), 1);
    QCOMPARE(arguments.at(0).value<IShape*>(), rect);
}

void Tests::TestEditorService::testShapeRemovedSignal()
{
    // Ajouter une forme
    Rectangle* rect = new Rectangle(0, 0, 100, 50);
    m_editorService->addShapeDirect(rect);

    // Créer un spy pour le signal shapeRemoved
    QSignalSpy spy(m_editorService, &Services::EditorService::shapeRemoved);
    QVERIFY(spy.isValid());

    // Retirer la forme
    m_editorService->removeShapeDirect(rect);

    // Vérifier que le signal a été émis
    QCOMPARE(spy.count(), 1);

    // Vérifier les arguments
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.size(), 1);
    QCOMPARE(arguments.at(0).value<IShape*>(), rect);

    delete rect;
}

void Tests::TestEditorService::testShapeCountChangedSignal()
{
    // Créer un spy pour le signal shapeCountChanged
    QSignalSpy spy(m_editorService, &Services::EditorService::shapeCountChanged);
    QVERIFY(spy.isValid());

    // Ajouter une forme
    m_editorService->addShapeDirect(new Rectangle(0, 0, 100, 50));

    // Vérifier que le signal a été émis
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toInt(), 1);

    // Ajouter une deuxième forme
    m_editorService->addShapeDirect(new Circle(50, 50, 25));

    // Vérifier le signal
    QCOMPARE(spy.count(), 1);
    arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toInt(), 2);
}

// ===== Tests sélection =====

void Tests::TestEditorService::testSelectionExists()
{
    // Vérifier que la sélection existe
    QVERIFY(m_editorService->getSelection() != nullptr);
}

void Tests::TestEditorService::testSelectionAddRemove()
{
    // Créer et ajouter une forme
    Rectangle* rect = new Rectangle(0, 0, 100, 50);
    m_editorService->addShapeDirect(rect);

    auto* selection = m_editorService->getSelection();
    QVERIFY(selection != nullptr);

    // Sélection vide au départ
    QCOMPARE(selection->getCount(), 0);
    QVERIFY(selection->isEmpty());

    // Ajouter à la sélection
    selection->addShape(rect);
    QCOMPARE(selection->getCount(), 1);
    QVERIFY(!selection->isEmpty());

    // Retirer de la sélection
    selection->removeShape(rect);
    QCOMPARE(selection->getCount(), 0);
    QVERIFY(selection->isEmpty());
}

void Tests::TestEditorService::testSelectionClear()
{
    // Créer et ajouter des formes
    Rectangle* rect1 = new Rectangle(0, 0, 100, 50);
    Rectangle* rect2 = new Rectangle(10, 10, 50, 50);
    m_editorService->addShapeDirect(rect1);
    m_editorService->addShapeDirect(rect2);

    auto* selection = m_editorService->getSelection();

    // Sélectionner les deux formes
    selection->addShape(rect1);
    selection->addShape(rect2);
    QCOMPARE(selection->getCount(), 2);

    // Clear
    selection->clear();
    QCOMPARE(selection->getCount(), 0);
    QVERIFY(selection->isEmpty());
}

void Tests::TestEditorService::testSelectionIsSelected()
{
    // Créer et ajouter une forme
    Rectangle* rect = new Rectangle(0, 0, 100, 50);
    m_editorService->addShapeDirect(rect);

    auto* selection = m_editorService->getSelection();

    // Pas sélectionnée au départ
    QVERIFY(!selection->isSelected(rect));

    // Sélectionner
    selection->addShape(rect);
    QVERIFY(selection->isSelected(rect));

    // Désélectionner
    selection->removeShape(rect);
    QVERIFY(!selection->isSelected(rect));
}

void Tests::TestEditorService::testSelectionToggle()
{
    // Créer et ajouter une forme
    Rectangle* rect = new Rectangle(0, 0, 100, 50);
    m_editorService->addShapeDirect(rect);

    auto* selection = m_editorService->getSelection();

    // Toggle 1 : sélectionne
    selection->toggleShape(rect);
    QVERIFY(selection->isSelected(rect));
    QCOMPARE(selection->getCount(), 1);

    // Toggle 2 : désélectionne
    selection->toggleShape(rect);
    QVERIFY(!selection->isSelected(rect));
    QCOMPARE(selection->getCount(), 0);
}

void Tests::TestEditorService::testSelectionCount()
{
    // Créer et ajouter des formes
    Rectangle* rect1 = new Rectangle(0, 0, 100, 50);
    Rectangle* rect2 = new Rectangle(10, 10, 50, 50);
    Circle* circle = new Circle(100, 100, 25);
    m_editorService->addShapeDirect(rect1);
    m_editorService->addShapeDirect(rect2);
    m_editorService->addShapeDirect(circle);

    auto* selection = m_editorService->getSelection();

    // Sélectionner progressivement
    QCOMPARE(selection->getCount(), 0);

    selection->addShape(rect1);
    QCOMPARE(selection->getCount(), 1);

    selection->addShape(rect2);
    QCOMPARE(selection->getCount(), 2);

    selection->addShape(circle);
    QCOMPARE(selection->getCount(), 3);
}

// ===== Tests intégration panneau de liste =====

void Tests::TestEditorService::testGetShapesReturnsValidPointers()
{
    // Créer et ajouter des formes
    Rectangle* rect = new Rectangle(0, 0, 100, 50);
    Circle* circle = new Circle(50, 50, 25);
    m_editorService->addShapeDirect(rect);
    m_editorService->addShapeDirect(circle);

    // Récupérer les formes (comme le fait le panneau)
    auto shapes = m_editorService->getShapes();
    QCOMPARE(shapes.size(), 2);

    // Vérifier que les pointeurs sont valides
    QVERIFY(shapes[0] != nullptr);
    QVERIFY(shapes[1] != nullptr);

    // Vérifier qu'on peut appeler des méthodes
    QVERIFY(!shapes[0]->getTypeName().isEmpty());
    QVERIFY(!shapes[1]->getTypeName().isEmpty());
}

void Tests::TestEditorService::testShapeToVariantForDisplay()
{
    // Créer une forme
    Rectangle* rect = new Rectangle(10, 20, 100, 50);
    m_editorService->addShapeDirect(rect);

    // Récupérer les formes
    auto shapes = m_editorService->getShapes();
    QCOMPARE(shapes.size(), 1);

    // Convertir en QVariant (comme le fait le panneau pour afficher les propriétés)
    QVariantMap props = shapes[0]->toVariant();

    // Vérifier les propriétés
    QCOMPARE(props["type"].toString(), QString("Rectangle"));
    QCOMPARE(props["x"].toDouble(), 10.0);
    QCOMPARE(props["y"].toDouble(), 20.0);
    QCOMPARE(props["width"].toDouble(), 100.0);
    QCOMPARE(props["height"].toDouble(), 50.0);
}

void Tests::TestEditorService::testDeleteSelectedShapes()
{
    // Créer et ajouter des formes
    Rectangle* rect1 = new Rectangle(0, 0, 100, 50);
    Rectangle* rect2 = new Rectangle(10, 10, 50, 50);
    m_editorService->addShapeDirect(rect1);
    m_editorService->addShapeDirect(rect2);
    QCOMPARE(m_editorService->getShapeCount(), 2);

    // Sélectionner une forme
    auto* selection = m_editorService->getSelection();
    selection->addShape(rect1);
    QCOMPARE(selection->getCount(), 1);

    // Supprimer la sélection (comme le fait le bouton du panneau)
    m_editorService->deleteSelectedShapes();

    // Vérifier que la forme sélectionnée a été supprimée
    QCOMPARE(m_editorService->getShapeCount(), 1);
    auto shapes = m_editorService->getShapes();
    QVERIFY(!shapes.contains(rect1)); // rect1 supprimée
    QVERIFY(shapes.contains(rect2));  // rect2 toujours là

    // La sélection doit être vide
    QCOMPARE(selection->getCount(), 0);
}

// ===== Tests pour les problèmes identifiés (TDD) =====

void Tests::TestEditorService::testShapeHasNameProperty()
{
    // PROBLÈME 1 : Les formes n'ont pas de nom
    // Ce test va ÉCHOUER jusqu'à ce qu'on ajoute une propriété "name" aux formes

    // Créer une forme
    Rectangle* rect = new Rectangle(0, 0, 100, 50);
    m_editorService->addShapeDirect(rect);

    // Tenter de récupérer le nom via QVariant (comme le fait QML)
    QVariantMap props = rect->toVariant();

    // ATTENDU : La propriété "name" devrait exister
    QVERIFY(props.contains("name"));

    // ATTENDU : On devrait pouvoir définir un nom
    rect->setProperty("name", "Mon Rectangle");

    // ATTENDU : Le nom devrait être récupérable
    QCOMPARE(rect->property("name").toString(), QString("Mon Rectangle"));

    // ATTENDU : Le nom devrait apparaître dans toVariant()
    QVariantMap updatedProps = rect->toVariant();
    QVERIFY(updatedProps.contains("name"));
    QCOMPARE(updatedProps["name"].toString(), QString("Mon Rectangle"));
}

void Tests::TestEditorService::testSetSelectionPropertyModifiesShape()
{
    // PROBLÈME 2 : Modifier les propriétés depuis le panneau ne change rien
    // Ce test va ÉCHOUER si setSelectionProperty() ne modifie pas réellement la forme

    // Créer et ajouter une forme
    Rectangle* rect = new Rectangle(0, 0, 100, 50);
    m_editorService->addShapeDirect(rect);

    // Sélectionner la forme
    auto* selection = m_editorService->getSelection();
    selection->addShape(rect);
    QCOMPARE(selection->getCount(), 1);

    // Vérifier les valeurs initiales
    QVariantMap initialProps = rect->toVariant();
    QCOMPARE(initialProps["x"].toDouble(), 0.0);
    QCOMPARE(initialProps["y"].toDouble(), 0.0);
    QCOMPARE(initialProps["width"].toDouble(), 100.0);
    QCOMPARE(initialProps["height"].toDouble(), 50.0);

    // ATTENDU : Modifier X via setSelectionProperty()
    bool success = m_editorService->setSelectionProperty("x", 50.0);
    QVERIFY(success);

    // ATTENDU : La forme doit avoir changé
    QVariantMap updatedProps = rect->toVariant();
    QCOMPARE(updatedProps["x"].toDouble(), 50.0);

    // ATTENDU : Modifier width
    success = m_editorService->setSelectionProperty("width", 200.0);
    QVERIFY(success);

    updatedProps = rect->toVariant();
    QCOMPARE(updatedProps["width"].toDouble(), 200.0);

    // ATTENDU : Modifier plusieurs propriétés
    m_editorService->setSelectionProperty("y", 30.0);
    m_editorService->setSelectionProperty("height", 80.0);

    updatedProps = rect->toVariant();
    QCOMPARE(updatedProps["x"].toDouble(), 50.0);
    QCOMPARE(updatedProps["y"].toDouble(), 30.0);
    QCOMPARE(updatedProps["width"].toDouble(), 200.0);
    QCOMPARE(updatedProps["height"].toDouble(), 80.0);
}

void Tests::TestEditorService::testSelectionFromListActivatesSelectionTool()
{
    // PROBLÈME 3 : Cliquer sur une forme dans la liste ne passe pas en mode sélection
    // Ce test va ÉCHOUER si l'outil de sélection n'est pas activé automatiquement

    // Créer des outils
    Editor::ShapeCreationTool* circleTool = new Editor::ShapeCreationTool("Circle", m_editorService);
    m_editorService->registerTool(circleTool);

    Editor::SelectionTool* selectionTool = new Editor::SelectionTool(m_editorService);
    m_editorService->registerTool(selectionTool);

    // Créer et ajouter une forme
    Rectangle* rect = new Rectangle(0, 0, 100, 50);
    m_editorService->addShapeDirect(rect);

    // Activer un outil de création (simuler qu'on est en train de dessiner)
    m_editorService->setActiveTool(circleTool);
    QCOMPARE(m_editorService->getActiveTool(), circleTool);

    // SCÉNARIO : L'utilisateur clique sur une forme dans la liste
    // Le panneau devrait :
    // 1. Sélectionner la forme
    // 2. Activer l'outil de sélection

    auto* selection = m_editorService->getSelection();
    selection->clear();
    selection->addShape(rect);
    QCOMPARE(selection->getCount(), 1);

    // ATTENDU : L'outil de sélection devrait être activé automatiquement
    // (Pour l'instant, ce test va échouer car le QML ne le fait pas)

    // Solution attendue : Le QML devrait appeler activateSelectionTool()
    m_editorService->activateSelectionTool();

    // ATTENDU : L'outil actif doit être SelectionTool
    QCOMPARE(m_editorService->getActiveTool(), selectionTool);
    QVERIFY(m_editorService->getActiveTool() != circleTool);

    // ATTENDU : La forme doit être sélectionnée
    QVERIFY(selection->isSelected(rect));
}
