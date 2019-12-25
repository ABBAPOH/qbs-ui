#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "projectmodel.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QStringListModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->dockWidget->setTitleBarWidget(new QWidget);
    const auto model = new ProjectModel(ui->treeView);
    ui->treeView->setModel(model);

    const auto projectDir = QStringLiteral("/Users/abbapoh/Programming/qt5/alien/qbs");
    const auto projectFilePath = projectDir + QStringLiteral("/qbs.qbs");
    const auto buildDir = projectDir + QStringLiteral("/build");
    setBuildDirPath(buildDir);

    const auto onProjectResolved = [this, model](const ErrorInfo &error)
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

    const auto dir = buildDirPath();
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

QString MainWindow::buildDirPath() const
{
    return ui->buildDirChooser->path();
}

void MainWindow::setBuildDirPath(QString path)
{
    ui->buildDirChooser->setPath(path);
}
