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
#include "concretesyntaxviewplugin.h"

#include <duseinterfaces/iuicontroller.h>
#include <duseinterfaces/iprojectcontroller.h>

#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>

#include "private/qquickflickable_p.h"

#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>

#include <QtWidgets/QWidget>

#include <QtCore/QDebug>
#include <QtCore/QRegularExpression>

namespace DuSE
{

ConcreteSyntaxViewPlugin::ConcreteSyntaxViewPlugin(QObject *parent) :
    IPlugin(parent),
    _concreteSyntaxQuickView(new QQuickView)
{
}

bool ConcreteSyntaxViewPlugin::initialize()
{
    _concreteSyntaxQuickView->setResizeMode(QQuickView::SizeRootObjectToView);
    ICore::self()->uiController()->addCentralWidgetTab(QWidget::createWindowContainer(_concreteSyntaxQuickView), "Concrete Syntax");
    initializeQuickView();

    connect(ICore::self()->uiController(), &IUiController::addToView, this, &ConcreteSyntaxViewPlugin::addToView);
    connect(ICore::self()->projectController(), SIGNAL(modelAboutToBeClosed(QList<QModelingObject *>)), this, SLOT(initializeQuickView()));

    return true;
}

void ConcreteSyntaxViewPlugin::initializeQuickView()
{
    _concreteSyntaxQuickView->setSource(QUrl("qrc:/concretesyntaxview/concretesyntaxview.qml"));
}

void ConcreteSyntaxViewPlugin::addToView(QObject *selectedModelingObject, QQuickItem *parent)
{
    QQmlContext *context = new QQmlContext(_concreteSyntaxQuickView->engine()->rootContext());
    context->setContextProperty("element", selectedModelingObject);
    QQmlComponent *qmlComponent = new QQmlComponent(_concreteSyntaxQuickView->engine());
    int x = qrand() % 400;
    int y = qrand() % 400;
    qmlComponent->setData(QString("import QtQuick 2.0\nimport QtModeling.Uml 1.0\n\n%1 { x: %2; y: %3; objectName: \"%4\" }").arg(QString(selectedModelingObject->metaObject()->className()).remove(QRegularExpression("^Q")).remove(QRegularExpression("Object$"))).arg(x).arg(y).arg(selectedModelingObject->objectName()).toLatin1(), QUrl());

    if (qmlComponent->isError()) {
        qWarning() << qmlComponent->errors();
    } else {
        QQuickItem *item = qobject_cast<QQuickItem *>(qmlComponent->beginCreate(context));
        if (item) {
            item->setParentItem(parent ? parent:(qobject_cast<QQuickFlickable *>(_concreteSyntaxQuickView->rootObject()))->contentItem());
            qmlComponent->completeCreate();
        }
    }

//    _concreteSyntaxQuickView->rootObject()->dumpObjectTree();
    qmlComponent->deleteLater();
}

}

