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

public Q_SLOTS:
    void scriptsMetrics();//apagar
    void loadPanel();
    //void runScript();//test
    bool runScript();

private:
    QDialog *_jsArchitecturalMetricsDialog;
    Ui::JsArchitecturalMetrics *ui;

};

#endif // JSARCHITECTURALMETRICS_H

}
