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

    enum class State {
        Empty,
        NotReady,
        Ready,
        Working
    };

    const QString &projectFilePath() const noexcept { return m_projectFilePath; }
    void setProjectFilePath(QString path);

    QString buildDirPath() const;
    void setBuildDirPath(QString path);

private:
    void open();
    void resolve(const QString &profile = {});
    void build();
    void cleanProject();
    void clearLog();
    void cancelJob();
    void logStatusMessage(const QString &message);
    void logMessage(const QString &message);
    void onTaskStarted(const QString &message, int maxProgress);
    void setState(State state);
    void onStateChanged(State state);

private:
    Ui::MainWindow *ui;
    QProgressBar *m_progressBar{nullptr};
    QString m_projectFilePath;
    std::unique_ptr<QbsSession> m_session{std::make_unique<QbsSession>()};
    State m_state{State::Empty};
};

#endif // MAINWINDOW_H
