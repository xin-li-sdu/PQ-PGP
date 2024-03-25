import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia
import FluentUI

//Window {
//    id:app
//    color: "#00000000"
//    //初始化一个MediaPlayer，解决macos切换到T_MediaPalyer页面崩溃问题
//    MediaPlayer{}
//    Component.onCompleted: {
//        FluApp.init(app,properties)
//        FluTheme.frameless = ("windows" === Qt.platform.os)
//        FluTheme.dark = false
//        FluApp.routes = {
//            "/":"qrc:/page/MainPage.qml",
//            "/about":"qrc:/page/AboutPage.qml",
//            "/login":"qrc:/page/LoginPage.qml",
//            "/chat":"qrc:/page/ChatPage.qml",
//            "/media":"qrc:/page/MediaPage.qml",
//        }
//        FluApp.initialRoute = "/"
//        FluApp.run()
//    }
//}


Window {
    id: app
    color: "#00000000"
    Timer {
        interval: 60  // 设置6秒(6000)的时间间隔
        running: true
        onTriggered:
        {
            // 显示Fluent应用程序的QML界面
            FluApp.run()
        }
    }

    Component.onCompleted: {
        FluApp.init(app,properties)
        FluTheme.frameless = ("windows" === Qt.platform.os)
        FluTheme.dark = false
        FluApp.routes = {
            "/":"qrc:/page/MainPage.qml",
            "/about":"qrc:/page/AboutPage.qml",
            "/login":"qrc:/page/LoginPage.qml",
            "/chat":"qrc:/page/ChatPage.qml",
            "/media":"qrc:/page/MediaPage.qml",
        }

        // 添加Timer控件
        // 设置Fluent应用程序的路由和初始页面，并在Timer中启动应用程序
        FluApp.initialRoute = "/"
        //FluApp.run()  // 注释掉该行代码
    }
}
