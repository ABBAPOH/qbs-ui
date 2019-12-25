#ifndef PROJECTMODEL_P_H
#define PROJECTMODEL_P_H

#include "projectmodel.h"
#include "treemodelitem.h"

struct ProjectModel::Item : public TreeModelItem<ProjectModel::Item>
{
    using Type = ProjectModel::ItemType;
    QString name;
    Type type;
};

#endif // PROJECTMODEL_P_H
