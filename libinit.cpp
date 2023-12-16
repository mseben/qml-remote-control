#include <QCoreApplication>
#include <QtConcurrent>
#include "qmlremotecontrol.h"

int lib_init(void) __attribute__((constructor));

void wait_for_qtapp()
{
    qCInfo(lcQmlRemoteControl) << "QMlRemoteControl loaded using LD_PRELOAD,";
    qCInfo(lcQmlRemoteControl) << "wait for QCoreApplication to start";
    while (QCoreApplication::startingUp()) {
        QThread::msleep(100);
    }
    qCInfo(lcQmlRemoteControl) << "QCoreApplication started";

    QObject *obj = QCoreApplication::instance();
    if (obj == nullptr) {
        qCCritical(lcQmlRemoteControl) << "Can't get QCoreApplication instance from helper thread";
        return;
    }

    QMetaObject::invokeMethod(
        obj,
        [=]() {
            qCInfo(lcQmlRemoteControl) << "instantiate QmlRemoteControl in main thread";
            new QmlRemoteControl(obj);
        },
        Qt::QueuedConnection);
}

bool lib_preloaded()
{
    char *val = getenv("LD_PRELOAD");
    if (val == nullptr) {
        return false;
    }
    QString valStr(val);
    if (valStr.contains("libqmlremotecontrol.so")) {
        return true;
    }
    return false;
}

int lib_init(void)
{
    qCInfo(lcQmlRemoteControl) << "GuiControl library loaded";
    if (lib_preloaded()) {
        QtConcurrent::run(wait_for_qtapp);
    }
    return 0;
}
