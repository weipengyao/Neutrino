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

#include "nGenericPan.h"
#include "ui_Box_lineout.h"

#ifndef __Box_lineout
#define __Box_lineout

#include "neutrino.h"

class nRect;

class Box_lineout : public nGenericPan {
    Q_OBJECT

public:

    Q_INVOKABLE Box_lineout(neutrino *);
    Ui::Box_lineout my_w;

public slots:

    void mouseAtWorld(QPointF);
    void updatePlot();

    void sceneChanged();

private:

    QPointer<nRect> box;
};

NEUTRINO_PLUGIN(Box_lineout,Analysis;Lineout);


#endif
