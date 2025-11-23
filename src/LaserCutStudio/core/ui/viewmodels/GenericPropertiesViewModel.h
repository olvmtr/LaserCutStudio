/**
 * @file GenericPropertiesViewModel.h
 * @brief ViewModel générique utilisant l'introspection Qt (QMetaObject)
 *
 * Ce ViewModel utilise le système d'introspection Qt pour exposer automatiquement
 * les propriétés de n'importe quel QObject sélectionné, sans duplication de code.
 *
 * ## Philosophie : Introspection > Duplication
 *
 * Au lieu de redéclarer manuellement chaque propriété (name, x, y, width, etc.),
 * ce ViewModel utilise QMetaObject pour les lire/écrire dynamiquement.
 *
 * ## Avantages
 *
 * - **Zéro duplication** : Pas besoin de Q_PROPERTY pour chaque propriété
 * - **Auto-extensible** : Ajouter une propriété à IShape = automatiquement disponible
 * - **Réutilisable** : Fonctionne avec IShape, IPart, IJoint, IProject, etc.
 * - **Cohérent** : Utilise toVariant() et QMetaObject comme le reste du code
 * - **Compact** : ~100 lignes au lieu de 595
 *
 * ## Utilisation QML
 *
 * @code
 * GenericPropertiesViewModel {
 *     id: propsVM
 *     selection: editor.selection
 * }
 *
 * TextField {
 *     text: propsVM.properties["name"] || ""
 *     enabled: propsVM.hasSelection
 *     onEditingFinished: propsVM.setProperty("name", text)
 * }
 *
 * SpinBox {
 *     value: propsVM.properties["x"] || 0
 *     onValueModified: propsVM.setProperty("x", value)
 * }
 * @endcode
 */

#ifndef GENERICPROPERTIESVIEWMODEL_H
#define GENERICPROPERTIESVIEWMODEL_H

#include <QObject>
#include <QVariantMap>

namespace LaserCutStudio {
namespace Core {

// Forward declarations
namespace Editor { class ISelection; }

namespace UI {

/**
 * @class GenericPropertiesViewModel
 * @brief ViewModel générique basé sur introspection Qt
 *
 * Expose automatiquement toutes les propriétés Qt (Q_PROPERTY) de l'objet
 * sélectionné via QMetaObject, sans duplication de code.
 */
class GenericPropertiesViewModel : public QObject
{
    Q_OBJECT

    /**
     * @brief Map contenant toutes les propriétés de l'objet sélectionné
     * Récupérées automatiquement via QMetaObject::property()
     */
    Q_PROPERTY(QVariantMap properties READ properties NOTIFY propertiesChanged)

    /**
     * @brief Indique si une seule entité est sélectionnée
     */
    Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY hasSelectionChanged)

public:
    explicit GenericPropertiesViewModel(QObject* parent = nullptr);
    ~GenericPropertiesViewModel() override;

    // ===== Getters =====

    QVariantMap properties() const { return m_properties; }
    bool hasSelection() const { return m_hasSelection; }

    // ===== Configuration =====

    /**
     * @brief Connecte le ViewModel à une sélection
     * @param selection Gestionnaire de sélection à observer
     */
    Q_INVOKABLE void setSelection(Editor::ISelection* selection);

    /**
     * @brief Obtient la sélection observée
     */
    Editor::ISelection* selection() const { return m_selection; }

    /**
     * @brief Modifie une propriété de l'objet sélectionné via introspection
     * @param name Nom de la propriété Qt
     * @param value Nouvelle valeur
     * @return true si modification réussie
     */
    Q_INVOKABLE bool setProperty(const QString& name, const QVariant& value);

signals:
    void propertiesChanged();
    void hasSelectionChanged();

private slots:
    /**
     * @brief Slot appelé quand la sélection change
     */
    void onSelectionChanged();

private:
    /**
     * @brief Rafraîchit les propriétés depuis l'objet sélectionné
     * Utilise QMetaObject pour introspection automatique
     */
    void refreshProperties();

    /**
     * @brief Obtient l'objet sélectionné (si une seule entité)
     * @return Pointeur vers l'objet ou nullptr
     */
    QObject* getSelectedObject() const;

    // ===== Membres =====

    Editor::ISelection* m_selection;
    QVariantMap m_properties;
    bool m_hasSelection;
};

} // namespace UI
} // namespace Core
} // namespace LaserCutStudio

#endif // GENERICPROPERTIESVIEWMODEL_H
