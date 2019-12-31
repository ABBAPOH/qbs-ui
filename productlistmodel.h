#ifndef PRODUCTLISTMODEL_H
#define PRODUCTLISTMODEL_H

#include <QtCore/QAbstractListModel>
#include <QtCore/QJsonObject>

class ProductListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ProductListModel(QObject *parent = nullptr);

    const QJsonObject &projectData() const noexcept { return m_data; }
    void setProjectData(QJsonObject data);

    QString targetExecutable(QModelIndex index) const;

public: // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

private:
    struct Item
    {
        QString name;
        QString exe;
    };
    void traverseProject(const QJsonObject &project, std::vector<Item> &items) const;

private:
    QJsonObject m_data;
    std::vector<Item> m_items;
};

#endif // PRODUCTLISTMODEL_H
