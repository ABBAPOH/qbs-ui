#ifndef PROJECTMODEL_H
#define PROJECTMODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QJsonObject>

#include <memory>

class ProjectModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum class ItemType {Project, Product};

    explicit ProjectModel(QObject *parent = nullptr);
    ~ProjectModel() override;

    const QJsonObject &projectData() const noexcept { return m_data; }
    void setProjectData(QJsonObject data);

    static QString typeToString(ItemType type);

public: // QAbstractItemModel interface
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

private:
    struct Item;
    using ItemPointer = Item*;
    using ItemHolder = std::unique_ptr<Item>;

    QModelIndex index(Item *item) const noexcept;
    ItemPointer item(QModelIndex index) const;
    void traverseProject(const QJsonObject &project, Item *parent);

private:
    ItemHolder m_rootItem;
    QJsonObject m_data;
};

#endif // PROJECTMODEL_H
