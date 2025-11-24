#include "ConstraintSolver.h"
#include <QtMath>
#include <algorithm>

namespace LaserCutStudio {
namespace Core {

ConstraintSolver::ConstraintSolver(QObject* parent)
    : QObject(parent),
      m_maxIterations(100),
      m_tolerance(1e-6),
      m_iterationCount(0),
      m_residualError(0.0),
      m_converged(false)
{
}

bool ConstraintSolver::solve(const QList<IConstraint*>& constraints)
{
    if (constraints.isEmpty()) {
        return true;
    }

    emit solvingStarted();

    // Trier par priorité (verrouillées en premier)
    QList<IConstraint*> sortedConstraints = sortByPriority(constraints);

    m_iterationCount = 0;
    m_residualError = 0.0;
    m_converged = false;

    for (int iter = 0; iter < m_maxIterations; ++iter) {
        m_iterationCount = iter + 1;

        // Appliquer chaque contrainte
        for (IConstraint* constraint : sortedConstraints) {
            if (constraint) {
                constraint->apply();
            }
        }

        // Calculer l'erreur totale
        m_residualError = calculateTotalError(sortedConstraints);

        emit iterationCompleted(m_iterationCount, m_residualError);

        // Vérifier la convergence
        if (m_residualError < m_tolerance) {
            m_converged = true;
            emit convergedChanged(true);
            emit solvingFinished(true);
            return true;
        }
    }

    // Pas convergé
    m_converged = false;
    emit convergedChanged(false);
    emit solvingFinished(false);
    return false;
}

void ConstraintSolver::setMaxIterations(int maxIterations)
{
    if (m_maxIterations != maxIterations) {
        m_maxIterations = maxIterations;
        emit maxIterationsChanged(maxIterations);
    }
}

void ConstraintSolver::setTolerance(double tolerance)
{
    if (!qFuzzyCompare(m_tolerance, tolerance)) {
        m_tolerance = tolerance;
        emit toleranceChanged(tolerance);
    }
}

double ConstraintSolver::calculateTotalError(const QList<IConstraint*>& constraints) const
{
    double totalError = 0.0;

    for (const IConstraint* constraint : constraints) {
        if (constraint) {
            double error = constraint->error();
            totalError += error * error;  // Somme des carrés (RMS)
        }
    }

    return qSqrt(totalError);
}

QList<IConstraint*> ConstraintSolver::sortByPriority(const QList<IConstraint*>& constraints) const
{
    QList<IConstraint*> sorted = constraints;

    std::sort(sorted.begin(), sorted.end(), [](const IConstraint* a, const IConstraint* b) {
        // Contraintes verrouillées en premier
        if (a->isLocked() && !b->isLocked()) return true;
        if (!a->isLocked() && b->isLocked()) return false;

        // Sinon, par priorité décroissante
        return a->priority() > b->priority();
    });

    return sorted;
}

} // namespace Core
} // namespace LaserCutStudio
