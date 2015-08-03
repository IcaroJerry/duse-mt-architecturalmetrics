#include "jsarchitecturalmetricsplugin.h"

#include "ui_jsarchitecturalmetrics.h"

#include <duseinterfaces/iuicontroller.h>
#include <duseinterfaces/iprojectcontroller.h>

#include <QtModeling/QModelingObject>

#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValueIterator>

#include <QtGui/QKeyEvent>

#include <QtWidgets/QListView>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QAction>

#include <QtCore/QTextStream>
#include <QtCore/QStringListModel>
#include <QtCore/QFile>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonValue>
#include <QtCore/QCoreApplication>

#include <QtCore/QDebug>

template <class T>
QScriptValue qSetToScriptValue(QScriptEngine *engine, const QSet<T *> &elements)
{
    QScriptValue array = engine->newArray();
    foreach (T *element, elements)
        array.property(QString::fromLatin1("push")).call(array, QScriptValueList() << engine->newQObject(element));
    return array;
}

template <class T>
void scriptValueToQSet(const QScriptValue &obj, QSet<T *> &elements)
{
    QScriptValueIterator it(obj);
    while (it.hasNext()) {
        it.next();
        elements.insert(qobject_cast<T *>(it.value().toQObject()));
    }
}

template <class T>
QScriptValue qListToScriptValue(QScriptEngine *engine, const QList<T *> &elements)
{
    QScriptValue array = engine->newArray();
    foreach (T *element, elements)
        array.property(QString::fromLatin1("push")).call(array, QScriptValueList() << engine->newQObject(element));
    return array;
}

template <class T>
void scriptValueToQList(const QScriptValue &obj, QList<T *> &elements)
{
    QScriptValueIterator it(obj);
    while (it.hasNext()) {
        it.next();
        elements.append(qobject_cast<T *>(it.value().toQObject()));
    }
}

namespace DuSE
{

JsArchitecturalMetricsPlugin::JsArchitecturalMetricsPlugin(QObject *parent) :
    IPlugin(parent),
    _jsArchitecturalMetricsConfig(new JsArchitecturalMetricsConfig),
    _jsArchitecturalMetrics(new Ui::JsArchitecturalMetrics),
    _codeCompletionView(new QListView),
    _engine(0)
{
}

JsArchitecturalMetricsPlugin::~JsArchitecturalMetricsPlugin()
{
    delete _jsArchitecturalMetricsConfig;
    delete _jsArchitecturalMetrics;
}

bool JsArchitecturalMetricsPlugin::initialize()
{

    QFile jsonFile(_jsArchitecturalMetricsConfig->_jsArchitecturalMetricsDir+"/"+_jsArchitecturalMetricsConfig->_jsonFileName);
    QString contentJsonFile;

    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    contentJsonFile = jsonFile.readAll();
    jsonFile.close();

    QJsonDocument jsonDocument = QJsonDocument::fromJson(contentJsonFile.toUtf8());
    QJsonObject jsonObject = jsonDocument.object();

    QString menuName = jsonObject.value(QString("MenuName")).toString();
    QString toolbarName = jsonObject.value(QString("ToolbarName")).toString();
    QString iconName = jsonObject.value(QString("IconName")).toString();
    QString metricDefault = jsonObject.value(QString("MetricDefault")).toString();

    QAction *actionLoadPanel = new QAction(QIcon::fromTheme(iconName), tr("Architectural Metrics..."), this);
    connect(actionLoadPanel, SIGNAL(triggered()), _jsArchitecturalMetricsConfig, SLOT(loadPanel()));
    ICore::self()->uiController()->addAction(actionLoadPanel, menuName);

    QAction *actionRunScript = new QAction(QIcon::fromTheme(iconName), tr("Run Architectural Metric"), this);
    actionRunScript->setData(QVariant::fromValue(metricDefault));
    connect(actionRunScript, SIGNAL(triggered()), this, SLOT(runScript()));
    ICore::self()->uiController()->addAction(actionRunScript, "", toolbarName);

    QWidget *jsWidget = new QWidget;
    _jsArchitecturalMetrics->setupUi(jsWidget);
    ICore::self()->uiController()->addDockWidget(Qt::BottomDockWidgetArea, tr("Architectural Metrics Output"), jsWidget);

    //_jsArchitecturalMetrics->txeJsMetricEvaluation->installEventFilter(this);
    //_codeCompletionView->installEventFilter(this);

    _codeCompletionView->setParent(_jsArchitecturalMetrics->txeJsMetricEvaluation);
    _codeCompletionView->hide();

    connect(ICore::self()->uiController(), &IUiController::currentModelingObjectChanged, this, &JsArchitecturalMetricsPlugin::setSelfProperty);
    connect(ICore::self()->projectController(), SIGNAL(modelOpened(QList<QModelingObject*>)), this, SLOT(initializeEngine(QList<QModelingObject*>)));
    connect(ICore::self()->projectController(), SIGNAL(modelAboutToBeClosed(QList<QModelingObject*>)), this, SLOT(destroyEngine()));

    return true;
}

void JsArchitecturalMetricsPlugin::setSelfProperty(QModelingObject *modelingObject)
{
    _engine->globalObject().setProperty("self", _engine->newQObject(modelingObject));
}

void JsArchitecturalMetricsPlugin::initializeEngine(QList<QModelingObject *> modelingObjects)
{
    if (modelingObjects.size() == 0)
        return;

    _engine = new QScriptEngine;
    qScriptRegisterMetaType(_engine, qSetToScriptValue<QObject>, scriptValueToQSet<QObject>);
    qScriptRegisterMetaType(_engine, qListToScriptValue<QObject>, scriptValueToQList<QObject>);

    QModelingObject *modelingObject = modelingObjects.at(0);
    _engine->globalObject().setProperty(modelingObject->objectName(), _engine->newQObject(modelingObject));

    QScriptValue array = _engine->newArray();
    foreach (QModelingObject *modelingObject, modelingObjects)
        array.property(QString::fromLatin1("push")).call(array, QScriptValueList() << _engine->newQObject(modelingObject));
    _engine->globalObject().setProperty("input", array);
}

void JsArchitecturalMetricsPlugin::destroyEngine()
{
    delete _engine;
    _engine = 0;
    _jsArchitecturalMetrics->txeJsMetricEvaluation->clear();
}

void JsArchitecturalMetricsPlugin::runScript()
{
    QAction *action;
    QString metricScript;

    if ((action = qobject_cast<QAction *>(sender()))) {
        metricScript = action->data().toString();

        _jsArchitecturalMetrics->txeJsMetricEvaluation->setText(_engine->evaluate(metricScript).toString());
    }
    qDebug() << metricScript;
}

}
