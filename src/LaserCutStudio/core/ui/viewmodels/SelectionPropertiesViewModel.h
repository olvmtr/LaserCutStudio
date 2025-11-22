/**
 * @file SelectionPropertiesViewModel.h
 * @brief ViewModel pour les propriétés de la sélection (Pattern MVVM)
 *
 * Ce ViewModel observe automatiquement la sélection et expose les propriétés
 * via Q_PROPERTY pour des bindings QML réactifs sans Connections manuelles.
 *
 * Avantages:
 * - Séparation claire UI/Logique (SOLID)
 * - Bindings automatiques (pas de Connections manuelles)
 * - Testable en C++ (pas besoin de tester QML)
 * - Extensible (ajouter propriétés sans modifier UI)
 */

#ifndef SELECTIONPROPERTIESVIEWMODEL_H
#define SELECTIONPROPERTIESVIEWMODEL_H

#include <QObject>
#include <QString>
#include <QVariant>

namespace LaserCutStudio {
namespace Core {

// Forward declarations
namespace Editor { class ISelection; }
class IShape;  // IShape est dans LaserCutStudio::Core, pas dans un sous-namespace

namespace UI {

/**
 * @class SelectionPropertiesViewModel
 * @brief ViewModel exposant les propriétés de la sélection pour QML
 *
 * Ce ViewModel observe la sélection courante et expose ses propriétés
 * via Q_PROPERTY. Quand la sélection change, les propriétés sont
 * automatiquement mises à jour et les bindings QML sont notifiés.
 *
 * ## Utilisation depuis QML
 *
 * @code
 * SelectionPropertiesViewModel {
 *     id: selectionVM
 *     selection: editor.selection
 * }
 *
 * TextField {
 *     text: selectionVM.name  // Binding automatique!
 *     onEditingFinished: selectionVM.name = text
 * }
 * @endcode
 *
 * ## Pattern MVVM
 *
 * - **Model**: IShape (données métier)
 * - **ViewModel**: SelectionPropertiesViewModel (adaptation pour UI)
 * - **View**: EditorView.qml (affichage)
 *
 * @note Toutes les propriétés sont nullables si aucune sélection
 */
class SelectionPropertiesViewModel : public QObject
{
    Q_OBJECT

    // ===== Propriétés de base =====

    /**
     * @brief Nom de la forme sélectionnée
     * QString vide si pas de sélection ou multi-sélection
     */
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

    /**
     * @brief Type de la forme sélectionnée (ex: "Rectangle", "Circle")
     * QString vide si pas de sélection ou multi-sélection
     */
    Q_PROPERTY(QString type READ type NOTIFY typeChanged)

    /**
     * @brief Position X de la forme
     * NaN si pas de sélection ou multi-sélection
     */
    Q_PROPERTY(double x READ x WRITE setX NOTIFY xChanged)

    /**
     * @brief Position Y de la forme
     * NaN si pas de sélection ou multi-sélection
     */
    Q_PROPERTY(double y READ y WRITE setY NOTIFY yChanged)

    /**
     * @brief Largeur de la forme (si applicable)
     * NaN si pas de sélection, multi-sélection, ou pas de largeur
     */
    Q_PROPERTY(double width READ width WRITE setWidth NOTIFY widthChanged)

    /**
     * @brief Hauteur de la forme (si applicable)
     * NaN si pas de sélection, multi-sélection, ou pas de hauteur
     */
    Q_PROPERTY(double height READ height WRITE setHeight NOTIFY heightChanged)

    // ===== Propriétés de visibilité =====

    /**
     * @brief Indique si une seule forme est sélectionnée
     * Les propriétés sont seulement éditables si true
     */
    Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY hasSelectionChanged)

    /**
     * @brief Indique si la forme a une propriété width
     */
    Q_PROPERTY(bool hasWidth READ hasWidth NOTIFY hasWidthChanged)

    /**
     * @brief Indique si la forme a une propriété height
     */
    Q_PROPERTY(bool hasHeight READ hasHeight NOTIFY hasHeightChanged)

    /**
     * @brief Indique si la forme a une propriété radius (Circle)
     */
    Q_PROPERTY(bool hasRadius READ hasRadius NOTIFY hasRadiusChanged)

    /**
     * @brief Rayon de la forme (si applicable)
     * NaN si pas de sélection, multi-sélection, ou pas de rayon
     */
    Q_PROPERTY(double radius READ radius WRITE setRadius NOTIFY radiusChanged)

public:
    explicit SelectionPropertiesViewModel(QObject* parent = nullptr);
    ~SelectionPropertiesViewModel() override;

    // ===== Getters =====

    QString name() const { return m_name; }
    QString type() const { return m_type; }
    double x() const { return m_x; }
    double y() const { return m_y; }
    double width() const { return m_width; }
    double height() const { return m_height; }
    double radius() const { return m_radius; }

    bool hasSelection() const { return m_hasSelection; }
    bool hasWidth() const { return m_hasWidth; }
    bool hasHeight() const { return m_hasHeight; }
    bool hasRadius() const { return m_hasRadius; }

    // ===== Setters =====

    void setName(const QString& name);
    void setX(double x);
    void setY(double y);
    void setWidth(double width);
    void setHeight(double height);
    void setRadius(double radius);

    // ===== Configuration =====

    /**
     * @brief Connecte le ViewModel à une sélection
     * Le ViewModel observe automatiquement les changements de sélection
     */
    Q_INVOKABLE void setSelection(Editor::ISelection* selection);

    /**
     * @brief Obtient la sélection observée
     */
    Editor::ISelection* selection() const { return m_selection; }

signals:
    // Signaux pour bindings QML
    void nameChanged();
    void typeChanged();
    void xChanged();
    void yChanged();
    void widthChanged();
    void heightChanged();
    void radiusChanged();

    void hasSelectionChanged();
    void hasWidthChanged();
    void hasHeightChanged();
    void hasRadiusChanged();

private slots:
    /**
     * @brief Slot appelé quand la sélection change
     * Rafraîchit toutes les propriétés depuis la forme sélectionnée
     */
    void onSelectionChanged();

private:
    /**
     * @brief Rafraîchit toutes les propriétés depuis la sélection courante
     */
    void refreshProperties();

    /**
     * @brief Réinitialise toutes les propriétés (aucune sélection)
     */
    void clearProperties();

    /**
     * @brief Obtient la forme sélectionnée (si une seule)
     * @return Pointeur vers la forme ou nullptr
     */
    IShape* getSelectedShape() const;

    /**
     * @brief Applique une propriété à la forme sélectionnée
     * @param propertyName Nom de la propriété Qt
     * @param value Nouvelle valeur
     */
    void applyPropertyToShape(const QString& propertyName, const QVariant& value);

    // ===== Membres =====

    Editor::ISelection* m_selection;

    // Propriétés cachées
    QString m_name;
    QString m_type;
    double m_x;
    double m_y;
    double m_width;
    double m_height;
    double m_radius;

    bool m_hasSelection;
    bool m_hasWidth;
    bool m_hasHeight;
    bool m_hasRadius;
};

} // namespace UI
} // namespace Core
} // namespace LaserCutStudio

#endif // SELECTIONPROPERTIESVIEWMODEL_H
