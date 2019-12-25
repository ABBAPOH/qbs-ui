#include "projectmodel.h"
#include "projectmodel_p.h"

#include <QtCore/QJsonArray>

ProjectModel::ProjectModel(QObject *parent) :
    QAbstractItemModel(parent),
    m_rootItem(std::make_unique<Item>())
{

}

ProjectModel::~ProjectModel() = default;

void ProjectModel::setProjectData(QJsonObject data)
{
    beginResetModel();
    m_data = std::move(data);
    m_rootItem = std::make_unique<Item>();
    traverseProject(m_data, m_rootItem.get());
    endResetModel();
}

QString ProjectModel::typeToString(ProjectModel::ItemType type)
{
    switch (type) {
    case ItemType::Project: return tr("Project");
    case ItemType::Product: return tr("Product");
    }
    return {};
}

QModelIndex ProjectModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!checkIndex(parent, CheckIndexOption::DoNotUseParent))
        return {};
    if (!hasIndex(row, column, parent))
        return {};

    return createIndex(row, column, item(parent)->child(row));
}

QModelIndex ProjectModel::parent(const QModelIndex &child) const
{
    if (!checkIndex(child, CheckIndexOption::DoNotUseParent))
        return {};
    if (!child.isValid())
        return {};

    const auto childItem = static_cast<Item*>(child.internalPointer());
    const auto parentItem = childItem->parent();

    if (parentItem == m_rootItem.get())
        return {};

    return createIndex(parentItem->row(), 0, parentItem);
}

int ProjectModel::rowCount(const QModelIndex &parent) const
{
    if (!checkIndex(parent))
        return {};
    return item(parent)->childCount();
}

int ProjectModel::columnCount(const QModelIndex &parent) const
{
    if (!checkIndex(parent))
        return {};
    return 2;
}

QVariant ProjectModel::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index, CheckIndexOption::IndexIsValid))
        return {};

    const auto item = static_cast<Item*>(index.internalPointer());
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        if (index.column() == 0)
            return item->name;
        else if (index.column() == 1)
            return typeToString(item->type);
    }

    return {};
}

QModelIndex ProjectModel::index(Item *item) const noexcept
{
    return createIndex(item->row(), 0, item);
}

ProjectModel::ItemPointer ProjectModel::item(QModelIndex index) const
{
    if (index.isValid())
        return static_cast<Item *>(index.internalPointer());

    return m_rootItem.get();
}

void ProjectModel::traverseProject(const QJsonObject &project, Item *parent)
{
    const auto products = project["products"].toArray();
    for (const auto &product: products) {
        auto item = std::make_unique<Item>();
        item->name = product["name"].toString();
        item->type = ItemType::Product;
        parent->append(std::move(item));
    }

    const auto subProjects = project["sub-projects"].toArray();
    for (const auto &subProject: subProjects) {
        auto item = std::make_unique<Item>();
        item->name = subProject["name"].toString();
        item->type = ItemType::Project;
        traverseProject(subProject.toObject(), item.get());
        parent->append(std::move(item));
    }
    auto lessThanItem = [](const auto &lhs, const auto &rhs)
    {
        return lhs-> name < rhs->name;
    };
    std::sort(parent->children().begin(), parent->children().end(), lessThanItem);
}
