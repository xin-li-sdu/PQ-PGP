﻿import QtQuick
import FluentUI

QtObject {
    readonly property string key : FluApp.uuid()
    readonly property int flag : 0
    property string title
    property int order : 0
    property int icon
    property bool recentlyAdded: false
    property bool recentlyUpdated: false
    property string desc
    property var image
    property var parent
    property int idx
    signal tap
    signal repTap
}
