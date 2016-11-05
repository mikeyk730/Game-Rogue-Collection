import QtQuick 2.4
import QtQuick.Dialogs 1.0
import Rogue 1.0

Item {
    id: root

    property alias monochrome: rogue.monochrome
    property alias font: rogue.font
    property alias screenSize: rogue.screenSize
    property alias fontSize: rogue.fontSize
    property alias title: rogue.title
    property Item activeItem: gameSelect

    signal rendered

    width: activeItem.width
    height: activeItem.height

    function setGraphics(graphics){
        rogue.setGraphics(graphics);
    }

    GameSelect {
        id: gameSelect

        z: 2
        width: 640
        height: 400
        anchors.centerIn: parent

        onRestore: {
            console.log(savefile);
            rogue.restoreGame(savefile);

            activeItem = rogue;
            activeItem.visible = true;
            activeItem.focus = true;
            gameSelect.visible = false;
        }

        function onGameSet()
        {
            if (rogue.game === "PC Rogue 1.48"){
                activeItem = titleScreen;
            }
            else{
                activeItem = rogue;
            }
            activeItem.visible = true;
            activeItem.focus = true;
            gameSelect.visible = false;
        }

        onSelected: {
            console.log(game);
            rogue.game = game;
            onGameSet();
        }

        Component.onCompleted: {
            if (rogue.game)
                onGameSet();
        }
    }

    RogueTitle {
        id: titleScreen
        visible: false

        z: 1
        width: 640
        height: 400
        anchors.centerIn: parent

        onDismissed: {
            activeItem = rogue;
            activeItem.visible = true;
            activeItem.focus = true;
            titleScreen.visible = false;
        }
    }

    QmlRogue {
        id: rogue
        visible: false

        onRendered: root.rendered()

        property string title: "Rogue Collection v2.0" + (rogue.game ? ' - ' + rogue.game : '')

        z: 0
        width: screenSize.width * fontSize.width
        height: screenSize.height * fontSize.height
        anchors.centerIn: parent

        //font.family: 'Consolas'
        //font.pixelSize: 16
        smooth: false

        Keys.onPressed: {
            if ((event.modifiers & Qt.ControlModifier) && (event.key === Qt.Key_S))
            {
                saveDialog.open();
                event.accepted = true;
            }
        }

        FileDialog {
            id: saveDialog
            title: "Please choose a file to save"
            nameFilters: [ "Saved games (*.sav)", "All files (*)" ]
            selectMultiple: false
            selectExisting: false

            onAccepted: {
                var path = saveDialog.fileUrl.toString();
                path = path.replace(/^(file:\/{3})/,"");
                rogue.saveGame(path);
            }
        }
    }
}
