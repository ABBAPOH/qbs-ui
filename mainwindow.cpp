#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "projectmodel.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QStringListModel>

static QStringList getProducts(const QJsonObject &project)
{
    QStringList list;
    const auto products = project["products"].toArray();
    for (const auto &product: products)
        list.append(product["name"].toString());

    const auto subProjects = project["sub-projects"].toArray();
    for (const auto &subProject: subProjects)
        list.append(getProducts(subProject.toObject()));
    return list;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    const auto model = new ProjectModel(ui->treeView);
    ui->treeView->setModel(model);

    const auto dir = QStringLiteral("/Users/abbapoh/Programming/qt5/alien/qbs/build");
    const auto projectFilePath = QStringLiteral("/Users/abbapoh/Programming/qt5/alien/qbs/qbs.qbs");

    const auto onProjectResolved = [this, model, dir](const ErrorInfo &error)
    {
        qDebug() << "project resolved";
        if (error.hasError()) {
            qWarning() << error.toString();
            return;
        }

        qDebug() << m_session->projectData().keys();
        qDebug() << m_session->projectData()["build-system-files"].toArray();
        qDebug() << m_session->projectData()["products"].toArray();
        model->setProjectData(m_session->projectData());
    };
    connect(m_session.get(), &QbsSession::projectResolved, this, onProjectResolved);

    QJsonObject request;
    request.insert("type", "resolve-project");
    request.insert("dry-run", !QFileInfo::exists(dir));
    request.insert("build-root", dir);
    request.insert("configuration-name", "qbs-ui");
    request.insert("data-mode", "only-if-changed");

    request.insert("project-file-path", projectFilePath);

    m_session->sendRequest(request);
}

MainWindow::~MainWindow()
{
    delete ui;
}

