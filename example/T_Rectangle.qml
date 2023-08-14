﻿import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Window
import FluentUI

FluScrollablePage{

    title:"Rectangle"
    leftPadding:10
    rightPadding:10
    bottomPadding:20

    RowLayout{
        Layout.topMargin: 20
        FluRectangle{
            width: 50
            height: 50
            color:"#0078d4"
            radius:[0,0,0,0]
        }
        FluRectangle{
            width: 50
            height: 50
            color:"#744da9"
            radius:[15,15,15,15]
        }
        FluRectangle{
            width: 50
            height: 50
            color:"#ffeb3b"
            radius:[15,0,0,0]
        }
        FluRectangle{
            width: 50
            height: 50
            color:"#f7630c"
            radius:[0,15,0,0]
        }
        FluRectangle{
            width: 50
            height: 50
            color:"#e71123"
            radius:[0,0,15,0]
        }
        FluRectangle{
            width: 50
            height: 50
            color:"#b4009e"
            radius:[0,0,0,15]
        }
    }
    FluText{
        text:"配合图片使用"
        fontStyle: FluText.SubTitle
        Layout.topMargin: 20
    }
    RowLayout{
        spacing: 14
        FluRectangle{
            width: 50
            height: 50
            radius:[25,0,25,25]
            Image {
                asynchronous: true
                anchors.fill: parent
                source: "qrc:/res/svg/avatar_1.svg"
                sourceSize: Qt.size(width,height)
            }
        }
        FluRectangle{
            width: 50
            height: 50
            radius:[10,10,10,10]
            Image {
                asynchronous: true
                anchors.fill: parent
                sourceSize: Qt.size(width,height)
                source: "qrc:/res/svg/avatar_2.svg"
            }
        }
        FluRectangle{
            width: 50
            height: 50
            radius:[25,25,25,25]
            Image {
                asynchronous: true
                anchors.fill: parent
                sourceSize: Qt.size(width,height)
                source: "qrc:/res/svg/avatar_3.svg"
            }
        }
        FluRectangle{
            width: 50
            height: 50
            radius:[0,25,25,25]
            Image {
                asynchronous: true
                anchors.fill: parent
                sourceSize: Qt.size(width,height)
                source: "qrc:/res/svg/avatar_4.svg"
            }
        }
    }
    FluRectangle{
        width: 1080/5
        height: 1439/5
        radius:[25,25,25,25]
        Image {
            asynchronous: true
            source: "qrc:/res/image/image_huoyin.webp"
            anchors.fill: parent
            sourceSize: Qt.size(width,height)
        }
        Layout.topMargin: 10
    }
}
