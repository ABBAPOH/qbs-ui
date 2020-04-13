#ifndef PROFILESMODEL_H
#define PROFILESMODEL_H

#include <QtCore/QAbstractListModel>

#include <memory>

namespace qbs {
class Settings;
}

class ProfilesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ProfilesModel(QObject *parent = nullptr);
    ~ProfilesModel() override;

    const QString &defaultProfile() const noexcept { return m_defaultProfile; }
    const QString &currentProfile() const noexcept { return m_currentProfile; }
    void setCurrentProfile(QString profile);

    QString effectiveProfile() const;
    Q_SIGNAL void effectiveProfileChanged(const QString &profile);

    QString profile(QModelIndex index) const;

public: // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

private:
    void readSettings();

private:
    std::unique_ptr<qbs::Settings> m_settings;
    QStringList m_data;
    QString m_defaultProfile;
    QString m_currentProfile;
};

#endif // PROFILESMODEL_H
