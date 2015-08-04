#include "jsarchitecturalmetricsconfig.h"

#include "ui_jsarchitecturalmetricsconfig.h"

#include <duseinterfaces/iplugincontroller.h>

#include <QtCore/QDir>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonValue>
#include <QtCore/QJsonArray>

#include <QtCore/QDebug>
#include "jsarchitecturalmetricsplugin.h"

namespace DuSE
{

JsArchitecturalMetricsConfig::JsArchitecturalMetricsConfig(QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::JsArchitecturalMetricsConfig)
{
    QDir jsArchitecturalMetricsDir(QCoreApplication::applicationDirPath());

    jsArchitecturalMetricsDir.cdUp();
    jsArchitecturalMetricsDir.cd("src/plugins/jsarchitecturalmetrics/");

    _jsArchitecturalMetricsDir = jsArchitecturalMetricsDir.absolutePath();
    _jsonFileName = "jsarchitecturalmetrics.json";

    _ui->setupUi(this);

    connect(_ui->runPushButton, SIGNAL(clicked()),this, SLOT(runSelectedScript()));
    connect(_ui->setDefaultPushButton, SIGNAL(clicked()), this, SLOT(setMetricDefault()));

}

JsArchitecturalMetricsConfig::~JsArchitecturalMetricsConfig()
{
    delete _ui;
}

QString JsArchitecturalMetricsConfig::scriptNameFileSelected()
{
    int selectedRow = _ui->scriptsMetricsTable->currentRow();

    return _ui->scriptsMetricsTable->item(selectedRow,2)->text();
}

void JsArchitecturalMetricsConfig::loadPanel()
{
    clearMetricsInfo();
    loadMetricsInfo();
    show();
}

void JsArchitecturalMetricsConfig::setMetricDefault()
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

    jsonObject["MetricDefault"] = _ui->metricDefaultComboBox->currentData().toString();
    jsonDocument.setObject(jsonObject);

    jsonFile.resize(0);
    jsonFile.write(jsonDocument.toJson());
    jsonFile.close();
}

void JsArchitecturalMetricsConfig::runSelectedScript()
{
    int selectedRow = _ui->scriptsMetricsTable->currentRow();

    QString scriptFileName = _ui->scriptsMetricsTable->item(selectedRow,2)->text();
    JsArchitecturalMetricsPlugin *js;

    if((js= qobject_cast<JsArchitecturalMetricsPlugin *>(this->parent())))
        if(!js->runScript(scriptFileName))
            qWarning() << "Couldn't run the selected metric.";
}

void JsArchitecturalMetricsConfig::clearMetricsInfo()
{
    while(_ui->scriptsMetricsTable->rowCount()>0) {
        _ui->scriptsMetricsTable->removeRow(0);
    }
    _ui->metricDefaultComboBox->clear();
}

void JsArchitecturalMetricsConfig::loadMetricsInfo()
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
