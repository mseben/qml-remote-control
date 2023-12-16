#include <QWebSocket>

#include "qmlremotecontrol.h"

const quint16 wsPort=2709;

QmlRemoteControl::QmlRemoteControl(QObject *parent)
   : QObject{parent}
{
    qCInfo(lcQmlRemoteControl) << Q_FUNC_INFO;
    _guiControl = new GuiControl(this);
    _wsServer = new QWebSocketServer(QStringLiteral("QmlRemoteControl"),
                                     QWebSocketServer::NonSecureMode,
                                     this);

    if (_wsServer->listen(QHostAddress::Any, wsPort)) {
        qCInfo(lcQmlRemoteControl) << "Listening on port" << wsPort;
        connect(_wsServer,
                &QWebSocketServer::newConnection,
                this,
                &QmlRemoteControl::onNewConnection);
    }
}

void QmlRemoteControl::onNewConnection()
{
    qCInfo(lcQmlRemoteControl) << Q_FUNC_INFO;

    QWebSocket *pSocket = _wsServer->nextPendingConnection();
    connect(pSocket, &QWebSocket::textMessageReceived, this, &QmlRemoteControl::processTextMessage);
}

void QmlRemoteControl::processTextMessage(QString message)
{
    qCInfo(lcQmlRemoteControl) << Q_FUNC_INFO << message;
    _guiControl->executeCommand(message);
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (pClient) {
        pClient->sendTextMessage("ok");
    }
}

void QmlRemoteControl::socketDisconnected()
{
    qCInfo(lcQmlRemoteControl) << Q_FUNC_INFO;

    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (pClient) {
        pClient->deleteLater();
    }
}

QmlRemoteControl::~QmlRemoteControl() {
    qCInfo(lcQmlRemoteControl) << Q_FUNC_INFO;
}
