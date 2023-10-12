#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QSplashScreen>
int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QDesktopWidget desktop;
    QRect          screenGeometry = desktop.screenGeometry(QCursor::pos());
    QPixmap        splashImage(":/new/photo/photo/SPC100.png");
    QSplashScreen  splash(splashImage);
    splash.move(screenGeometry.center() - splash.rect().center());  // 将启动图片移动到屏幕中央
    splash.show();
    MainWindow w;
    splash.finish(&w);
    w.show();

    return a.exec();
}
