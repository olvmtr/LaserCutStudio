#ifndef CONSTRAINTSOLVER_H
#define CONSTRAINTSOLVER_H

#include "core/models/constraints/IConstraint.h"
#include <QObject>
#include <QList>

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Solveur de contraintes géométriques (méthode de relaxation itérative)
 *
 * Le ConstraintSolver résout un système de contraintes géométriques en appliquant
 * itérativement chaque contrainte jusqu'à convergence.
 *
 * ## Algorithme
 *
 * 1. **Trier** les contraintes par priorité (verrouillées en premier)
 * 2. **Itérer** jusqu'à maxIterations :
 *    - Appliquer chaque contrainte (`constraint->apply()`)
 *    - Calculer l'erreur totale
 *    - Si erreur < tolérance → Convergé ✅
 * 3. **Retourner** le résultat (convergé ou timeout)
 *
 * ## Exemple d'utilisation
 *
 * @code
 * ConstraintSolver solver;
 * solver.setMaxIterations(100);
 * solver.setTolerance(1e-6);
 *
 * QList<IConstraint*> constraints;
 * constraints << distanceConstraint << angleConstraint << fixedPointConstraint;
 *
 * bool success = solver.solve(constraints);
 * if (success) {
 *     qDebug() << "Convergé en" << solver.iterationCount() << "itérations";
 *     qDebug() << "Erreur résiduelle:" << solver.residualError();
 * } else {
 *     qWarning() << "Pas convergé après" << solver.maxIterations() << "itérations";
 * }
 * @endcode
 *
 * @note Méthode simple mais robuste, adaptée à la plupart des sketches
 */
class ConstraintSolver : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int maxIterations READ maxIterations WRITE setMaxIterations NOTIFY maxIterationsChanged)
    Q_PROPERTY(double tolerance READ tolerance WRITE setTolerance NOTIFY toleranceChanged)
    Q_PROPERTY(int iterationCount READ iterationCount NOTIFY iterationCountChanged)
    Q_PROPERTY(double residualError READ residualError NOTIFY residualErrorChanged)
    Q_PROPERTY(bool converged READ converged NOTIFY convergedChanged)

signals:
    void maxIterationsChanged(int newMaxIterations);
    void toleranceChanged(double newTolerance);
    void iterationCountChanged(int newIterationCount);
    void residualErrorChanged(double newResidualError);
    void convergedChanged(bool newConverged);

    void solvingStarted();
    void iterationCompleted(int iteration, double error);
    void solvingFinished(bool success);

public:
    /**
     * @brief Constructeur
     * @param parent Parent Qt
     */
    explicit ConstraintSolver(QObject* parent = nullptr);

    /**
     * @brief Résout un système de contraintes
     * @param constraints Liste des contraintes à satisfaire
     * @return true si convergé, false sinon
     */
    bool solve(const QList<IConstraint*>& constraints);

    // Getters
    int maxIterations() const { return m_maxIterations; }
    double tolerance() const { return m_tolerance; }
    int iterationCount() const { return m_iterationCount; }
    double residualError() const { return m_residualError; }
    bool converged() const { return m_converged; }

    // Setters
    /**
     * @brief Définit le nombre maximal d'itérations
     * @param maxIterations Nombre maximal (défaut : 100)
     */
    void setMaxIterations(int maxIterations);

    /**
     * @brief Définit la tolérance d'erreur
     * @param tolerance Tolérance (défaut : 1e-6)
     */
    void setTolerance(double tolerance);

private:
    /**
     * @brief Calcule l'erreur totale du système
     * @param constraints Liste des contraintes
     * @return Somme des erreurs au carré (RMS)
     */
    double calculateTotalError(const QList<IConstraint*>& constraints) const;

    /**
     * @brief Trie les contraintes par priorité (verrouillées en premier)
     * @param constraints Liste à trier
     * @return Liste triée
     */
    QList<IConstraint*> sortByPriority(const QList<IConstraint*>& constraints) const;

    int m_maxIterations = 100;     ///< Nombre maximal d'itérations
    double m_tolerance = 1e-6;     ///< Tolérance d'erreur
    int m_iterationCount = 0;      ///< Nombre d'itérations effectuées
    double m_residualError = 0.0;  ///< Erreur résiduelle finale
    bool m_converged = false;      ///< État de convergence
};

} // namespace Core
} // namespace LaserCutStudio

#endif // CONSTRAINTSOLVER_H
