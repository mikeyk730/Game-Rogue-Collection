import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2

Window{
    id: dialogwindow
    title: qsTr("About")
    width: 830
    height: 500

    modality: Qt.ApplicationModal

    onClosing: mainContent.state = "Default";

    ColumnLayout{
        anchors.fill: parent
        anchors.margins: 15
        spacing: 15
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Rogue Collection"
            font {bold: true; pointSize: 16}
        }
        Loader{
            id: mainContent
            Layout.fillHeight: true
            Layout.fillWidth: true

            states: [
                State {
                    name: "Default"
                    PropertyChanges {
                        target: mainContent
                        sourceComponent: defaultComponent
                    }
                },
                State {
                    name: "License"
                    PropertyChanges {
                        target: mainContent
                        sourceComponent: licenseComponent
                    }
                }
            ]
            Component.onCompleted: mainContent.state = "Default";
        }
        Item{
            Layout.fillWidth: true
            height: childrenRect.height
            Button{
                anchors.left: parent.left
                text: qsTr("License")
                onClicked: {
                    mainContent.state == "Default" ? mainContent.state = "License" : mainContent.state = "Default"
                }
            }
            Button{
                anchors.right: parent.right
                text: qsTr("Close")
                onClicked: dialogwindow.close();
            }
        }
    }
    // MAIN COMPONENTS ////////////////////////////////////////////////////////
    Component{
        id: defaultComponent
        ColumnLayout{
            anchors.fill: parent
            spacing: 10
            Text{
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignHCenter
                text: "Exploring the Dungeons of Doom\n\n" +

                      qsTr("Version: ") + "2.0\n" +
                      qsTr("Author: ") + "Mike Kaminski\n" +
                      qsTr("Email: ")  + "mikeyk730@gmail.com\n" +
                      qsTr("Source: ") + "https://github.com/mikeyk730/Rogue-Collection\n\n" +

                      'Rogue:\n' +
                      "Copyright (C) 1981 Michael Toy, Ken Arnold, and Glenn Wichman\n" +
                      "Copyright (C) 1983 Jon Lane (A.I. Design update for the IBM PC)\n" +
                      "Copyright (C) 1985 Epyx\n" +
                      "Credit to Roguelike Restoration Project\n"
            }
        }
    }
    Component{
        id: licenseComponent
        TextArea{
            anchors.fill: parent
            readOnly: true
            text: "Copyright (C) 2016 Mike Kaminski <mikeyk730@gmail.com>\n" +
                  "https://github.com/mikeyk730/Rogue-Collection\n\n" +

                  "Rogue Collection is free software: you can redistribute it and/or modify " +
                  "it under the terms of the GNU General Public License as published by " +
                  "the Free Software Foundation, either version 3 of the License, or " +
                  "(at your option) any later version.\n\n" +

                  "This program is distributed in the hope that it will be useful, " +
                  "but WITHOUT ANY WARRANTY; without even the implied warranty of " +
                  "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the " +
                  "GNU General Public License for more details.\n\n" +

                  "You should have received a copy of the GNU General Public License " +
                  "along with this program.  If not, see <http://www.gnu.org/licenses/>."
        }
    }
}