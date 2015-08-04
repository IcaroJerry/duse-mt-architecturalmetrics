#ifndef JSARCHITECTURALMETRICSCONFIG_H
#define JSARCHITECTURALMETRICSCONFIG_H

#include <duseinterfaces/iplugin.h>

#include <QtWidgets/QDialog>

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
    QString scriptNameFileSelected();

public Q_SLOTS:
    void loadPanel();
    void setMetricDefault();
    void runSelectedScript();

private:
    void clearMetricsInfo();
    void loadMetricsInfo();

    QDialog *_jsArchitecturalMetricsDialog;
    Ui::JsArchitecturalMetricsConfig *_ui;
};

}

#endif // JSARCHITECTURALMETRICSCONFIG_H
