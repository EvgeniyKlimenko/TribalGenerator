#include <QtWidgets/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString localName(QLocale::system().name());
    QTranslator t;
    t.load(":/translations/TribalGen_" + localName);
    a.installTranslator(&t);

    MainWindow w;
    w.setWindowIcon(QIcon(":/icons/TribalGenerator.ico"));
    w.show();

    return a.exec();
}
