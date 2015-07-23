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
#include "javascriptconsoleplugin.h"

#include "ui_javascriptconsole.h"

#include <duseinterfaces/iuicontroller.h>
#include <duseinterfaces/iprojectcontroller.h>

#include <QtModeling/QModelingObject>

#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValueIterator>

#include <QtGui/QKeyEvent>

#include <QtWidgets/QListView>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>

#include <QtCore/QTimer>
#include <QtCore/QTextStream>
#include <QtCore/QStringListModel>

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

JavaScriptConsolePlugin::JavaScriptConsolePlugin(QObject *parent) :
    IPlugin(parent),
    _javaScriptConsole(new Ui::JavaScriptConsole),
    _codeCompletionView(new QListView),
    _engine(0)
{
}

JavaScriptConsolePlugin::~JavaScriptConsolePlugin()
{
    delete _javaScriptConsole;
    delete _engine;
}

bool JavaScriptConsolePlugin::initialize()
{
    QWidget *javaScriptConsoleWidget = new QWidget;
    _javaScriptConsole->setupUi(javaScriptConsoleWidget);
    ICore::self()->uiController()->addDockWidget(Qt::BottomDockWidgetArea, tr("JavaScript Console"), javaScriptConsoleWidget);

    _javaScriptConsole->txeJavaScript->installEventFilter(this);
    _codeCompletionView->installEventFilter(this);

    _codeCompletionView->setParent(_javaScriptConsole->txeJavaScript);
    _codeCompletionView->hide();

    connect(_javaScriptConsole->tbtJSEvaluate, &QToolButton::clicked, this, &JavaScriptConsolePlugin::evaluate);
    connect(_javaScriptConsole->tbtJSEvaluate, SIGNAL(clicked()), ICore::self()->uiController(), SIGNAL(updateCurrentModelingObject()));

    connect(_javaScriptConsole->tbtSaveScript, &QToolButton::clicked, this, &JavaScriptConsolePlugin::saveScript);
    connect(_javaScriptConsole->tbtOpenScript, &QToolButton::clicked, this, &JavaScriptConsolePlugin::openScript);

    connect(ICore::self()->uiController(), &IUiController::currentModelingObjectChanged, this, &JavaScriptConsolePlugin::setSelfProperty);
    connect(ICore::self()->projectController(), SIGNAL(modelOpened(QList<QModelingObject*>)), this, SLOT(initializeEngine(QList<QModelingObject*>)));
    connect(ICore::self()->projectController(), SIGNAL(modelAboutToBeClosed(QList<QModelingObject*>)), this, SLOT(destroyEngine()));

    return true;
}

void JavaScriptConsolePlugin::setSelfProperty(QModelingObject *modelingObject)
{
    _engine->globalObject().setProperty("self", _engine->newQObject(modelingObject));
}

void JavaScriptConsolePlugin::initializeEngine(QList<QModelingObject *> modelingObjects)
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

    _javaScriptConsole->txeJavaScript->setText("self");
    _javaScriptConsole->tbtJSEvaluate->setEnabled(true);
    _javaScriptConsole->tbtSaveScript->setEnabled(true);
    _javaScriptConsole->tbtOpenScript->setEnabled(true);
    QTimer::singleShot(0, this, SLOT(evaluate()));
}

void JavaScriptConsolePlugin::destroyEngine()
{
    delete _engine;
    _engine = 0;
    _javaScriptConsole->txeJavaScript->clear();
    _javaScriptConsole->txeJavaScriptEvaluation->clear();
    _javaScriptConsole->tbtJSEvaluate->setEnabled(false);
    _javaScriptConsole->tbtSaveScript->setEnabled(false);
    _javaScriptConsole->tbtOpenScript->setEnabled(false);
}

bool JavaScriptConsolePlugin::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress && obj == _javaScriptConsole->txeJavaScript) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == 46) {
            QModelingObject *modelingObject = dynamic_cast<QModelingObject *>(_engine->evaluate(_javaScriptConsole->txeJavaScript->toPlainText()).toQObject());
            if (modelingObject) {
                const QMetaObject *metaObject = modelingObject->metaObject();
                int propertyCount = metaObject->propertyCount();
                QStringList propertyList;
                for (int i = 0; i < propertyCount; ++i)
                    propertyList << metaObject->property(i).name();
                _codeCompletionView->setModel(new QStringListModel(propertyList));
                QFont font;
                QFontMetrics fm(font);
                _codeCompletionView->setGeometry(_javaScriptConsole->txeJavaScript->cursorRect().x(), _javaScriptConsole->txeJavaScript->cursorRect().y()+fm.height(), 200, 100);
                _codeCompletionView->show();
                _codeCompletionView->setFocus();
            }
        }
        return QObject::eventFilter(obj, event);
    } else if (event->type() == QEvent::KeyPress && obj == _codeCompletionView) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
         if (keyEvent->key() == 16777220 || keyEvent->key() == 32) { // spacebar or enter
            _javaScriptConsole->txeJavaScript->insertPlainText(_codeCompletionView->model()->data(_codeCompletionView->selectionModel()->selectedIndexes().first()).toString());
            _codeCompletionView->hide();
            _javaScriptConsole->txeJavaScript->setFocus();
            return true;
        }
        else if (keyEvent->key() == 16777235 || keyEvent->key() == 16777237 || keyEvent->key() == 16777239 || keyEvent->key() == 16777238) { // uparrow and downarrow, pageup, pagedown
            return QObject::eventFilter(obj, event);
        }
        else {
            _codeCompletionView->hide();
            _javaScriptConsole->txeJavaScript->setFocus();
            return true;
        }
    }
    // standard event processing
    return QObject::eventFilter(obj, event);
}

void JavaScriptConsolePlugin::evaluate()
{
    _javaScriptConsole->txeJavaScriptEvaluation->setText(_engine->evaluate(_javaScriptConsole->txeJavaScript->toPlainText()).toString());
}

void JavaScriptConsolePlugin::saveScript()
{
    QString fileName = QFileDialog::getSaveFileName(0, tr("Save script"), QDir::currentPath(), "*.js");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(0, tr("Save script"), tr("Error when saving script as %1").arg(fileName));
            return;
        }
        QTextStream out(&file);
        out << _javaScriptConsole->txeJavaScript->toPlainText();
        file.close();
    }
}

void JavaScriptConsolePlugin::openScript()
{
    QString fileName = QFileDialog::getOpenFileName(0, tr("Open script"), QDir::currentPath(), "*.js");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::critical(0, tr("Open script"), tr("Error when opening script %1").arg(fileName));
            return;
        }
        QTextStream in(&file);
        _javaScriptConsole->txeJavaScript->setPlainText(in.readAll());
        file.close();
    }
}

}

