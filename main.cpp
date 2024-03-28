#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QGuiApplication>
#include <QSplashScreen>
#include <QtGlobal>
#define START_SCALE_NUMBEL (0.8)
#define START_SCALE_WIDTH  (1920.0 / START_SCALE_NUMBEL)
#define START_SCALE_HEIGHT (1080.0 / START_SCALE_NUMBEL)
int main(int argc, char* argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);

    QDesktopWidget desktop;

    QRect   screenGeometry = desktop.screenGeometry(QCursor::pos());
    float   screenWidth    = screenGeometry.width() / START_SCALE_WIDTH;
    float   screenHeight   = screenGeometry.height() / START_SCALE_HEIGHT;
    QPixmap splashImage(":/new/photo/photo/start.jpg");
    splashImage = splashImage.scaled(splashImage.width() * screenWidth, splashImage.height() * screenHeight);
    QSplashScreen splash(splashImage);
    splash.move(screenGeometry.center() - splash.rect().center());  // 将启动图片移动到屏幕中央
    splash.show();
    MainWindow w;
    splash.finish(&w);
    w.show();

    return a.exec();
}
