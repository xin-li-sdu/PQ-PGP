﻿import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import FluentUI

FluScrollablePage{

    title:"CalendarPicker"
    leftPadding:10
    rightPadding:10
    bottomPadding:20

    FluArea{
        Layout.fillWidth: true
        Layout.topMargin: 20
        height: 350
        paddings: 10
        FluCalendarView{
        }
    }

    FluArea{
        Layout.fillWidth: true
        Layout.topMargin: 20
        height: 80
        paddings: 10
        ColumnLayout{
            anchors{
                verticalCenter: parent.verticalCenter
                left: parent.left
            }
            FluCalendarPicker{
            }
        }
    }

}
