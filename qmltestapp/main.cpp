#include <QApplication>
#include <QQmlApplicationEngine>
#include "qmlremotecontrol.h"

QmlRemoteControl *rmtCtrl;

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    QmlRemoteControl remoteControl(&app);
    return app.exec();
}

