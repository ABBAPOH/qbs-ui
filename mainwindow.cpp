#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "projectmodel.h"

#include <QtWidgets/QProgressBar>

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QStringListModel>

const auto projectDir = QStringLiteral("/Users/abbapoh/Programming/qt5/alien/qbs");
const auto projectFilePath = projectDir + QStringLiteral("/qbs.qbs");
const auto buildDir = projectDir + QStringLiteral("/build");

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->dockWidget->setTitleBarWidget(new QWidget);
    const auto model = new ProjectModel(ui->treeView);
    ui->treeView->setModel(model);
    ui->splitter->setSizes({200, 100});

    m_progressBar = new QProgressBar(statusBar());
    m_progressBar->setMaximum(1);
    m_progressBar->setMaximumWidth(200);
    statusBar()->addPermanentWidget(m_progressBar);

    setBuildDirPath(buildDir);

    connect(ui->buildButton, &QAbstractButton::clicked, this, &MainWindow::build);
    connect(ui->cleanButton, &QAbstractButton::clicked, this, &MainWindow::cleanProject);

    const auto onProjectResolved = [this, model](const ErrorInfo &error)
    {
        logStatusMessage(tr("Project resolved"));
        qDebug() << "project resolved";
        if (error.hasError()) {
            qWarning() << error.toString();
            return;
        }

//        qDebug() << m_session->projectData().keys();
//        qDebug() << m_session->projectData()["build-system-files"].toArray();
//        qDebug() << m_session->projectData()["products"].toArray();
        model->setProjectData(m_session->projectData());
    };
    const auto onProjectBuilt = [this](const ErrorInfo &error)
    {
        logStatusMessage(tr("Build done!"));
    };
    const auto onProjectCleaned = [this](const ErrorInfo &error)
    {
        logStatusMessage(tr("Clean done!"));
    };

    connect(m_session.get(), &QbsSession::projectResolved, this, onProjectResolved);
    connect(m_session.get(), &QbsSession::projectBuilt, this, onProjectBuilt);
    connect(m_session.get(), &QbsSession::projectCleaned, this, onProjectCleaned);
    connect(m_session.get(), &QbsSession::taskStarted, this, &MainWindow::onTaskStarted);
    connect(m_session.get(), &QbsSession::taskProgress,
            this, [this](int p){ m_progressBar->setValue(p); });
    connect(m_session.get(), &QbsSession::commandDescription, this, &MainWindow::logMessage);

    resolve();
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

void MainWindow::resolve()
{
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

void MainWindow::build()
{
    const auto dir = buildDirPath();
    QJsonObject request;
    request.insert("type", "build-project");

    m_session->sendRequest(request);
}

void MainWindow::cleanProject()
{
    QJsonObject request;
    request.insert("type", "clean-project");
    m_session->sendRequest(request);
}

void MainWindow::clearLog()
{
    ui->plainTextEdit->clear();
}

void MainWindow::logStatusMessage(const QString &message)
{
    statusBar()->showMessage(message);
    logMessage(message);
}

void MainWindow::onTaskStarted(const QString &message, int maxProgress)
{
    m_progressBar->setValue(0);
    m_progressBar->setMaximum(maxProgress);
    clearLog();
    logStatusMessage(message);
}

void MainWindow::logMessage(const QString &message)
{
    qDebug() << message;
    ui->plainTextEdit->appendPlainText(message);
}
