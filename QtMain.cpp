#include "stdafx.h"
#include "QtSimulatorWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QApplication app(argc, argv);
    QtSimulatorWindow win;
    return win.run("Flight Simulator") ? 0 : 1;
}
