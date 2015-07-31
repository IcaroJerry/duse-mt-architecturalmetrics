#ifndef JSARCHITECTURALMETRICS_H
#define JSARCHITECTURALMETRICS_H

#include <QtWidgets/QDialog>

#include <duseinterfaces/ijsplugin.h>

namespace DuSE
{

namespace Ui {
    class JsArchitecturalMetrics;
}

class JsArchitecturalMetrics : public QDialog
{
    Q_OBJECT

public:
    explicit JsArchitecturalMetrics(QWidget *parent = 0);
    ~JsArchitecturalMetrics();

    QString _jsArchitecturalMetricsDir;
    QString _jsonFileName;

public Q_SLOTS:
    void loadPanel();
    void setMetricDefault();
    bool runScript();

private:

    void loadMetricsInfo();

    QDialog *_jsArchitecturalMetricsDialog;
    Ui::JsArchitecturalMetrics *_ui;

};

#endif // JSARCHITECTURALMETRICS_H

}
