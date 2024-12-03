#include "mainwidget.h"

#include <QApplication>
#include "logindialog.h"

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);

    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    // QFont font;
    // font.setStyleStrategy(QFont::PreferAntialias);
    // font.setHintingPreference(QFont::PreferFullHinting);
    // QApplication::setFont(font);

    QApplication a(argc, argv);
    // MainWidget w;
    LoginDialog w;
    w.show();
    return a.exec();
}
