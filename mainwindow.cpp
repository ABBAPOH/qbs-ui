#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QFileInfo>
#include <QtCore/QJsonObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    const auto dir = QStringLiteral("/Users/abbapoh/Programming/qt5/alien/qbs/build");
    const auto projectFilePath = QStringLiteral("/Users/abbapoh/Programming/qt5/alien/qbs/qbs.qbs");

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

