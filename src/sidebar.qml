import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

Item {
    id: sidebar
    signal enterClicked()
    signal leaveClicked()
    signal herwClicked()
    signal buyWeapon(int slot, string weapon)

    function healthChanged(player, hp) {
        playerSquareModel.setProperty(player, "hp", hp)
    }

    function weaponChanged(player, weapon) {
        playerSquareModel.setProperty(player, "weapon", weapon)
    }

    function cashChanged(cash) {
        console.log("Cash changed")
        shoppingGrid.cash = cash;
    }

    function logMessage(message) {
        logModel.append({"message": qsTr(message)})
        logView.positionViewAtEnd()
    }

    width: 320
    height: 720

    ColumnLayout {
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width

        ListModel {
            id: playerSquareModel
            ListElement { hp: 100; weapon: "puukko" }
            ListElement { hp: 100; weapon: "puukko" }
            ListElement { hp: 100; weapon: "puukko" }
            ListElement { hp: 100; weapon: "puukko" }
        }

        GridLayout {
            Layout.fillWidth: true

            id: playergrid
            columns: 2
            
            Repeater {
                model: playerSquareModel
                delegate: Rectangle {
                    width: 150
                    height: 75

                    Rectangle {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: { parent.width - 25 }
                        height: { parent.height - 35 }
                        color: {
                            if (hp > 40) "green";
                            else if (hp > 20) "orange";
                            else if (hp > 0) "red";
                            else "darkred"
                        }

                        Label {
                            anchors.baseline: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: hp
                        }
                    }

                    Label {
                        anchors.bottom: parent.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: weapon
                    }
                }
            }

        }

        GridLayout {
            id: shoppingGrid
            visible: enterButton.enabled
            enabled: enterButton.enabled
            property int cash: 400

            columns: 2

            Row {
                Layout.columnSpan: 2
                Label {
                    text: qsTr("Rahat: ")
                }

                Label {
                    text: shoppingGrid.cash
                }
            }

            ListModel {
                id: weaponPurchaseModel

                ListElement { slot: 0; name: qsTr("Pelaaja") }
                ListElement { slot: 1; name: qsTr("Apuri 1") }
                ListElement { slot: 2; name: qsTr("Apuri 2") }
                ListElement { slot: 3; name: qsTr("Apuri 3") }
            }


            Column {
                Label {
                    text: qsTr("Pistooli - 100€")
                }

                id: pistolColumn
                Grid {
                    id: pistolGrid
                    columns: 2

                    Repeater {
                        model: weaponPurchaseModel
                        delegate: Button {
                            text: name
                            onClicked: {
                                sidebar.buyWeapon(slot, "pistol")
                            }
                        }
                    }
                }
            }

            Column {
                Label {
                    text: qsTr("Konepistooli - 250€")
                }

                id: smgColumn
                Grid {
                    id: smgGrid
                    columns: 2

                    Repeater {
                        model: weaponPurchaseModel
                        delegate: Button {
                            text: name
                            onClicked: {
                                sidebar.buyWeapon(slot, "smg")
                            }
                        }
                    }
                }
            }

            Column {
                Label {
                    text: qsTr("Haulikko - 500€")
                }

                id: shotgunColumn
                Grid {
                    id: shotgunGrid
                    columns: 2

                    Repeater {
                        model: weaponPurchaseModel
                        delegate: Button {
                            text: name
                            onClicked: {
                                sidebar.buyWeapon(slot, "shotgun")
                            }
                        }
                    }
                }
            }

            Column {
                Label {
                    text: qsTr("Kivääri - 300€")
                }

                id: rifleColumn
                Grid {
                    id: rifleGrid
                    columns: 2

                    Repeater {
                        model: weaponPurchaseModel
                        delegate: Button {
                            text: name
                            onClicked: {
                                sidebar.buyWeapon(slot, "rifle")
                            }
                        }
                    }
                }
            }
        }
        
        Grid {
            id: buttongrid
            Button {
                id: enterButton
                objectName: "enterButton" // needed to find from c++
                onClicked: sidebar.enterClicked()
                text: qsTr("Siirry halliin")
            }

            Button {
                id: leaveButton
                objectName: "leaveButton"
                onClicked: sidebar.leaveClicked()
                text: qsTr("Poistu hallista")
                enabled: false
            }

            Button {
                id: herwButton
                objectName: "herwButton"
                onClicked: sidebar.herwClicked()
                text: qsTr("Enter Herwood")
                enabled: false
            }
        }

        ListModel {
            id: logModel
            ListElement {
                message: qsTr("Peli alkaa")
            }
        }

        ScrollView {
            Layout.fillWidth: true
            verticalScrollBarPolicy: Qt.ScrollBarAlwaysOn
            ListView {
                id: logView
                model: logModel
                width: parent.width
                delegate: Row {
                    Text {
                        width: logView.width
                        wrapMode: Text.Wrap
                        text: qsTr(message)
                    }
                }
            }
        }
    }
}
