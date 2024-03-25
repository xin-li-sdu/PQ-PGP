import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import FluentUI

FluScrollablePage{

    title:"操作视频"
    leftPadding:10
    rightPadding:10
    bottomPadding:20

    onVisibleChanged: {
        if(visible){
            player.play()
        }else{
            player.pause()
        }
    }

    FluArea{
        Layout.fillWidth: true
        height: 320
        Layout.topMargin: 20
        paddings: 10
        ColumnLayout{
            anchors{
                verticalCenter: parent.verticalCenter
                left:parent.left
            }
            FluMediaPlayer{
                id:player
               // source:"http://clips.vorwaerts-gmbh.de/big_buck_bunny.mp4"
                 // source:"qrc:/res/video/2.mp4"
                source:"https://user-images.githubusercontent.com/40687850/244873119-d4c34726-a58e-402a-8e83-6cc2c1a573b3.mp4"
            }
        }
    }

    FluArea{
        Layout.fillWidth: true
        height: 68
        Layout.topMargin: 20
        paddings: 10

        FluButton{
            text:"在播放器打开操作视频"
            anchors.verticalCenter: parent.verticalCenter
            onClicked:{
                FluApp.navigate("/media",{source:"https://user-images.githubusercontent.com/40687850/244873119-d4c34726-a58e-402a-8e83-6cc2c1a573b3.mp4"})
            }
        }

    }

}

