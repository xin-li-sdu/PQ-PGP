﻿import QtQuick
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Controls
import FluentUI

FluScrollablePage{

    title:"InfoBar"
    leftPadding:10
    rightPadding:10
    bottomPadding:20

    FluButton{
        text:"Info"
        Layout.topMargin: 20
        onClicked: {
            showInfo("这是一个Info样式的InfoBar")
        }
    }
    FluButton{
        text:"Warning"
        Layout.topMargin: 20
        onClicked: {
            showWarning("这是一个Warning样式的InfoBar")
        }
    }
    FluButton{
        text:"Error"
        Layout.topMargin: 20
        onClicked: {
            showError("这是一个Error样式的InfoBar")
        }
    }
    FluButton{
        text:"Success"
        Layout.topMargin: 20
        onClicked: {
            showSuccess("这是一个Success样式的InfoBar这是一个Success样式的InfoBar")
        }
    }
}
