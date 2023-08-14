﻿import QtQuick
import QtQuick.Controls
import FluentUI

Item {

    property string  headerText: "Titlte"
    property bool expand: false
    property int contentHeight : 300
    default property alias content: container.data

    id:root
    height: layout_header.height + container.height
    width: 400
    implicitWidth: width
    implicitHeight: height

    Rectangle{
        id:layout_header
        width: parent.width
        height: 45
        radius: 4
        color: FluTheme.dark ? Qt.rgba(39/255,39/255,39/255,1) : Qt.rgba(251/255,251/255,253/255,1)
        border.color: FluTheme.dark ? Qt.rgba(45/255,45/255,45/255,1) : Qt.rgba(226/255,229/255,234/255,1)

        MouseArea{
            id:root_mouse
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                expand = !expand
            }
        }

        FluText{
            text: headerText
            anchors{
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: 15
            }
        }

        FluIconButton{
            anchors{
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: 15
            }
            color:{
                if(root_mouse.containsMouse){
                    return FluTheme.dark ? Qt.rgba(73/255,73/255,73/255,1) : Qt.rgba(245/255,245/255,245/255,1)
                }
                return FluTheme.dark ? Qt.rgba(0,0,0,0) : Qt.rgba(0,0,0,0)
            }
            onClicked: {
                expand = !expand
            }
            contentItem: FluIcon{
                rotation: expand?0:180
                iconSource:FluentIcons.ChevronUp
                iconSize: 15
                Behavior on rotation {
                    NumberAnimation{
                        duration: 150
                    }
                }
            }
        }
    }


    Rectangle{
        id:container
        width: parent.width
        clip: true
        anchors{
            top: layout_header.bottom
            left: layout_header.left
        }
        radius: 4
        color: FluTheme.dark ? Qt.rgba(39/255,39/255,39/255,1) : Qt.rgba(251/255,251/255,253/255,1)
        border.color: FluTheme.dark ? Qt.rgba(45/255,45/255,45/255,1) : Qt.rgba(226/255,229/255,234/255,1)
        height: expand ? contentHeight : 0
        Behavior on height {
            NumberAnimation{
                duration: 150
            }
        }
    }

}
