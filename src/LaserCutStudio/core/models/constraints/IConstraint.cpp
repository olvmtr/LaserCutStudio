#include "IConstraint.h"

namespace LaserCutStudio {
namespace Core {

IConstraint::IConstraint(bool locked, double priority)
    : Interface(),
      m_locked(locked),
      m_priority(priority)
{
}

IConstraint::~IConstraint()
{
}

void IConstraint::setLocked(bool locked)
{
    if (m_locked != locked) {
        m_locked = locked;
        emit lockedChanged(locked);
        emit constraintChanged();
    }
}

void IConstraint::setPriority(double priority)
{
    if (priority < 0.0) {
        priority = 0.0;
    }

    if (!qFuzzyCompare(m_priority, priority)) {
        m_priority = priority;
        emit priorityChanged(priority);
        emit constraintChanged();
    }
}

} // namespace Core
} // namespace LaserCutStudio
