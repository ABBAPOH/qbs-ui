#ifndef PROFILESMODEL_H
#define PROFILESMODEL_H

#include <QtCore/QAbstractListModel>

namespace qbs {
class Settings;
}

class ProfilesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ProfilesModel(QObject *parent = nullptr);
    ~ProfilesModel() override;

public: // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

private:
    void readSettings();

private:
    std::unique_ptr<qbs::Settings> m_settings;
    QStringList m_data;
    QString m_defaultProfile;
};

#endif // PROFILESMODEL_H
