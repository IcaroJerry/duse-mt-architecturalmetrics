/****************************************************************************
**
** Copyright (C) 2013 Sandro S. Andrade <sandroandrade@kde.org>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtUml module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
import QtQuick 2.0

UmlElement {
    UmlSlot {
        id: nameSlot
        anchors.top: parent.top
        Text {
            id: label
            text: element.name
            anchors.centerIn: parent
            font { family: "Korolev"; italic: element.isAbstract }
        }
    }
    UmlSlot {
        id: attributeSlot
        anchors { top: nameSlot.bottom; topMargin: -1 }
        height: (parent.height - nameSlot.height)/2
        ListView {
            model: element.ownedAttributes
            anchors { fill: parent; margins: 4 }
            delegate: Text {
                text: visibility(modelData.visibility) + modelData.name + ": " + (modelData.type ? modelData.type.name:"<no type>")
                font { family: "Korolev" }
            }
        }
    }
    UmlSlot {
        anchors { top: attributeSlot.bottom; topMargin: -1; bottom: parent.bottom }
        ListView {
            model: element.ownedOperations
            anchors { fill: parent; margins: 4 }
            delegate: Text {
                text: visibility(modelData.visibility) + modelData.name + operationSignature(model)
                font { family: "Korolev" }
            }
        }
    }
    function visibility(visibilityEnum)
    {
        switch (visibilityEnum) {
        case 0: return " "
        case 1: return "+"
        case 2: return "-"
        case 3: return "#"
        case 4: return "~"
        }
    }
    function operationSignature(model)
    {
        var generalString = "(";
        var returnString = ": ";
        var list = element.ownedOperations[model.index].ownedParameters;
        var count = list.length;
        for (var i = 0; i < count ; ++i)
        {
            if (list[i].direction === 4)
                returnString = returnString + list[i].type.name;
            else
            generalString = generalString + list[i].type.name + ", ";
        }
        if (returnString == ": ")
            returnString = returnString + "void";
        generalString = generalString + ")" + returnString;
        generalString = generalString.replace(', )',')');
        return generalString;
    }
}
