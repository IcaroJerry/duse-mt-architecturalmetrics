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
#include "jsarchitecturalmetrics.h"
#include "ui_jsarchitecturalmetrics.h"

#include <duseinterfaces/iplugincontroller.h>

#include <QtCore/QDir>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonValue>
#include <QtCore/QJsonArray>

#include <QtCore/QDebug>

namespace DuSE
{

JsArchitecturalMetrics::JsArchitecturalMetrics(QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::JsArchitecturalMetrics)
{
    QDir jsArchitecturalMetricsDir(QCoreApplication::applicationDirPath());

    jsArchitecturalMetricsDir.cdUp();
    jsArchitecturalMetricsDir.cd("src/plugins/jsarchitecturalmetrics/");

    _jsArchitecturalMetricsDir = jsArchitecturalMetricsDir.absolutePath();
    _jsonFileName = "jsarchitecturalmetrics.json";

    _ui->setupUi(this);

    connect(_ui->runPushButton, SIGNAL(clicked()), this, SLOT(runScript()));
    connect(_ui->setDefaultPushButton, SIGNAL(clicked()), this, SLOT(setMetricDefault()));

}

JsArchitecturalMetrics::~JsArchitecturalMetrics()
{
    delete _ui;
}

void JsArchitecturalMetrics::loadPanel()
{
    loadMetricsInfo();
    show();
}

void JsArchitecturalMetrics::setMetricDefault()
{
    QFile jsonFile(_jsArchitecturalMetricsDir+"/"+_jsonFileName);
    QString contentJsonFile;

    if (!jsonFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
            qWarning("Couldn't open json file.");
            return;
    }

    contentJsonFile = jsonFile.readAll();

    QJsonDocument jsonDocument = QJsonDocument::fromJson(contentJsonFile.toUtf8());
    QJsonObject jsonObject = jsonDocument.object();
    int selectRow = _ui->scriptsMetricsTable->SelectRows;

    jsonObject["MetricDefault"] = _ui->scriptsMetricsTable->item(selectRow, 2)->text();

    QString test = jsonObject["MetricDefault"].toString();
    qDebug() << test;

    jsonFile.close();
}

bool JsArchitecturalMetrics::runScript()
{
    QAction *action;

    if ((action = qobject_cast<QAction *>(sender()))) {
       QString scriptDir = action->data().toString();

       QFile scriptFile(scriptDir);
       QString contentScriptFile;

       scriptFile.open(QIODevice::ReadOnly | QIODevice::Text);
       contentScriptFile = scriptFile.readAll();
       scriptFile.close();

       qDebug() << contentScriptFile;
    }

    return true;
}

void JsArchitecturalMetrics::loadMetricsInfo()
{
    _ui->scriptsMetricsTable->setColumnWidth(1, 240);
    _ui->scriptsMetricsTable->setColumnWidth(2, 110);

    QFile jsonFile(_jsArchitecturalMetricsDir+"/"+ _jsonFileName);
    QString contentJsonFile;

    if (!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning("Couldn't open json file.");
            return;
    }

    contentJsonFile = jsonFile.readAll();
    jsonFile.close();

    QJsonDocument jsonDocument = QJsonDocument::fromJson(contentJsonFile.toUtf8());
    QJsonObject jsonObject = jsonDocument.object();

    QString scriptMetricDefault = jsonObject.value("MetricDefault").toString();

    QJsonArray scriptsList = jsonObject.value(QString("ScriptsFile")).toArray();

    for(int i = 0; i < scriptsList.size(); i++) {

        QString metricName;
        QString shortDescription;
        QString scriptFile;

        metricName = scriptsList.at(i).toObject().value("MetricName").toString();
        shortDescription = scriptsList.at(i).toObject().value("ShortDescription").toString();
        scriptFile = scriptsList.at(i).toObject().value("ScriptFile").toString();

        _ui->scriptsMetricsTable->insertRow(0);

        _ui->scriptsMetricsTable->setItem(0, 0, new QTableWidgetItem(metricName));
        _ui->scriptsMetricsTable->setItem(0, 1, new QTableWidgetItem(shortDescription));
        _ui->scriptsMetricsTable->setItem(0, 2, new QTableWidgetItem(scriptFile));

        _ui->metricDefaultComboBox->addItem(metricName,QVariant::fromValue(scriptFile));

        if(scriptFile == scriptMetricDefault){
            _ui->scriptsMetricsTable->selectRow(0);
            _ui->metricDefaultComboBox->setCurrentIndex(i);
        }
    }
}

}
