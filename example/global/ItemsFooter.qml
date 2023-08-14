pragma Singleton
//左下角关于栏
import QtQuick
import FluentUI

FluObject{
    id:footer_items
    FluPaneItemSeparator{}
    FluPaneItem{
        title:"已生成密钥查询"
        onTap:{
             FluApp.navigate("/login")
        }
    }
//    FluPaneItem{
//        title:"意见反馈"
//        onTap:{
//            Qt.openUrlExternally("tencent://message/?uin=2635410120&Site=&Menu=yes")
//        }
//    }
    FluPaneItem{
        title:"关于"
        onTap:{
            FluApp.navigate("/about")
        }
    }
}
