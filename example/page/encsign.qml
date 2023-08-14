import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import FluentUI
import encsign
import QtQuick.Dialogs

//FluWindow {
FluContentPage
{

    leftPadding:10
    rightPadding:10
    bottomPadding:20

    //width: 680
    //height: 400
    //minimumWidth: 500
   // minimumHeight: 600

    title:"加密&签名"

    /*onInitArgument:
        (argument)=>{
            scrollview.focus = true
        }
*/
    //ChatController
    ENC_SIGN
    {
        id:controller

        onResponseDataChanged:
        {
            appendMessage(false,responseData)//这句疑似是处理回传txt，respen被标记为false了吗，或者这个就是显示函数

        }
    }


    Item
    {
        id:layout_bottom
        height: 150
        anchors{
            bottom: parent.bottom
            bottomMargin: 30
            left: parent.left
            right: parent.right
            leftMargin: 10
            rightMargin: 10
        }


        ScrollView   ///输入框
        {
            id:scrollview
            anchors{
               // bottom: parent.bottom
                left: parent.left
                right: button_send.left
                bottomMargin: 10
                leftMargin: 10
                rightMargin: 10
               // Layout.preferredHeight: 100

            }
            height: Math.min(textbox.implicitHeight,64)
            //FluTextBox
            FluMultilineTextBox
            {
                Layout.topMargin: 20
                id:textbox
                focus:true
                placeholderText: "reciver's pk\n\n\n"

            }


        }
        ScrollView   ///输入框
        {
            id:scrollview1
            anchors{
                bottom: parent.bottom
                left: parent.left
                right: button_send.left
                bottomMargin: 10
                leftMargin: 10
                rightMargin: 10
            }
            height: Math.min(textbox_code.implicitHeight,64)
            FluMultilineTextBox
            {
                Layout.preferredHeight: 100
                Layout.preferredWidth: 200
                id:textbox_code
                focus:true
                placeholderText: "message\n\n\n"
            }

        }
        FluFilledButton//按钮的操作
        {
            id:button_send
            text:controller.isLoading ? timer_loading.loadingText :"生成"
            anchors
            {
                bottom: parent.bottom
                right: parent.right
                bottomMargin: 10
                rightMargin: 10
            }
            width: 60
            disabled: controller.isLoading
            onClicked://点击的作用
            {
                if(textbox.text === "")
                {
                    showWarning("reciver's pk不能为空")
                    return
                }
                else if(textbox_code.text === "")
                {
                    showWarning("message不能为空")
                    return
                }
                else
                {
                var text = textbox.text//获得他的文本
                var text_code = textbox_code.text//获得他的文本
              //  appendMessage(true,text)//给这个txt做了什么处理？
                controller.sendesMessage(text,text_code)//发送给谁这个文本

               // textbox.clear()//发送完清空文本
                }
            }
  }
    }


    ListModel
    {
        id:model_message
       /* ListElement
        {
            isMy:false
            text:"密文："
        }
        ListElement
        {
            isMy:true
            text:"2"
        }*/
    }

    FluAppBar
    {
        id:appbar
        title:"输出"
   //     width:parent.width
    }

    Component
    {
        id:com_text
        TextEdit
        {
            id:item_text
            text: message
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
            width: Math.min(list_message.width,600,implicitWidth)
            TapHandler
            {
                acceptedButtons: Qt.RightButton
                onTapped:
                {
                    menu_item.showMenu(item_text.selectedText)
                }
            }
        }
    }

    FluArea

    {
        id:layout_content
        anchors{
            top: appbar.bottom
            left: parent.left
            right: parent.right
            bottom: layout_bottom.top
            margins: 10
        }
        color: FluTheme.dark ? Qt.rgba(39/255,39/255,39/255,1) : Qt.rgba(245/255,245/255,245/255,1)
        ListView
        {
            id:list_message
            anchors.fill: parent
            model:model_message
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
                        //top: item_avatar.top
                        left: parent.left
                        right: parent.right
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


    //FluArea
    //从这拿走了



            Timer{
                id:timer_loading
                property int count : 0
                property string loadingText : ""
                interval: 500
                running: controller.isLoading
                repeat: true
                onTriggered: {
                    switch(count%3){
                    case 0:
                        loadingText = "."
                        break
                    case 1:
                        loadingText = ".."
                        break
                    case 2:
                        loadingText = "..."
                        break
                    default:
                        loadingText = ""
                        break
                    }
                    count++
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

    function appendMessage(isMy,text)//判断是不是我发送的，找到这个函数了，用户或后端
    {
        model_message.append({isMy:isMy,text:text})
        list_message.positionViewAtEnd()
    }

}
