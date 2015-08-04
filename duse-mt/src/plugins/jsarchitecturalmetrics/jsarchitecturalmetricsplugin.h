#ifndef JSARCHITECTURALMETRICSPLUGIN_H
#define JSARCHITECTURALMETRICSPLUGIN_H

#include <duseinterfaces/iplugin.h>

#include "jsarchitecturalmetricsconfig.h"

QT_BEGIN_NAMESPACE
class QListView;
class QScriptEngine;

class QModelingObject;
class QModelingObjectView;
class QModelingObjectModel;
class QModelingObjectPropertyEditor;
class QModelingObjectPropertyModel;
QT_END_NAMESPACE

namespace DuSE
{

namespace Ui {
    class JsArchitecturalMetrics;
}

class JsArchitecturalMetricsPlugin : public IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.liveblue.DuSE.IPlugin" FILE "jsarchitecturalmetrics.json")

public:
    JsArchitecturalMetricsPlugin(QObject *parent = 0);
    ~JsArchitecturalMetricsPlugin();

    virtual bool initialize();
    bool runScript(QString scriptFileName);
//    void runSelectedScript();

private Q_SLOTS:
    void setSelfProperty(QModelingObject *modelingObject);
    void initializeEngine(QList<QModelingObject *> modelingObjects);
    void destroyEngine();
    void runDefaultScript();
    void runSelectedScript();

private:
    JsArchitecturalMetricsConfig *_jsArchitecturalMetricsConfig;
    Ui::JsArchitecturalMetrics *_jsArchitecturalMetrics;
    QListView *_codeCompletionView;
    QScriptEngine *_engine;
};

}

#endif // JSARCHITECTURALMETRICSPLUGIN_H
