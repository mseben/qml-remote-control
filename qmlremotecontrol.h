#pragma once

#include <QWebSocketServer>
#include <QTimer>

#include "guicontrol.h"

class QmlRemoteControl : public QObject
{
    Q_OBJECT
public:
    QmlRemoteControl(QObject *parent);
    ~QmlRemoteControl();

private Q_SLOTS:
    void onNewConnection();
    void socketDisconnected();
    void processTextMessage(QString message);
private:
    QWebSocketServer *_wsServer;
    QTimer *_timer;
    GuiControl *_guiControl;
};
