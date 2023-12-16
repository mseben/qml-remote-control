import QtQuick 2.0
import QtQuick.Controls 2.15

//https://www.youtube.com/watch?v=Bo5Og2fb1CA

ApplicationWindow
{
    objectName: "applicationWindow"
    visible: true
    width: 640
    height: 480
    title: qsTr("Qml window")

    Column {
        id: column
        objectName: "column"
        width: 127
        height: 75
        anchors.centerIn: parent

        TextField {
            id: textField
            objectName: "textField"
            placeholderText: "Enter some text"
        }
        Button {
            id: button
            objectName: "button"
            text: "Click to open popup"

            onClicked: {
                popup.open()
            }
        }
    }

    Popup {
        id: popup
        objectName: "popup"
        anchors.centerIn: parent
        width: 150
        height: 75
        closePolicy: "CloseOnEscape"

        Column {
            anchors.centerIn: parent
            spacing:10
            Text {
                objectName: "popupText"
                text: textField.text
            }
            Button {
                objectName: "popupButton"
                text: "close"
                width: 100
                onClicked: {
                    popup.close()
                }
            }
        }
    }
}
