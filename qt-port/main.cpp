#include "mainwindow.h"
#include <QGuiApplication>
#include <iostream>

int mainQT(int argc, char *argv[])
{
#ifdef WIN32
    SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED);
#endif
    QGuiApplication a(argc, argv);
    MainWindow w;

    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    fmt.setStencilBufferSize(8);
    w.setFormat(fmt);

    w.showMaximized();
    return a.exec();
}
