#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qbssession.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QProgressBar;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString buildDirPath() const;
    void setBuildDirPath(QString path);

private:
    void resolve();
    void build();
    void cleanProject();
    void clearLog();
    void logStatusMessage(const QString &message);
    void logMessage(const QString &message);
    void onTaskStarted(const QString &message, int maxProgress);

private:
    Ui::MainWindow *ui;
    QProgressBar *m_progressBar{nullptr};
    std::unique_ptr<QbsSession> m_session{std::make_unique<QbsSession>()};
};

#endif // MAINWINDOW_H
