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
#include "jsarchitecturalmetricsplugin.h"

#include <duseinterfaces/iuicontroller.h>

#include <QtCore/QFile>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonValue>
#include <QtCore/QCoreApplication>
#include <QtWidgets/QAction>

namespace DuSE
{

JsArchitecturalMetricsPlugin::JsArchitecturalMetricsPlugin(QObject *parent) :
    IJsPlugin(parent),
    _jsArchitecturalMetrics(new JsArchitecturalMetrics)
{
}

JsArchitecturalMetricsPlugin::~JsArchitecturalMetricsPlugin()
{
    delete _jsArchitecturalMetrics;
}

bool JsArchitecturalMetricsPlugin::initialize()
{

    QFile jsonFile(_jsArchitecturalMetrics->_jsArchitecturalMetricsDir+"/"+_jsArchitecturalMetrics->_jsonFileName);
    QString contentJsonFile;

    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    contentJsonFile = jsonFile.readAll();
    jsonFile.close();

    QJsonDocument jsonDocument = QJsonDocument::fromJson(contentJsonFile.toUtf8());
    QJsonObject jsonObject = jsonDocument.object();

    QString menuName = jsonObject.value(QString("MenuName")).toString();
    QString toolbarName = jsonObject.value(QString("ToolbarName")).toString();
    QString iconName = jsonObject.value(QString("IconName")).toString();

    QAction *actionLoadPanel = new QAction(QIcon::fromTheme(iconName), tr("Architectural Metrics..."), this);
    connect(actionLoadPanel, SIGNAL(triggered()), _jsArchitecturalMetrics, SLOT(loadPanel()));
    ICore::self()->uiController()->addAction(actionLoadPanel, menuName);

    QAction *actionRunScript = new QAction(QIcon::fromTheme(iconName), tr("Run Architectural Metric"), this);
    connect(actionRunScript, SIGNAL(triggered()), _jsArchitecturalMetrics, SLOT(runDefaultScript()));
    ICore::self()->uiController()->addAction(actionRunScript, "", toolbarName);

    return true;
}

}
