import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import "qrc:///global/"
import FluentUI

FluWindow {
    id:rootwindow
    width: 1000
    height: 640
    title: "后量子算法PGP"
    minimumWidth: 520
    minimumHeight: 460

    FluAppBar{
        id:appbar
        z:10
        showDark: true
        width:parent.width
    }



    FluNavigationView
    {
        id:nav_view
        anchors.fill: parent
        items: ItemsOriginal
        footerItems:ItemsFooter
        //logo: "qrc:/res/image/favicon.ico"
        logo: "qrc:/res/image/favicon.ico"
        z: 11
        title:"PQ-PGP"

//        autoSuggestBox:FluAutoSuggestBox
//        {
//            width: 280
//            anchors.centerIn: parent
//            iconSource: FluentIcons.Zoom
//            items: ItemsOriginal.getSearchData()
//            placeholderText: "查找功能"
//            //text: "查找功能"
//            onItemClicked:
//                (data)=>
//                {
//                    ItemsOriginal.startPageByItem(data)
//                }
//        }
        Component.onCompleted:
        {
            ItemsOriginal.navigationView = nav_view
            nav_view.setCurrentIndex(0)
            nav_view.push("qrc:/T_Home.qml")
        }
    }





}
