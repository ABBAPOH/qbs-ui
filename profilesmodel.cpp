#include "profilesmodel.h"

#include <tools/settings.h>

#include <QtCore/QDebug>

ProfilesModel::ProfilesModel(QObject *parent) :
    QAbstractListModel(parent),
    m_settings(std::make_unique<qbs::Settings>(QString()))
{
    readSettings();
}

void ProfilesModel::setCurrentProfile(QString profile)
{
    if (m_currentProfile == profile)
        return;
    int newRow = m_data.indexOf(profile);
    if (newRow == -1) {
        qWarning() << "unknown profile" << profile;
        return;
    }
    const auto oldProfile = std::move(m_currentProfile);
    m_currentProfile = std::move(profile);
    const auto newIndex = this->index(newRow);
    emit dataChanged(newIndex, newIndex);

    int oldRow = m_data.indexOf(oldProfile);
    if (oldRow == -1)
        return;
    const auto oldIndex = this->index(oldRow);
    emit dataChanged(oldIndex, oldIndex);
}

QString ProfilesModel::effectiveProfile() const
{
    if (!m_currentProfile.isEmpty())
        return m_currentProfile;
    return m_defaultProfile;
}

QString ProfilesModel::profile(QModelIndex index) const
{
    if (!checkIndex(index, CheckIndexOption::IndexIsValid))
        return {};
    return m_data.at(index.row());
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
        if (data == m_defaultProfile && data == m_currentProfile)
            return tr("%1 (default, current)").arg(data);
        else if (data == m_defaultProfile)
            return tr("%1 (default)").arg(data);
        else if (data == m_currentProfile)
            return tr("%1 (current)").arg(data);
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
