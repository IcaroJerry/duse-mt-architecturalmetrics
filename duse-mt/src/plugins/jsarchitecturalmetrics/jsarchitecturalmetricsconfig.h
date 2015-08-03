#ifndef JSARCHITECTURALMETRICSCONFIG_H
#define JSARCHITECTURALMETRICSCONFIG_H

#include <QtWidgets/QDialog>

#include <duseinterfaces/iplugin.h>

namespace DuSE
{

namespace Ui {
    class JsArchitecturalMetricsConfig;
}

class JsArchitecturalMetricsConfig : public QDialog
{
    Q_OBJECT

public:
    explicit JsArchitecturalMetricsConfig(QWidget *parent = 0);
    ~JsArchitecturalMetricsConfig();

    QString _jsArchitecturalMetricsDir;
    QString _jsonFileName;

public Q_SLOTS:
    void loadPanel();
    void setMetricDefault();
    void runSelectedScript();
    void runDefaultScript();

private:
    void clearMetricsInfo();
    void loadMetricsInfo();
    //bool runScript(QString scriptFileName);

    QDialog *_jsArchitecturalMetricsDialog;
    Ui::JsArchitecturalMetricsConfig *_ui;

};

#endif // JSARCHITECTURALMETRICSCONFIG_H

}
