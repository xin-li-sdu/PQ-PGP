﻿import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import FluentUI

FluScrollablePage{

    title:"Pivot"
    leftPadding:10
    rightPadding:10
    bottomPadding:20


    FluArea{
        Layout.fillWidth: true
        Layout.topMargin: 20
        height: 400
        paddings: 10

        FluPivot{
            anchors.fill: parent
            FluPivotItem{
                title:"All"
                contentItem:FluText{
                    text:"All emails go here."
                }
            }
            FluPivotItem{
                title:"Unread"
                contentItem:FluText{
                    text:"Unread emails go here."
                }
            }
            FluPivotItem{
                title:"Flagged"
                contentItem:FluText{
                    text:"Flagged emails go here."
                }
            }
            FluPivotItem{
                title:"Urgent"
                contentItem:FluText{
                    text:"Urgent emails go here."
                }
            }
        }
    }


}
