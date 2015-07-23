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
#include "designspaceexplorerplugin.h"

#include <duseinterfaces/iuicontroller.h>
#include <duseinterfaces/iplugincontroller.h>
#include <duseinterfaces/iprojectcontroller.h>

#include <QtQuick/QQuickView>

#include <QtWidgets/QAction>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QApplication>

#include <QtCore/QFileInfo>

#include <QtScript/QScriptValueIterator>

#include <QtModeling/QXmiReader>
#include <QtModeling/QMetaModelPlugin>

#include <QtModelingWidgets/QModelingObjectView>
#include <QtModelingWidgets/QModelingObjectModel>

#include <QtUml/QUmlModel>
#include <QtUml/QUmlProfile>
#include <QtUml/QUmlProperty>
#include <QtUml/QUmlOpaqueExpression>
#include <QtUml/QUmlProfileApplication>

#include <QtDuse/QDuseDesignSpace>
#include <QtDuse/QDuseModelChange>
#include <QtDuse/QDuseVariationPoint>
#include <QtDuse/QDuseDesignDimension>
#include <QtDuse/QDuseDesignDimensionInstance>

#include "newdusedesigndialog.h"

#include <QDebug>

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

DesignSpaceExplorerPlugin::DesignSpaceExplorerPlugin(QObject *parent) :
    IPlugin(parent),
    _currentDesignSpaceLocationQuickView(new QQuickView),
    _metricsQuickView(new QQuickView),
    _designSpaceExplorer(new QTableWidget),
    _currentDesignSpaceLocationView(new QModelingObjectView),
    _currentDesignSpaceLocationQtModel(new QModelingObjectModel),
    _newDuseDesignDialog(new DuSE::NewDuseDesignDialog),
    _engine(0)
{
    _currentDesignSpaceLocationView->setModel(_currentDesignSpaceLocationQtModel);
}

DesignSpaceExplorerPlugin::~DesignSpaceExplorerPlugin()
{
    delete _newDuseDesignDialog;
    delete _engine;
}

bool DesignSpaceExplorerPlugin::initialize()
{
    _currentDesignSpaceLocationQuickView->setSource(QUrl("qrc:/designspaceexplorer/currentdesignspacelocation.qml"));
    _currentDesignSpaceLocationQuickView->setResizeMode(QQuickView::SizeRootObjectToView);

    ICore::self()->uiController()->addCentralWidgetTab(QWidget::createWindowContainer(_currentDesignSpaceLocationQuickView), "Current Design Space Location");

    _metricsQuickView->setSource(QUrl("qrc:/designspaceexplorer/qualitymetrics/qualitymetrics.qml"));
    _metricsQuickView->setResizeMode(QQuickView::SizeRootObjectToView);

    ICore::self()->uiController()->addDockWidget(Qt::LeftDockWidgetArea, tr("Quality Metrics"), QWidget::createWindowContainer(_metricsQuickView));

    ICore::self()->uiController()->addDockWidget(Qt::LeftDockWidgetArea, tr("Current Location Inspector"), _currentDesignSpaceLocationView);

    _designSpaceExplorer->setAlternatingRowColors(true);
    _designSpaceExplorer->horizontalHeader()->setStretchLastSection(true);
    _designSpaceExplorer->verticalHeader()->setVisible(false);
    _designSpaceExplorer->setRowCount(0);
    _designSpaceExplorer->setColumnCount(3);
    _designSpaceExplorer->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Design Dimension")));
    _designSpaceExplorer->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Dimension Instance Target")));
    _designSpaceExplorer->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Variation Point")));
    _designSpaceExplorer->resizeColumnToContents(0);
    _designSpaceExplorer->resizeColumnToContents(1);
    _designSpaceExplorer->resizeColumnToContents(2);

    ICore::self()->uiController()->addDockWidget(Qt::RightDockWidgetArea, "Design Space Explorer", _designSpaceExplorer);

    QString iconThemeName;

    QAction *_actionFileNewDuseDesign = new QAction(0);
    _actionFileNewDuseDesign->setText(QApplication::translate("MainWindow", "New &DuSE Design ...", 0));
    _actionFileNewDuseDesign->setShortcut(QApplication::translate("MainWindow", "Ctrl+D", 0));
    _actionFileNewDuseDesign->setObjectName(QStringLiteral("actionFileNewDuseDesign"));
    QIcon icon8;
    iconThemeName = QStringLiteral("document-new");
    if (QIcon::hasThemeIcon(iconThemeName)) {
        icon8 = QIcon::fromTheme(iconThemeName);
    } else {
        icon8.addFile(QStringLiteral(""), QSize(), QIcon::Normal, QIcon::Off);
    }
    _actionFileNewDuseDesign->setIcon(icon8);
    connect(_actionFileNewDuseDesign, &QAction::triggered, this, &DesignSpaceExplorerPlugin::newDuseDesign);
    ICore::self()->uiController()->addAction(_actionFileNewDuseDesign, "menu_File");

    QAction *_actionFileOpenDuseDesign = new QAction(0);
    _actionFileOpenDuseDesign->setText(QApplication::translate("MainWindow", "Open DuSE Design ...", 0));
    _actionFileOpenDuseDesign->setShortcut(QApplication::translate("MainWindow", "Ctrl+U", 0));
    _actionFileOpenDuseDesign->setObjectName(QStringLiteral("actionFileOpenDuseDesign"));
    QIcon icon9;
    iconThemeName = QStringLiteral("document-open");
    if (QIcon::hasThemeIcon(iconThemeName)) {
        icon9 = QIcon::fromTheme(iconThemeName);
    } else {
        icon9.addFile(QStringLiteral(""), QSize(), QIcon::Normal, QIcon::Off);
    }
    _actionFileOpenDuseDesign->setIcon(icon9);
    connect(_actionFileOpenDuseDesign, &QAction::triggered, this, &DesignSpaceExplorerPlugin::openDuseDesign);
    ICore::self()->uiController()->addAction(_actionFileOpenDuseDesign, "menu_File");

    connect(ICore::self()->projectController(), SIGNAL(modelOpened(QList<QModelingObject*>)), this, SLOT(initializeEngine(QList<QModelingObject*>)));
    connect(ICore::self()->projectController(), SIGNAL(modelAboutToBeClosed(QList<QModelingObject*>)), this, SLOT(destroyEngine()));
    connect(ICore::self()->projectController(), SIGNAL(modelClosed()), _currentDesignSpaceLocationQtModel, SLOT(clear()));
    connect(ICore::self()->projectController(), SIGNAL(modelClosed()), this, SLOT(resetDesignSpaceExplorer()));

    return true;
}

void DesignSpaceExplorerPlugin::initializeEngine(QList<QModelingObject *> modelingObjects)
{
    Q_UNUSED(modelingObjects);
    _engine = new QScriptEngine;
    qScriptRegisterMetaType(_engine, qSetToScriptValue<QObject>, scriptValueToQSet<QObject>);
    qScriptRegisterMetaType(_engine, qListToScriptValue<QObject>, scriptValueToQList<QObject>);
    currentDesignSpaceLocationChanged();
}

void DesignSpaceExplorerPlugin::destroyEngine()
{
    delete _engine;
    _engine = 0;
}

void DesignSpaceExplorerPlugin::resetDesignSpaceExplorer()
{
    qDeleteAll(_duseInstance);
    _duseInstance.clear();
    qDeleteAll(_currentDesignSpaceLocationMofModel);
    _currentDesignSpaceLocationMofModel.clear();

    _designSpaceExplorer->setRowCount(0);
    _designSpaceExplorer->clearContents();
}

void DesignSpaceExplorerPlugin::newDuseDesign()
{
    ICore::self()->projectController()->closeModel();
    resetDesignSpaceExplorer();
    do {
        if (_newDuseDesignDialog->exec() == QDialog::Accepted) {
            if (_newDuseDesignDialog->_inputModelFileName.isEmpty() || _newDuseDesignDialog->_duseInstanceModelFileName.isEmpty()) {
                QMessageBox::critical(0, tr("Create new DuSE design"), tr("You should select an input model and a DuSE instance model !"));
            }
            else {
                QFile file(_newDuseDesignDialog->_duseInstanceModelFileName);
                if (!file.open(QFile::ReadOnly | QFile::Text)) {
                    QMessageBox::critical(0, tr("Create new DuSE design"), tr("Cannot read DuSE instance file !"));
                    return;
                }
                QXmiReader reader;
                _duseInstance = reader.readFile(&file);
                if (QString::fromLatin1(_duseInstance.first()->asQModelingObject()->metaObject()->className()) != QStringLiteral("QDuseDesignSpaceObject")) {
                    QMessageBox::critical(0, tr("Create new DuSE design"), QStringLiteral("%1 is not a valid DuSE instance !").arg(QFileInfo(file).fileName()));
                    return;
                }
                if (!ICore::self()->projectController()->openModel(_newDuseDesignDialog->_inputModelFileName)) {
                    QMessageBox::critical(0, tr("Create new DuSE design"), QStringLiteral("Error loading input file %1 !").arg(_newDuseDesignDialog->_inputModelFileName));
                    return;
                }

                bool found = false;
                QUmlModel *package = dynamic_cast<QUmlModel *>(ICore::self()->projectController()->currentModelElements().first());
                QDuseDesignSpace *designSpace = dynamic_cast<QDuseDesignSpace *>(_duseInstance.first());
                foreach (QUmlProfileApplication *profileApplication, package->profileApplications()) {
                    if (profileApplication->appliedProfile()->name() == designSpace->requiredProfile()) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    QMessageBox::critical(0, tr("Create new DuSE design"), QStringLiteral("Input model does not contain the required %1Profile profile application !").arg(_duseInstance.first()->asQModelingObject()->objectName()));
                    return;
                }

                QScriptValue array = _engine->newArray();
                foreach (QObject *modelingObject, ICore::self()->projectController()->currentModelObjects())
                    array.property(QString::fromLatin1("push")).call(array, QScriptValueList() << _engine->newQObject(modelingObject));
                _engine->globalObject().setProperty("input", array);

                foreach (QDuseDesignDimension *designDimension, designSpace->designDimensions()) {
                    foreach (const QString &body, designDimension->instanceSelectionRule()->bodies()) {
                        QScriptValue value = _engine->evaluate(body);
                        if (value.toVariant().canConvert(QMetaType::type("QVariantList"))) {
                            foreach (const QVariant &variant, value.toVariant().value<QVariantList>()) {
                                if (variant.canConvert(QMetaType::type("QObject*"))) {
                                    QDuseDesignDimensionInstance *designDimensionInstance = new QDuseDesignDimensionInstance;
                                    designDimensionInstance->setTargetInstance(qmodelingelementproperty_cast<QUmlElement *>(variant.value<QObject *>()));
                                    designDimension->addDesignDimensionInstance(designDimensionInstance);
                                }
                            }
                        }
                        else if (value.toVariant().canConvert(QMetaType::type("QObject*"))) {
                            QDuseDesignDimensionInstance *designDimensionInstance = new QDuseDesignDimensionInstance;
                            designDimensionInstance->setTargetInstance(qmodelingelementproperty_cast<QUmlElement *>(value.toVariant().value<QObject *>()));
                            designDimension->addDesignDimensionInstance(designDimensionInstance);
                        }
                    }
                }

//                evaluateQualityMetrics();
                populateDesignSpaceExplorer();
            }
        }
        else
            return;
    } while (_newDuseDesignDialog->_inputModelFileName.isEmpty() || _newDuseDesignDialog->_duseInstanceModelFileName.isEmpty());
}

void DesignSpaceExplorerPlugin::populateDesignSpaceExplorer()
{
    QDuseDesignSpace *designSpace = dynamic_cast<QDuseDesignSpace *>(_duseInstance.first());
    _designSpaceExplorer->setRowCount(0);
    int row = 0;
    foreach (QDuseDesignDimension *dimension, designSpace->designDimensions()) {
        _designSpaceExplorer->setRowCount(_designSpaceExplorer->rowCount() + dimension->designDimensionInstances().count());
        foreach (QDuseDesignDimensionInstance *instance, dimension->designDimensionInstances()) {
            _designSpaceExplorer->setItem(row, 0, new QTableWidgetItem(dimension->name()));
            _designSpaceExplorer->setItem(row, 1, new QTableWidgetItem(instance->targetInstance()->asQModelingObject()->objectName()));
            QComboBox *comboBox = new QComboBox;
            foreach (QDuseVariationPoint *variationPoint, dimension->variationPoints()) {
                comboBox->addItem(variationPoint->asQModelingObject()->objectName());
            }
            _designSpaceExplorer->setCellWidget(row, 2, comboBox);
            connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(currentDesignSpaceLocationChanged()));
            ++row;
        }
    }
    _designSpaceExplorer->resizeRowsToContents();
    currentDesignSpaceLocationChanged();
}

void DesignSpaceExplorerPlugin::openDuseDesign()
{
}

void DesignSpaceExplorerPlugin::currentDesignSpaceLocationChanged()
{
    qDeleteAll(_currentDesignSpaceLocationMofModel);
    _currentDesignSpaceLocationMofModel.clear();

    _currentDesignSpaceLocation.clear();

    if (_duseInstance.size() == 0)
        return;

    QModelingObject *designSpaceObject = _duseInstance.first()->asQModelingObject();
    int rowCount = _designSpaceExplorer->rowCount();
    for (int i = 0; i < rowCount; ++i)
        _currentDesignSpaceLocation[_designSpaceExplorer->item(i, 1)->text()][qmodelingelementproperty_cast<QDuseDesignDimension *>(designSpaceObject->findChild<QModelingObject *>(_designSpaceExplorer->item(i, 0)->text()))] = qmodelingelementproperty_cast<QDuseVariationPoint *>(designSpaceObject->findChild<QModelingObject *>((dynamic_cast<QComboBox *>(_designSpaceExplorer->cellWidget(i, 2)))->currentText()));

    foreach (QModelingElement *element, ICore::self()->projectController()->currentModelElements())
        _currentDesignSpaceLocationMofModel << element->clone();

    QList<QModelingObject *> currentModelObjects;
    foreach (QModelingElement *element, _currentDesignSpaceLocationMofModel)
        currentModelObjects << element->asQModelingObject();

    QDuseDesignSpace *designSpace = dynamic_cast<QDuseDesignSpace *>(_duseInstance.first());
    QUmlModel *topModel = dynamic_cast<QUmlModel *>(_currentDesignSpaceLocationMofModel.first());
    foreach (QDuseDesignDimension *designDimension, designSpace->designDimensions()) {
        foreach (QDuseDesignDimensionInstance *designDimensionInstance, designDimension->designDimensionInstances()) {
            QDuseVariationPoint *variationPoint = _currentDesignSpaceLocation[designDimensionInstance->targetInstance()->asQModelingObject()->objectName()][designDimension];
            foreach (QModelingElement *addedElement, variationPoint->addedElements()) {
                QObject *modelingObject = addedElement->asQModelingObject();
                QMetaModelPlugin *plugin = ICore::self()->pluginController()->metamodelPlugins()[modelingObject->property("namespaceUri").toString()];
                QModelingElement *newAddedElement = plugin->createModelingElement(modelingObject->property("factoryType").toString());
                newAddedElement->asQModelingObject()->setObjectName(QStringLiteral("%1-%2").arg(designDimensionInstance->targetInstance()->asQModelingObject()->objectName()).arg(modelingObject->property("factoryType").toString()));
                topModel->addPackagedElement(dynamic_cast<QUmlPackageableElement *>(newAddedElement));
            }
        }
    }

    foreach (QDuseDesignDimension *designDimension, designSpace->designDimensions()) {
        foreach (QDuseDesignDimensionInstance *designDimensionInstance, designDimension->designDimensionInstances()) {
            QDuseVariationPoint *variationPoint = _currentDesignSpaceLocation[designDimensionInstance->targetInstance()->asQModelingObject()->objectName()][designDimension];
            foreach (QDuseModelChange *modelChange, variationPoint->modelChanges()) {
                QScriptValue target = _engine->evaluate(modelChange->targetSelectionRule()->bodies().first());
                if (target.toVariant().canConvert(QMetaType::type("QObject*"))) {
                    QObject *targetObject = target.toVariant().value<QObject *>();
                    QString propertyName = modelChange->targetProperty()->name();
                    QScriptValue value = _engine->evaluate(modelChange->valueSelectionRule()->bodies().first());
                    QMetaProperty metaProperty = targetObject->metaObject()->property(targetObject->metaObject()->indexOfProperty(propertyName.toLatin1()));
                    QString typeName = QString::fromLatin1(metaProperty.typeName());
                    if (metaProperty.type() == QVariant::String) {
                        targetObject->setProperty(propertyName.toLatin1(), value.toVariant().toString());
                    }
                    else if (metaProperty.type() == QVariant::Int || metaProperty.isEnumType()) {
                        targetObject->setProperty(propertyName.toLatin1(), value.toVariant().toInt());
                    }
                    else if (typeName.endsWith('*')) {
                        targetObject->setProperty(propertyName.toLatin1(), QVariant::fromValue(qmodelingelementproperty_cast<QUmlElement *>(value.toVariant().value<QObject *>())));
                    }
                }
            }
        }
    }

    _currentDesignSpaceLocationQtModel->setModelingObjects(currentModelObjects);
}

}

