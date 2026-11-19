#include "stdafx.h"
#include "SDLMainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    SDLMainWindow win;
    return win.run("Flight Simulator") ? 0 : 1;
}
