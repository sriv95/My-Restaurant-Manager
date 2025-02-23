#include <QtWidgets/QApplication>
#include "mainwindow.h"
#include "header/json.h"

#include <QSettings>

int main(int argc, char *argv[])
{    QApplication a(argc, argv);


    QSettings settings(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
    settings.setValue("version","v1.0"); //current Version

    RestuarantManagement w;
    w.setWindowTitle("My Restaurant Manager "+settings.value("version").toString());
    w.show();

    return a.exec();
}
