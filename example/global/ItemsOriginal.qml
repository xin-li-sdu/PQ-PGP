pragma Singleton

import QtQuick
import FluentUI


FluObject{

    property var navigationView
    FluPaneItem
    {
        title:"项目介绍"
        icon:FluentIcons.Home
        onTap:{
            navigationView.push("qrc:/T_Home.qml")
        }
    }

    FluPaneItem
   {
        title:"密钥生成"
        icon:FluentIcons.MobileLocked
        onTap:
        {

            //registerForPageResult("/login")
            navigationView.push("qrc:/page/keygen.qml")
            //FluApp.navigate("/login")
            }
    }

    FluPaneItemExpander
    {
        title:"邮件加密&签名"
        //icon:FluentIcons.CheckboxComposite
        icon:FluentIcons.MailForward
        FluPaneItem
        {
            image:"qrc:/res/image/control/BreadcrumbBar.png"
            recentlyAdded:true
            order:9
            desc:"后量子的加密签名算法，可以对明文进行加密同时对密文进行签名. "
            title:"加密&签名"
            icon:FluentIcons.PLAP
            onTap:
            {
            navigationView.push("qrc:/page/encsign.qml")
            }
           }
        FluPaneItem
        {
            image:"qrc:/res/image/control/Button.png"
            desc:"加密体制基于后量子算法的加密系统,保障量子计算威胁下信道安全."
            recentlyAdded:true
            order:5
            title:"加密"
            icon:FluentIcons.ProtectedDocument
            onTap:
            {
            navigationView.push("qrc:/page/enc.qml")
            }
           }

        FluPaneItem
        {
            title:"签名"
            image:"qrc:/res/image/control/InkCanvas.png"

            desc:"基于后量子密码算法的签名系统，可以对消息进行签名. "
            recentlyAdded:true
            order:7
            icon:FluentIcons.InkingTool
            onTap:
            {
            navigationView.push("qrc:/page/sign.qml")
            }
           }
    }

    FluPaneItemExpander
    {
        title:"邮件认证&解密"
        icon:FluentIcons.LeaveChat
        FluPaneItem
        {
            title:"认证&解密"
            image:"qrc:/res/image/control/RichTextBlock.png"
            desc:"与加密&签名相对，可以对密文进行验证并解密."
            recentlyAdded:true
            order:8
            icon:FluentIcons.Unlock
            onTap:
            {
            navigationView.push("qrc:/page/decvfy.qml")
            }
           }

        FluPaneItem
        {
            title:"解密"
            image:"qrc:/res/image/control/PasswordBox.png"
            desc:"对应于加密，将密文恢复为明文. "
            recentlyAdded:true
            order:4
              icon:FluentIcons.Page
            onTap:
            {
            navigationView.push("qrc:/page/dec.qml")
            }
           }
        FluPaneItem
        {
            title:"认证"
            image:"qrc:/res/image/control/InputValidation.png"
            desc:"对应与签名，验证消息真实性. "
            recentlyAdded:true
            order:6
            icon:FluentIcons.Fingerprint
            onTap:
            {
                navigationView.push("qrc:/page/vfgtest.qml")
           }
        }

    }



   FluPaneItemExpander
{
        title:"内置邮箱"
        icon:FluentIcons.ButtonMenu
        FluPaneItem
        {
            image:"qrc:/res/image/control/ContentDialog.png"
            desc:"一键完成邮件编辑-加密-发送全流程."
            recentlyAdded:true
            order:2
            title:"邮件加密并发送"
            icon:FluentIcons.ProtectedDocument
            onTap:
            {
            navigationView.push("qrc:/page/send_enc.qml")
            }
           }
        FluPaneItem
        {
            image:"qrc:/res/image/control/Flyout.png"
            desc:"一键完成邮件接收-解码-解密全流程."
            recentlyAdded:true
            order:1
            title:"邮件接收并解密"
            icon:FluentIcons.ProtectedDocument
            onTap:
            {
            navigationView.push("qrc:/page/dec_get.qml")
            }
           }
    }

//    FluPaneItemExpander{
//        title:"功能测试"
//        icon:FluentIcons.AllApps

//        FluPaneItem{
//            title:"图表test"
//           // icon:FluentIcons.AllApps
//            image:"qrc:/res/image/control/TabView.png"
//            recentlyAdded:true
//            order:1
//            desc:"This is a meaningless English paragraph. "
//            onTap:{
//                navigationView.push("qrc:/T_TabView.qml")
//            }
//        }
//        FluPaneItem{
//            title:"文件测试"
//           // icon:FluentIcons.AllApps
//            image:"qrc:/res/image/control/TabView.png"
//            recentlyAdded:true
//            order:1
//            desc:"This is a meaningless English paragraph. "
//            onTap:{
//                navigationView.push("qrc:/T_Progress.qml")
//            }
//        }
//        FluPaneItem{
//            icon:FluentIcons.Media
//            title:"视频演示"
//            image:"qrc:/res/image/control/MediaPlayerElement.png"
//            recentlyAdded:true
//            order:0
//            desc:"软件操作演示视频."
//            onTap:{
//                navigationView.push("qrc:/T_MediaPlayer.qml")
//            }
//        }
//      }

      /*   FluPaneItem{
            title:"MultiWindow"
            onTap:{
                navigationView.push("qrc:/T_MultiWindow.qml")
            }
        }
        FluPaneItem{
            title:"FlipView"
            image:"qrc:/res/image/control/FlipView.png"
            recentlyAdded:true
            order:2
            desc:"Presents a collection of items that the user canflip through, one item at a time."
            onTap:{
                navigationView.push("qrc:/T_FlipView.qml")
            }
        }
    }
*/
//    FluPaneItemExpander
//{
//        title:"Theming"
//        icon:FluentIcons.Brightness
//        FluPaneItem
//        {
//            title:"Theme"
//            onTap:{
//                navigationView.push("qrc:/T_Theme.qml")
//            }
//        }

//        FluPaneItem{
//            title:"Awesome"
//            onTap:{
//                navigationView.push("qrc:/T_Awesome.qml")
//            }
//        }
//        FluPaneItem{
//            title:"Typography"
//            onTap:{
//                navigationView.push("qrc:/T_Typography.qml")
//            }
//        }
//    }



    function getRecentlyAddedData(){
        var arr = []
        for(var i=0;i<children.length;i++)
        {
            var item = children[i]
            if(item instanceof FluPaneItem && item.recentlyAdded)
            {
                arr.push(item)
            }
            if(item instanceof FluPaneItemExpander)
            {
                for(var j=0;j<item.children.length;j++)
                {
                    var itemChild = item.children[j]
                    if(itemChild instanceof FluPaneItem && itemChild.recentlyAdded)
                    {
                        arr.push(itemChild)
                    }
                }
            }
        }
        arr.sort(function(o1,o2){ return o2.order-o1.order })
        return arr
    }

    function getRecentlyUpdatedData(){
        var arr = []
        var items = navigationView.getItems();
        for(var i=0;i<items.length;i++)
        {
            var item = items[i]
            if(item instanceof FluPaneItem && item.recentlyUpdated)
            {
                arr.push(item)
            }
        }
        return arr
    }

    function getSearchData(){
        var arr = []
        var items = navigationView.getItems();
        for(var i=0;i<items.length;i++){
            var item = items[i]
            if(item instanceof FluPaneItem){
                arr.push({title:item.title,key:item.key})
            }
        }
        return arr
    }

    function startPageByItem(data){
        var items = navigationView.getItems();
        for(var i=0;i<items.length;i++){
            var item =  items[i]
            if(item.key === data.key){
                if(navigationView.getCurrentIndex() === i){
                    return
                }
                item.tap()
                navigationView.setCurrentIndex(i)
                if(item.parent){
                    item.parent.isExpand = true
                }
                return
            }
        }
    }

}

