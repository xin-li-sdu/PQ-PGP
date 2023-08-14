import QtQuick
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Controls
import FluentUI
import QtQuick.Dialogs

FluScrollablePage{

    title:"Dialog"
    leftPadding:10
    rightPadding:10
    bottomPadding:20

//    FluContentDialog{
//        id:dialog
//        title:"友情提示"
//        message:"确定要退出程序么？"
//        negativeText:"取消"
//        onNegativeClicked:{
//            showSuccess("点击取消按钮")
//        }
//        positiveText:"确定"
//        onPositiveClicked:{
//            showSuccess("点击确定按钮")
//        }
//    }

//    FluButton{
//        Layout.topMargin: 20
//        text:"Show Dialog"
//        onClicked: {
//            dialog.open()
//        }
//    }
        width: 640
        height: 480
        visible: true


            Button
            {
                text: qsTr("Choose File")
                onClicked: fileDialog.open()
            }

            FileDialog
            {
                id: fileDialog
                selectedNameFilter.index: 1
                nameFilters: ["Text files (*.txt)", "HTML files (*.html *.htm)"]
            }

            MyDocument {
                id: document
                fileType: fileDialog.selectedNameFilter.extensions[0]
            }

}
