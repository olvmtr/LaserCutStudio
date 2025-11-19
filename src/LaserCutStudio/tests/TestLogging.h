#ifndef TESTLOGGING_H
#define TESTLOGGING_H

#include <QObject>
#include <QtTest/QtTest>

namespace LaserCutStudio {
namespace Tests {

/**
 * @brief Tests pour le système de logging
 */
class TestLogging : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Tests LogManager
    void testSingleton();
    void testFormatSelection();
    void testFileOutput();
    void testLoggingRules();
    void testMessageFormatting();

    // Tests LogCategories
    void testCategories();
    void testCategoryFiltering();
};

} // namespace Tests
} // namespace LaserCutStudio

#endif // TESTLOGGING_H
