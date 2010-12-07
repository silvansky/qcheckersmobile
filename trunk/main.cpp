#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
#ifdef Q_WS_S60
    w.setStyleSheet("#centralWidget { background-color: rgba(255, 255, 255, 0); }"\
	    "QMainWindow { background-image: url(:/bg_symbian.png); }"\
	    "QStackedWidget { background-color: rgba(255, 255, 255, 0); }");
    w.showFullScreen();
#else
    w.setStyleSheet("#centralWidget { background-color: rgba(255, 255, 255, 0); }"\
	    "QMainWindow { background-image: url(:/bg_desktop.png); }");
    w.show();
#endif
    return a.exec();
}
