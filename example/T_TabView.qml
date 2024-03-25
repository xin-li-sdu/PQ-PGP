import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import FluentUI
import QtCharts


//FluScrollablePage{
FluContentPage
{
    title: qsTr("这是一个图表实例")
    Rectangle
    {
    Row{



      Item{
        width: 300
        height: 340
        id:table1



           ChartView {
            title: "曲线图"
            anchors.fill: parent
            antialiasing: true

            SplineSeries {
                name: "SplineSeries"
                XYPoint { x: 0; y: 0.0 }
                XYPoint { x: 1.1; y: 3.2 }
                XYPoint { x: 1.9; y: 2.4 }
                XYPoint { x: 2.1; y: 2.1 }
                XYPoint { x: 2.9; y: 2.6 }
                XYPoint { x: 3.4; y: 2.3 }
                XYPoint { x: 4.1; y: 3.1 }
            }
        }

    }
        Item {
            id:table2
            width:300
            height:340
            ChartView {
                title:"条状图"
                anchors.fill:parent
                antialiasing:true
                legend.alignment:Qt.AlignBottom
                BarSeries {
                    axisX:BarCategoryAxis{categories:["2007","2008","2009","2010","2011","2012"]}
                    id:barseries
                    BarSet {label:"Bob";values:[2,2,3,4,5,6]}
                    BarSet {label:"Susan";values:[2,1,2,4,1,7]}
                    BarSet {label:"James";values:[2,5,8,13,5,8]}
                }
            }
    }

}
}


//    property var colors : [FluColors.Yellow,FluColors.Orange,FluColors.Red,FluColors.Magenta,FluColors.Purple,FluColors.Blue,FluColors.Teal,FluColors.Green]

//    Component{
//        id:com_page
//        Rectangle{
//            anchors.fill: parent
//            color: argument
//        }
//    }

//    function newTab(){
//        tab_view.appendTab("qrc:/res/image/favicon.ico","Document "+tab_view.count(),com_page,colors[Math.floor(Math.random() * 8)].dark)
//    }

//    Component.onCompleted: {
//        newTab()
//        newTab()
//        newTab()
//    }

//    FluArea{
//        Layout.fillWidth: true
//        Layout.topMargin: 20
//        height: 50
//        paddings: 10
//        RowLayout{
//            spacing: 14
//            FluDropDownButton{
//                id:btn_tab_width_behavior
//                Layout.preferredWidth: 140
//                text:"Equal"
//                items:[
//                    FluMenuItem{
//                        text:"Equal"
//                        onClicked: {
//                            btn_tab_width_behavior.text = text
//                            tab_view.tabWidthBehavior = FluTabView.Equal
//                        }
//                    },
//                    FluMenuItem{
//                        text:"SizeToContent"
//                        onClicked: {
//                            btn_tab_width_behavior.text = text
//                            tab_view.tabWidthBehavior = FluTabView.SizeToContent
//                        }
//                    },
//                    FluMenuItem{
//                        text:"Compact"
//                        onClicked: {
//                            btn_tab_width_behavior.text = text
//                            tab_view.tabWidthBehavior = FluTabView.Compact
//                        }
//                    }
//                ]
//            }
//            FluDropDownButton{
//                id:btn_close_button_visibility
//                text:"Always"
//                Layout.preferredWidth: 120
//                items:[
//                    FluMenuItem{
//                        text:"Nerver"
//                        onClicked: {
//                            btn_close_button_visibility.text = text
//                            tab_view.closeButtonVisibility = FluTabView.Nerver
//                        }
//                    },
//                    FluMenuItem{
//                        text:"Always"
//                        onClicked: {
//                            btn_close_button_visibility.text = text
//                            tab_view.closeButtonVisibility = FluTabView.Always
//                        }
//                    },
//                    FluMenuItem{
//                        text:"OnHover"
//                        onClicked: {
//                            btn_close_button_visibility.text = text
//                            tab_view.closeButtonVisibility = FluTabView.OnHover
//                        }
//                    }
//                ]
//            }
//        }
//    }

//    FluArea{
//        Layout.fillWidth: true
//        Layout.topMargin: 5
//        height: 400
//        paddings: 10
//        FluTabView{
//            id:tab_view
//            onNewPressed:{
//                newTab()
//            }
//        }
//    }

}
