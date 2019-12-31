#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("qbs-ui"));
    QCoreApplication::setOrganizationName(QStringLiteral("qt-project"));
    MainWindow w;
    w.show();
    return a.exec();
}
