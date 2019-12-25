#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qbssession.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString buildDirPath() const;
    void setBuildDirPath(QString path);

private:
    Ui::MainWindow *ui;
    std::unique_ptr<QbsSession> m_session{std::make_unique<QbsSession>()};
};

#endif // MAINWINDOW_H
