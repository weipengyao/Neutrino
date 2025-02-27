/*
 *
 *    Copyright (C) 2014 Alessandro Flacco, Tommaso Vinci All Rights Reserved
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
#include "ui_FocalSpot.h"

#ifndef __FocalSpot
#define __FocalSpot

class neutrino;
class nLine;

class FocalSpot : public nGenericPan {
    Q_OBJECT

    using nGenericPan::loadSettings;

public:
    Q_INVOKABLE FocalSpot(neutrino *);

    Ui::FocalSpot my_w;

    QPointer<nLine> nContour;

public slots:
    void calculate_stats();
    QList<double> find_contour(double);

    void bufferChanged(nPhysD*);

    void on_centroid_toggled(bool tog);
    void setPosZero(QPointF pos);

    void mouseAtPlot(QMouseEvent*);
    void mouseAtWorld(QPointF);

    void loadSettings(QString=QString());


private:
    // blurred image for easier calculations
    nPhysD decimated;

};

NEUTRINO_PLUGIN(FocalSpot,Analysis);


#endif
