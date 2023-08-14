import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import FluentUI
import Controller

FluScrollablePage{

    title:"加密&签名"
    leftPadding:10
    rightPadding:10
    bottomPadding:20


    ChatController
    {
        id:controller

        onResponseDataChanged:
        {
           // appendMessage(false,responseData)//这句疑似是处理回传txt，respen被标记为false了吗

        }
    }
    FluTextBox{
        id:textbox_email
        Layout.topMargin: 20
        placeholderText: "接收者邮箱"
        Layout.preferredWidth: 300
        disabled:toggle_switch.selected
    }
    FluMultilineTextBox
    {
        id:textbox_code
        Layout.topMargin: 20
        Layout.preferredWidth: 300
        Layout.preferredHeight: 100
        placeholderText: "明文"
        disabled:toggle_switch.selected
        //var orgPasswd = FluMultilineTextBox.text;
    }

    FluFilledButton
    {
                text:"加密&签名"
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: 20
                onClicked:
                {
                    if(textbox_email.text === "")
                    {
                        showError("邮箱不能为空")
                        return
                    }
                    else if(textbox_code.text === "")
                    {
                        showError("明文不能为空")
                        return
                    }
                    else
                    {
                        var text = textbox_code.text//获得他的文本？
                        //appendMessage(true,text)//给这个txt做了什么处理？
                        controller.sendMessage(text)//发送给谁这个文本
                        //textbox.clear()//发送完清空文本
                    }

                    //onResult({password:textbox_password.text})
                   // window.close()
                }
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
            width: Math.min(list_message.width-200,600,implicitWidth)
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



               Rectangle//显示文本
              {
                    id:item_layout_content
                    color: isMy ? "#FF95EC69" : "#FFFFFF"//颜色
                    width: item_msg_loader.width+10
                    height: item_msg_loader.height+10
                    radius: 3
                    Loader
                    {
                        id:item_msg_loader
                        anchors.centerIn: parent
                        sourceComponent: com_text
                    }
                }

            }
        }
    }

    function generateRandomNames(numNames) {
        const alphabet = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';
        const names = [];
        function generateRandomName() {
            const nameLength = Math.floor(Math.random() * 5) + 4;
            let name = '';
            for (let i = 0; i < nameLength; i++) {
                const letterIndex = Math.floor(Math.random() * 26);
                name += alphabet.charAt(letterIndex);
            }
            return name;
        }
        for (let i = 0; i < numNames; i++)
        {
            const name = generateRandomName();
            names.push({title:name});
        }
        return names;
    }

}
