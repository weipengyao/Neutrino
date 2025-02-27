/*
 *
 *    Copyright (C) 2013 Alessandro Flacco, Tommaso Vinci All Rights Reserved
 *
 *    This file is part of neutrino.
 *
 *    Neutrino is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    Neutrino is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with neutrino.  If not, see <http://www.gnu.org/licenses/>.
 *
 *    Contact Information:
 *	Alessandro Flacco <alessandro.flacco@polytechnique.edu>
 *	Tommaso Vinci <tommaso.vinci@polytechnique.edu>
 *
 */
#include <QtGui>
#include <QWidget>

#include <QComboBox>
#include <QGestureEvent>
#include <QGraphicsScene>
#include <QMainWindow>

#include <QTextBrowser>
#include <QPrinter>
#include <QPrintDialog>

#include <iostream>
#include "nPhysD.h"
#include "panThread.h"

#ifndef __generic_pan
#define __generic_pan

#include "nPanPlug.h"

class neutrino;
class nApp;

typedef void (*ifunc)(void *, int &);

class nHelpTextBrowser : public QTextBrowser {
    Q_OBJECT
public:
    nHelpTextBrowser(QWidget* parent) : QTextBrowser(parent) {}

public slots:
    void print() {
        QPrinter printer(QPrinter::HighResolution);
        QPrintDialog dialog(&printer,this);
        dialog.setWindowTitle(tr("Print Help"));
        if (dialog.exec() == QDialog::Accepted) {
            document()->print(&printer);
        }
    }
};


class nGenericPan : public QMainWindow {
    Q_OBJECT

public:
    Q_INVOKABLE nGenericPan(){}
    Q_INVOKABLE nGenericPan(neutrino *);
    ~nGenericPan() override {}

    QPointer<neutrino> nparent;
    QPointer<nApp> napp;

    nPhysD *currentBuffer;

    // thread stuff
    panThread nThread;

    QPointer<QMainWindow> helpwin;

public slots:

    void raiseNeutrino();

    void changeEvent(QEvent *e) override;

    QString panName();
    void grabSave();
    void help();

    void showMessage(QString);
    void showMessage(QString,int);
    virtual void mouseAtMatrix(QPointF) { }
    virtual void mouseAtWorld(QPointF) { }

    virtual void nZoom(double) { }

    virtual void imageMousePress(QPointF) { }
    virtual void imageMouseRelease(QPointF) { }

    void physAdd(nPhysD *);
    void physDel(nPhysD *);

    virtual void bufferChanged(nPhysD *);

    // threads
    void runThread(void *iparams, ifunc, QString=QString("Calculating"), int=0);

    // to sync image list on combos on the widget
    void comboChanged(int);
    nPhysD* getPhysFromCombo(QComboBox*);

    QString getNameForCombo(QComboBox*,nPhysD *);

    void raiseIt();

    void closeEvent(QCloseEvent*) override;

    //settings
    void loadDefaults();
    void saveDefaults();
    void loadSettings(QString=QString());
    void saveSettings(QString=QString());
    virtual void loadSettings(QSettings&);
    virtual void saveSettings(QSettings&);
    void loadUi(QSettings&);
    void saveUi(QSettings&);

    bool nPhysExists(nPhysD*);

    // python stuff
    void set(QString, QVariant, int=1);
    QVariant get(QString, int=1);
    QList<QList<qreal> >  getData(QString, int=1);
    void button(QString, int=1);
    QStringList widgets();

    void show(bool onlyOneAllowed=false);

    void keyPressEvent(QKeyEvent *event) override;

protected:
    void decorate(QWidget *wdg);
    void focusOutEvent(QFocusEvent *event) override;

};

Q_DECLARE_METATYPE(nGenericPan*);

#endif
