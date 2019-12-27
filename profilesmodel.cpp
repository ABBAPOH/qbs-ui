#include "profilesmodel.h"

#include <tools/settings.h>

ProfilesModel::ProfilesModel(QObject *parent) :
    QAbstractListModel(parent),
    m_settings(std::make_unique<qbs::Settings>(QString()))
{
    readSettings();
}

ProfilesModel::~ProfilesModel() = default;

int ProfilesModel::rowCount(const QModelIndex &parent) const
{
    if (!checkIndex(parent))
        return {};
    if (parent.isValid())
        return 0;
    return m_data.size();
}

QVariant ProfilesModel::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index, CheckIndexOption::IndexIsValid))
        return {};
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        const auto data = m_data.at(index.row());
        if (data == m_defaultProfile)
            return tr("%1 (default)").arg(data);
        return data;
    }
    return {};
}

void ProfilesModel::readSettings()
{
    beginResetModel();
    m_data = m_settings->profiles();
    m_defaultProfile = m_settings->defaultProfile();
    endResetModel();
}
