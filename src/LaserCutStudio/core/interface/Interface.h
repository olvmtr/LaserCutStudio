#ifndef INTERFACE_H
#define INTERFACE_H

#include <QString>
#include <QUuid>
#include <memory>

namespace LaserCutStudio {
namespace Core {

/**
 * @brief Classe de base abstraite pour le pattern Prototype
 *
 * Cette classe implémente le pattern Prototype permettant de cloner
 * des objets sans connaître leur type concret. Toutes les interfaces
 * du système héritent de cette classe.
 */
class Interface
{
public:
    /**
     * @brief Constructeur par défaut
     * Génère automatiquement un UUID unique pour l'instance
     */
    Interface();

    /**
     * @brief Constructeur de copie
     * @param other Instance à copier
     */
    Interface(const Interface& other);

    /**
     * @brief Destructeur virtuel
     */
    virtual ~Interface() = default;

    /**
     * @brief Clone l'objet (Pattern Prototype)
     * @return Pointeur vers une nouvelle instance clonée
     */
    virtual Interface* clone() const = 0;

    /**
     * @brief Obtient l'UUID unique de l'instance
     * @return UUID de l'instance
     */
    QUuid getId() const { return m_id; }

    /**
     * @brief Obtient l'UUID sous forme de chaîne
     * @return UUID formaté en QString
     */
    QString getIdAsString() const { return m_id.toString(); }

protected:
    /**
     * @brief UUID unique de l'instance
     */
    QUuid m_id;
};

} // namespace Core
} // namespace LaserCutStudio

#endif // INTERFACE_H
