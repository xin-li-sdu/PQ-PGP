pragma Singleton
//左下角关于栏
import QtQuick
import FluentUI

FluObject{
    id:footer_items
    FluPaneItemSeparator{}
    FluPaneItem{
        title:"我的公钥"
        onTap:{
             FluApp.navigate("/login")
        }
    }
    FluPaneItem{
        title:"已注册ID查询"
        onTap:{
             FluApp.navigate("/media")
        }
    }
    FluPaneItem{
        title:"关于"
        onTap:{
            FluApp.navigate("/about")
        }
    }
}
