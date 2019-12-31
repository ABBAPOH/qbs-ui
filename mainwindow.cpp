#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "projectmodel.h"
#include "profilesmodel.h"

#include <QtWidgets/QProgressBar>
#include <QtWidgets/QFileDialog>

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QStringListModel>
#include <QtCore/QSettings>

#include <tools/settings.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->dockWidget->setTitleBarWidget(new QWidget);
    const auto model = new ProjectModel(ui->treeView);
    ui->treeView->setModel(model);
    ui->splitter->setSizes({200, 100});

    const auto profilesModel = new ProfilesModel(ui->profilesView);
    ui->profilesView->setModel(profilesModel);
    connect(profilesModel, &ProfilesModel::effectiveProfileChanged, this, &MainWindow::resolve);
    auto onDoubleClicked = [this, profilesModel](const QModelIndex &index)
    {
        profilesModel->setCurrentProfile(profilesModel->profile(index));
    };
    connect(ui->profilesView, &QAbstractItemView::doubleClicked, this, onDoubleClicked);

    m_progressBar = new QProgressBar(statusBar());
    m_progressBar->setMaximum(1);
    m_progressBar->setMaximumWidth(200);
    statusBar()->addPermanentWidget(m_progressBar);

    connect(ui->buildButton, &QAbstractButton::clicked, this, &MainWindow::build);
    connect(ui->cleanButton, &QAbstractButton::clicked, this, &MainWindow::cleanProject);
    connect(ui->cancelButton, &QAbstractButton::clicked, this, &MainWindow::cancelJob);

    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::open);

    const auto onProjectResolved = [this, model](const ErrorInfo &error)
    {
        if (error.hasError()) {
            logMessage(error.toString());
            logStatusMessage(tr("Resolve failed"));
            setState(State::NotReady);
            return;
        }
        logStatusMessage(tr("Project resolved"));

//        qDebug() << m_session->projectData().keys();
//        qDebug() << m_session->projectData()["build-system-files"].toArray();
//        qDebug() << m_session->projectData()["products"].toArray();
        model->setProjectData(m_session->projectData());
        setState(State::Ready);
    };
    const auto onProjectBuilt = [this](const ErrorInfo &error)
    {
        if (error.hasError()) {
            logMessage(error.toString());
            logStatusMessage(tr("Build failed"));
        } else {
            logStatusMessage(tr("Build done!"));
        }
        setState(State::Ready);
    };
    const auto onProjectCleaned = [this](const ErrorInfo &error)
    {
        if (error.hasError()) {
            logMessage(error.toString());
            logStatusMessage(tr("Clean failed"));
        } else {
            logStatusMessage(tr("Clean done!"));
        }
        setState(State::Ready);
    };

    connect(m_session.get(), &QbsSession::projectResolved, this, onProjectResolved);
    connect(m_session.get(), &QbsSession::projectBuilt, this, onProjectBuilt);
    connect(m_session.get(), &QbsSession::projectCleaned, this, onProjectCleaned);
    connect(m_session.get(), &QbsSession::taskStarted, this, &MainWindow::onTaskStarted);
    connect(m_session.get(), &QbsSession::taskProgress,
            this, [this](int p){ m_progressBar->setValue(p); });
    connect(m_session.get(), &QbsSession::commandDescription, this, &MainWindow::logMessage);
    auto onProcessResult = [this](
            const QString &executable,
            const QStringList &arguments,
            const QString &workingDir,
            const QStringList &stdOut,
            const QStringList &stdErr,
            bool success)
    {
        logMessage(executable + arguments.join(' '));
        logMessage(stdErr.join("\n"));
    };
    connect(m_session.get(), &QbsSession::processResult, this, onProcessResult);

    onStateChanged(m_state);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setProjectFilePath(QString path)
{
    if (m_projectFilePath == path)
        return;
    m_projectFilePath = std::move(path);
    setBuildDirPath(QFileInfo(m_projectFilePath).absolutePath() + QStringLiteral("/build"));
    resolve();
}

QString MainWindow::buildDirPath() const
{
    return ui->buildDirChooser->path();
}

void MainWindow::setBuildDirPath(QString path)
{
    ui->buildDirChooser->setPath(path);
}

void MainWindow::open()
{
    QSettings set;
    const auto lastFilePathKey = QStringLiteral("lastFilePath");
    const auto lastFilePath = set.value(lastFilePathKey).toString();
    const auto filePath = QFileDialog::getOpenFileName(
            this, tr("Open project"), lastFilePath, QStringLiteral("*.qbs"));
    if (filePath.isEmpty())
        return;
    set.setValue(lastFilePathKey, filePath);
    setProjectFilePath(filePath);
}

void MainWindow::resolve(const QString &profile)
{
    const auto dir = buildDirPath();
    QJsonObject request;
    request.insert("type", "resolve-project");
    request.insert("dry-run", !QFileInfo::exists(dir));
    request.insert("build-root", dir);
    request.insert("configuration-name", "qbs-ui");
    request.insert("data-mode", "only-if-changed");
    if (!profile.isEmpty())
        request.insert("top-level-profile", profile);

    request.insert("project-file-path", m_projectFilePath);

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

void MainWindow::cancelJob()
{
    QJsonObject request;
    request.insert("type", "cancel-job");
    m_session->sendRequest(request);
}

void MainWindow::logStatusMessage(const QString &message)
{
    statusBar()->showMessage(message);
    logMessage(message);
}

void MainWindow::onTaskStarted(const QString &message, int maxProgress)
{
    setState(State::Working);
    m_progressBar->setValue(0);
    m_progressBar->setMaximum(maxProgress);
    clearLog();
    logStatusMessage(message);
}

void MainWindow::setState(MainWindow::State state)
{
    if (m_state == state)
        return;
    m_state = state;
    onStateChanged(m_state);
}

void MainWindow::onStateChanged(MainWindow::State state)
{
    const bool isDisabled = (state == State::Working || state == State::Empty);
    ui->buildDirChooser->setEnabled(!isDisabled);
    ui->buildButton->setEnabled(state == State::Ready);
//    ui->resolveButton->setEnabled(state != State::Working);
    ui->cleanButton->setEnabled(!isDisabled);
    ui->cancelButton->setEnabled(state == State::Working);
    ui->profilesView->setEnabled(!isDisabled);
}

void MainWindow::logMessage(const QString &message)
{
    qDebug() << message;
    ui->plainTextEdit->appendPlainText(message);
}
