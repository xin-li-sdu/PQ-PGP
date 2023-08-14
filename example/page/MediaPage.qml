import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import FluentUI
import encsign
//左下角密钥生成的显示界面.
FluWindow {

    width: 300
    height: 300
    minimumWidth: 300
    minimumHeight:300

    title:"pk"

    ENC_SIGN
    {
        id:controller

        onResponseDataChanged:
        {
            appendMessage(false,responseData)//这句疑似是处理回传txt，respen被标记为false了吗，或者这个就是显示函数

        }
    }

    FluAppBar
    {
        id:appbar
        title:"PK"
        width:parent.width
    }

    Component
    {
        id:com_text
        TextEdit
        {
            id:item_text
            text: "message"
            wrapMode: Text.WrapAnywhere
            readOnly: true
            selectByMouse: true
            selectByKeyboard: true
            selectedTextColor: Qt.rgba(51,153,255,1)
            color:FluColors.Black
            selectionColor: {
                if(FluTheme.dark){
                    return FluTheme.primaryColor.lighter
                }else{
                    return FluTheme.primaryColor.dark
                }
            }
           // width: Math.min(list_message.width-200,600,implicitWidth)
            width:30
        }
    }

    FluArea

    {
        id:layout_content
        anchors{
            top: appbar.bottom
            left: parent.left
            right: parent.right
            //bottom: layout_bottom.top
            margins: 10
        }
        color: FluTheme.dark ? Qt.rgba(39/255,39/255,39/255,1) : Qt.rgba(245/255,245/255,245/255,1)
        ListView
        {
            id:list_message
            anchors.fill: parent
            //model:model_message
            clip: true
            ScrollBar.vertical: FluScrollBar {}
            preferredHighlightBegin: 0
            preferredHighlightEnd: 0
            highlightMoveDuration: 0
            header:Item{
                width: list_message.width
                height:20
            }
            footer:Item{
                width: list_message.width
                height:20
            }
            delegate: Item
            {
                width: ListView.view.width
                height: childrenRect.height

           /*      Rectangle//显示头像
                 {
                    id:item_avatar
                    width: 30
                    height: 30
                    radius:[15,15,15,15]
                    anchors{
                        right: isMy ? parent.right : undefined
                        rightMargin: isMy ? 20 : undefined
                        left: isMy ? undefined : parent.left
                        leftMargin: isMy ? undefined : 20
                        top:parent.top
                    }
                    Image {
                        asynchronous: true
                        anchors.fill: parent
                        sourceSize: Qt.size(100,100)
                        source: isMy ? "qrc:/res/svg/avatar_2.svg" : "qrc:/res/image/logo_openai.png"
                    }
                }
*/
               Rectangle//显示文本的区域（超级重要）
              {
                    id:item_layout_content
                    color: isMy ? Qt.rgba(39/255,39/255,39/255,1) : Qt.rgba(245/255,245/255,245/255,1)//颜色
                    //color: FluTheme.dark ? Qt.rgba(39/255,39/255,39/255,1) : Qt.rgba(251/255,251/255,253/255,1)
                    width: item_msg_loader.width+50
                    height: item_msg_loader.height+10
                    radius: 3
                    anchors{//左还是右
                        top: item_avatar.top
                        right: isMy ? item_avatar.left : undefined

                        rightMargin: isMy ? 10 : undefined
                        //left: isMy ? undefined : item_avatar.right+50//加
                        leftMargin: isMy ? undefined : 10

                    }

                    Loader
                    {
                        id:item_msg_loader
                        property var message: model.text
                        anchors.centerIn: parent
                        sourceComponent: com_text
                    }
                }


                Item{
                    id:item_layout_bottom
                    width: parent.width
                    anchors.top: item_layout_content.bottom
                    height: 20
                }
            }
        }
    }

    FluMenu{
        id:menu_item
        focus: false
        property string selectedText: ""
        FluMenuItem{
            text:"复制"
            onClicked: {
                controller.clipText(menu_item.selectedText)
                showSuccess("复制成功")
            }
        }
        function showMenu(text){
            menu_item.selectedText = text
            menu_item.popup()
        }
    }

}
