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

#include <QtCore/QDir>
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
    QDir jsArchitecturalMetricsDir(QCoreApplication::applicationDirPath());

    jsArchitecturalMetricsDir.cdUp();
    jsArchitecturalMetricsDir.cd("src/plugins/jsarchitecturalmetrics/");

    QFile jsonFile(jsArchitecturalMetricsDir.absoluteFilePath("jsarchitecturalmetrics.json"));
    QString contentJsonFile;

    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    contentJsonFile = jsonFile.readAll();
    jsonFile.close();

    QJsonDocument jsonDocument = QJsonDocument::fromJson(contentJsonFile.toUtf8());
    QJsonObject jsonObject = jsonDocument.object();

    QString menuName = jsonObject.value(QString("MenuName")).toString();
    QString toolbarName = jsonObject.value(QString("ToolbarName")).toString();
    QString iconName = jsonObject.value(QString("IconName")).toString();
    QString scriptFile = jsonObject.value(QString("ScriptFile")).toString();

    QAction *actionLoadPanel = new QAction(QIcon::fromTheme(iconName), tr("Open the panel of Architectural Metrics"), this);
    connect(actionLoadPanel, SIGNAL(triggered()), _jsArchitecturalMetrics, SLOT(loadPanel()));
    ICore::self()->uiController()->addAction(actionLoadPanel, menuName);

    QAction *actionRunScript = new QAction(QIcon::fromTheme(iconName), tr("Run Architectural Metric"), this);
    actionRunScript->setData(QVariant::fromValue(scriptFile));
    connect(actionRunScript, SIGNAL(triggered()), _jsArchitecturalMetrics, SLOT(runScript()));
    ICore::self()->uiController()->addAction(actionRunScript, "", toolbarName);

    return true;
}

}
/*
bool JsArchitecturalMetricsPlugin::initialize()
{
    QDir jsArchitecturalMetricsDir(QCoreApplication::applicationDirPath());

    jsArchitecturalMetricsDir.cdUp();
    jsArchitecturalMetricsDir.cd("src/plugins/jsarchitecturalmetrics/");

    QFile jsonFile(jsArchitecturalMetricsDir.absoluteFilePath("jsarchitecturalmetrics.json"));
    QString contentJsonFile;

    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    contentJsonFile = jsonFile.readAll();
    jsonFile.close();

    QJsonDocument jsonDocument = QJsonDocument::fromJson(contentJsonFile.toUtf8());
    QJsonObject jsonObject = jsonDocument.object();

    //QWidget *scriptsMetricsWidget = new QWidget;
    //_scriptsMetrics->setupUi(scriptsMetricsWidget);

    QString menuName = jsonObject.value(QString("MenuName")).toString();
    QString toolbarName = jsonObject.value(QString("ToolbarName")).toString();
    QString iconName = jsonObject.value(QString("IconName")).toString();
    QString scriptFile = jsonObject.value(QString("ScriptFile")).toString();

    QAction *actionRunScript = new QAction(QIcon::fromTheme(iconName), "", 0);
    actionRunScript->setData(QVariant::fromValue(scriptFile));
    connect(actionRunScript, &QAction::triggered, this, &JsArchitecturalMetricsPlugin::scriptsMetrics);
    ICore::self()->uiController()->addAction(actionRunScript, "", toolbarName);

    QAction *actionOpenConfig = new QAction(QIcon::fromTheme(iconName), "", 0);
    actionOpenConfig->setData(QVariant::fromValue(scriptFile));
    connect(actionOpenConfig, &QAction::triggered, this, &JsArchitecturalMetricsPlugin::runScript);
    ICore::self()->uiController()->addAction(actionOpenConfig, menuName, "");

   return true;
}

void JsArchitecturalMetricsPlugin::scriptsMetrics()
{
    _scriptsMetricsDialog->exec();
}

bool JsArchitecturalMetricsPlugin::runScript()
{

 QAction *action;

 if ((action = qobject_cast<QAction *>(sender()))) {
    QString scriptFileName = action->data().toString();
 }
 return true;
}

}
*/
