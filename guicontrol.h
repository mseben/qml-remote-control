#pragma once
#include <QObject>
#include <QPointer>
#include <QQmlEngine>
#include <QQuickItem>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(lcQmlRemoteControl)

class GuiControl : public QObject
{
    Q_OBJECT
public:
    GuiControl(QObject *parent);
    void executeCommand(const QString cmdData) const    ;

private:
    void executeClick(QQuickItem *qmlItem) const;
    void executeSetText(QQuickItem *qmlItem, const QString &rmtCmd) const;
    void executePressKeys(QQuickItem *qmlItem, const QString &rmtCmd) const;
    void delayAndProcessEvents() const;

    void executeDumpObjectTree(QObject *parentObject, const QString &objectPath) const;
    QQuickItem *findQQuickItem(QObject *parentObject, const QString &objectName) const;

    QPointer<QQuickItem> findQmlItem(const QString &objName) const;
    std::tuple<QString, QString> parseMsgData(const QString udpData) const;
    QQmlEngine *_qml_engine;
};
