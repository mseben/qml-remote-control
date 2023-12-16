//#include <QGuiApplication>
#include <QApplication>
#include <QLoggingCategory>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTime>
#include <QTimer>
#include <QWindow>

#include "guicontrol.h"

Q_LOGGING_CATEGORY(lcQmlRemoteControl, "QmlRemoteControl")

static const char *CMD_CLICK = "click";
static const char *CMD_SETTEXT = "setText";
static const char *CMD_PRESSKEYS = "pressKeys";
static const char *CMD_DUMP = "dump";

GuiControl::GuiControl(QObject *parent)
    : QObject{parent}
{
    qCInfo(lcQmlRemoteControl) << Q_FUNC_INFO;
    QWindowList lists = QApplication::topLevelWindows();
    //QWindowList lists = QGuiApplication::topLevelWindows();
    if (lists.empty()) {
        qCritical(lcQmlRemoteControl) << "Can't get topLevelWindows";
        return;
    }
    _qml_engine = QQmlEngine::contextForObject(lists[0])->engine();
    if (_qml_engine == nullptr) {
        qCritical(lcQmlRemoteControl) << "Can't get QQmlEngine pointer, guicontrol will not work";
    }
}

std::tuple<QString, QString> GuiControl::parseMsgData(const QString msgData) const
{
    qCDebug(lcQmlRemoteControl) << Q_FUNC_INFO;

    QStringList cmdTokens = msgData.split('.');
    if (cmdTokens.size() == 2) {
        enum { qmlObjName, rmtCmd };
        return {cmdTokens[qmlObjName], cmdTokens[rmtCmd]};
    } else if (cmdTokens.size() == 1) {
        return {QString(), msgData};
    } else {
        qCCritical(lcQmlRemoteControl) << "Wrong gui control command received";
        return {QString(), QString()};
    }
}

void GuiControl::executeCommand(const QString udpData) const
{
    qCDebug(lcQmlRemoteControl) << Q_FUNC_INFO;

    auto [qmlObjName, rmtCmd] = parseMsgData(udpData);

    if (rmtCmd.isEmpty()) {
        qCCritical(lcQmlRemoteControl) << "Can't properly parse data" << rmtCmd;
        return;
    }

    if (rmtCmd == CMD_DUMP) {
        executeDumpObjectTree(_qml_engine, QString("/"));
        return;
    }

    if (qmlObjName.isEmpty()) {
        qCCritical(lcQmlRemoteControl)
            << "QmlObjectName is empty, can't properly parse data" << udpData;
        return;
    }

    QPointer<QQuickItem> qmlItem = findQQuickItem(_qml_engine, qmlObjName);
    if (qmlItem.isNull()) {
        qCCritical(lcQmlRemoteControl) << "Can't find qml item" << qmlObjName;
        return;
    }

    if (rmtCmd == CMD_CLICK) {
        executeClick(qmlItem);
    } else if (rmtCmd.startsWith(CMD_SETTEXT)) {
        executeSetText(qmlItem, rmtCmd);
    } else if (rmtCmd.startsWith(CMD_PRESSKEYS)) {
        executePressKeys(qmlItem, rmtCmd);
    } else {
        qCCritical(lcQmlRemoteControl) << "Can't identify control command";
    }
    delayAndProcessEvents();
}

void GuiControl::executeSetText(QQuickItem *qmlItem, const QString &rmtCmd) const
{
    const QString className = qmlItem->metaObject()->className();
    qCDebug(lcQmlRemoteControl) << Q_FUNC_INFO << className;

    QString text = rmtCmd;
    text.remove(CMD_SETTEXT).remove("(").remove(")");
    qmlItem->setProperty("text", text);
}

void GuiControl::executePressKeys(QQuickItem *qmlItem, const QString &rmtCmd) const
{
    const QString className = qmlItem->metaObject()->className();
    qCDebug(lcQmlRemoteControl) << Q_FUNC_INFO << className;

    QString keys = rmtCmd;
    keys.remove(CMD_SETTEXT).remove("(").remove(")");

    qmlItem->setFocus(true);
    QKeyEvent keyPress(QKeyEvent::KeyPress, keys[0].toLatin1(), Qt::NoModifier, keys, false, 0);
    QGuiApplication::sendEvent(qmlItem, &keyPress);

    QKeyEvent keyRelease(QKeyEvent::KeyRelease, keys[0].toLatin1(), Qt::NoModifier, keys, false, 0);
    QGuiApplication::sendEvent(qmlItem, &keyRelease);
}

//https://forum.qt.io/topic/95023/qml-how-to-simulate-mouse-events/5
void GuiControl::executeClick(QQuickItem *qmlItem) const
{
    const QString className = qmlItem->metaObject()->className();
    qCDebug(lcQmlRemoteControl) << Q_FUNC_INFO << className;

    //reused Button, do not propagate the mouse click to mouse area
    //so try to find child object which is qquickmousearea and send the click directly to it
    if (!className.startsWith("QQuickMouseArea")) {
        qCDebug(lcQmlRemoteControl) << "Try to find QQuickMouseArea for mouse click";
        QObjectList children = qmlItem->children();
        if (!children.empty()) {
            for (QObject *obj : children) {
                QString cn = obj->metaObject()->className();
                if (cn.startsWith("QQuickMouseArea")) {
                    qmlItem = qobject_cast<QQuickItem *>(obj);
                    qCDebug(lcQmlRemoteControl)
                        << "Found QQuickMouseArea will be used for mouse click";
                    break;
                }
            }
        }
    }

    qreal w = qmlItem->width();
    qreal h = qmlItem->height();
    QPointF pos(w / 2, h / 2);

    qmlItem->setFocus(true);

    auto down = new QMouseEvent(QMouseEvent::Type::MouseButtonPress,
                                pos,
                                Qt::MouseButton::LeftButton,
                                Qt::MouseButton::LeftButton,
                                Qt::KeyboardModifier::NoModifier);
    auto up = new QMouseEvent(QMouseEvent::Type::MouseButtonRelease,
                              pos,
                              Qt::MouseButton::LeftButton,
                              Qt::MouseButton::LeftButton,
                              Qt::KeyboardModifier::NoModifier);

    QCoreApplication::postEvent(qmlItem, down);
    QCoreApplication::postEvent(qmlItem, up);
}

void GuiControl::delayAndProcessEvents() const
{
    QTime dieTime = QTime::currentTime().addSecs(1);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

QQuickItem *GuiControl::findQQuickItem(QObject *parentObject, const QString &objectName) const
{
    qInfo(lcQmlRemoteControl) << ":" << parentObject->objectName() << parentObject->metaObject()->className();

    if (parentObject == nullptr) {
        return nullptr;
    }

    QObjectList children = parentObject->children();
    if (children.empty()) {
        return nullptr;
    }
    for (QObject *obj : children) {
        qInfo(lcQmlRemoteControl) << ":" << obj->objectName() << obj->metaObject()->className();
        if (obj->objectName() == objectName) {
            QQuickItem *quickItemObj = qobject_cast<QQuickItem *>(obj);
            return quickItemObj;
        } else {
            QQuickItem *quickItemObj = findQQuickItem(obj, objectName);
            if (quickItemObj != nullptr) {
                return quickItemObj;
            }
        }
    }

    //https://stackoverflow.com/questions/36767512/how-to-access-qml-listview-delegate-items-from-c
    QQuickItem *contentItem = parentObject->property("contentItem").value<QQuickItem *>();
    if (contentItem == nullptr) {
        return nullptr;
    }
    QList<QQuickItem *> contentItemChildren = contentItem->childItems();
    for (QObject *obj : contentItemChildren) {
        if (obj->objectName() == objectName) {
            QQuickItem *quickItemObj = qobject_cast<QQuickItem *>(obj);
            return quickItemObj;
        }
        QQuickItem *quickItemObj = findQQuickItem(obj, objectName);
        if (quickItemObj != nullptr) {
            return quickItemObj;
        }
    }

    return nullptr;
}

void GuiControl::executeDumpObjectTree(QObject *parentObject, const QString &objectPath) const
{
    Q_UNUSED(parentObject)
    Q_UNUSED(objectPath)
    //tbd
}
