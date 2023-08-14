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

    title:"认证"

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
        anchors
        {
            //bottom: parent.bottom
            //bottomMargin: 30
            top:parent.top
            topMargin: 30
            left: parent.left
            right: parent.right
            leftMargin: 10
            rightMargin: 10
        }


        ScrollView   ///输入框
        {
            id:scrollview22
            anchors{
               // bottom: parent.bottom
                top:parent.top
                left: parent.left
                right: parent.right
                bottomMargin: 10
                leftMargin: 10
                rightMargin: 10
               // Layout.preferredHeight: 100

            }
            height: Math.min(textbox.implicitHeight,64)
            //FluTextBox
            FluMultilineTextBox
            {
                 //top:parent.bottom
                Layout.topMargin: 20
                id:textbox_mpk
                focus:true
                placeholderText: "sender' mpk\n\n\n"

            }
        }
    }
    Item
        {
            id:layout_bottom12
            height: 150
            anchors
            {
                //bottom: parent.bottom
                //bottomMargin: 30
                top:parent.top
                topMargin: 30
                left: parent.left
                right: parent.right
                leftMargin: 10
                rightMargin: 10
            }
        ScrollView   ///输入框
        {
            id:scrollview11
            anchors{
               // bottom: parent.bottom
                top:parent.bottom
                left: parent.left
                right: parent.right
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
                placeholderText: "明文\n\n\n"

            }

        }
     }
    Item
    {
        id:layout_bottom1
        height: 150
        anchors
        {
            bottom: parent.bottom
            bottomMargin: 200
            //top:parent.top
            //topMargin: 30
            left: parent.left
            right: parent.right
            leftMargin: 10
            rightMargin: 10
        }
        ScrollView   ///输入框
        {
            id:scrollview1
            anchors{
                //bottom: parent.bottom
                top:parent.bottom
                left: parent.left
                right: parent.right
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
                placeholderText: "signature\n\n\n"
            }

        }
    }
        FluFilledButton//按钮的操作
        {
            id:button_send
            text:controller.isLoading ? timer_loading.loadingText :"认证"
            anchors
            {
                //top:parent.top
                bottom: parent.bottom
                //bottom: bottom
                right: parent.right
                //left: parent.left
                bottomMargin: 10
                //rightMargin: 150
                //leftMargin: 150

            }
           width: 180
            height:60
            disabled: controller.isLoading
            onClicked://点击的作用
            {
                if(textbox.text === "")
                   {
                      showWarning("明文不能为空")
                      return
                   }
                else if(textbox_code.text === "")
                {
                    showWarning("signature不能为空")
                    return
                }
                else if(textbox_mpk.text === "")
                {
                    showWarning("sender' mpk不能为空")
                    return
                }
                else
                {
                   var text_code = textbox_code.text//获得他的文本
                   var text = textbox.text//获得他的文本
                    var text_mpk =textbox_mpk.text
              //  appendMessage(true,text)//给这个txt做了什么处理？
                   if(!controller.sendvfyMessage(text,text_code,text_mpk))
                   {
                       showError("认证失败")
                       return
                   }
                   else
                   {
                       showSuccess("认证成功")
                       return
                   }

               // textbox.clear()//发送完清空文本
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
