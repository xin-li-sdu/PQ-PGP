import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import FluentUI

FluWindow {

    width: 500
    height: 600
    minimumWidth: 500
    minimumHeight: 600
    maximumWidth: 500
    maximumHeight: 600

    title:"关于"

    FluAppBar{
        id:appbar
        title:"关于"
        width:parent.width
    }

    ColumnLayout{
        anchors{
            top: appbar.bottom
            left: parent.left
            right: parent.right
        }

        RowLayout{
            Layout.topMargin: 20
            Layout.leftMargin: 15
            spacing: 14
            FluText{
                text:" PQ-PGP：后量子时代的PGP"
                fontStyle: FluText.Title
            }
            FluText{
                text:"v1.01"
                fontStyle: FluText.Body
                Layout.alignment: Qt.AlignBottom
            }
        }

        RowLayout{
            spacing: 14
            Layout.topMargin: 20
            Layout.leftMargin: 15
            FluText{
                text:"项目团队："
            }
            FluText{
                text:"PQPGP"
                Layout.alignment: Qt.AlignBottom
            }
        }

        RowLayout{
            spacing: 14
            Layout.topMargin: 20
            Layout.leftMargin: 15
            FluText{
                text:"作品简介："
            }


     }
        RowLayout
        {
         spacing: 14
         Layout.topMargin: 20
         Layout.leftMargin: 15
         FluText{
             id:text_hublink2
             text:"现如今流行的电子邮件加密软件PGP可为数据通信提供机密性和认证性.然而世界正
在向量子时代前进,Shor算法/Grover算法等量子算法的提出,使得当前PGP采用的 R
SA/ELGamal公钥密码体制不再安全.针对于此,我们设计并实现了首个后量子 PGP :
PQ-PGP。底层基于GPV方案实现了MNRU格上的IBE，然后经Naor变换和Fujisak
i-Okamoto变换转换为ISE，加密和签名共用一对密钥。该系统具有强安全、低开销、
高速度、易操作等优势，适用于一切需要为数据通信提供机密性和认证性的场景。"

         }
       }

    }
}
