#include "productlistmodel.h"

#include <QtCore/QJsonArray>

ProductListModel::ProductListModel(QObject *parent) :
    QAbstractListModel(parent)
{

}

void ProductListModel::setProjectData(QJsonObject data)
{
    beginResetModel();
    std::vector<Item> items;
    traverseProject(data, items);
    m_data = std::move(data);
    m_items = std::move(items);
    endResetModel();
}

QString ProductListModel::targetExecutable(QModelIndex index) const
{
    if (!checkIndex(index, CheckIndexOption::IndexIsValid))
        return {};
    return m_items[index.row()].exe;
}

int ProductListModel::rowCount(const QModelIndex &parent) const
{
    if (!checkIndex(parent))
        return {};
    if (parent.isValid())
        return 0;
    return m_items.size();
}

QVariant ProductListModel::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index, CheckIndexOption::IndexIsValid))
        return {};
    if (role == Qt::DisplayRole || role == Qt::EditRole)
        return m_items[index.row()].name;
    return {};
}

void ProductListModel::traverseProject(const QJsonObject &project, std::vector<Item> &items) const
{
    const auto products = project["products"].toArray();
    for (const auto &product: products) {
        if (product["is-runnable"].toBool()) {
            items.push_back(
                {product["name"].toString(), product["target-executable"].toString()});
        }
    }

    const auto subProjects = project["sub-projects"].toArray();
    for (const auto &subProject: subProjects) {
        traverseProject(subProject.toObject(), items);
    }
}
