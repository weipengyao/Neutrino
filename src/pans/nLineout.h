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

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_series_data.h>
#include <qwt_plot_marker.h>

#include "nGenericPan.h"
#include "ui_nLineout.h"

#include <iostream>
#include "nPhysImageF.h"
#include "nPhysMaths.h"

#ifndef __nlineout
#define __nlineout

#include "neutrino.h"

class nLineout : public nGenericPan {
	Q_OBJECT
public:
	nLineout(neutrino *, QString, enum phys_direction);
	
	public slots:
	void mouseAtMatrix(QPointF);
	
	void toggle_zoom();
	void toggle_scale();
	void toggle_scale(bool);

	void nZoom(double);
	
	void updateLastPoint();
	
public slots:
	void getMinMax();
	void setMinMax(QPointF);
	
private:
	Ui::nLineout my_w;
	
	QwtPlotCurve *curve;
	QwtPlotMarker marker, markerRuler;
	QAction *toggleZoom,*toggleAutoscale;
	
	bool lineout_zoom;
	enum phys_direction cut_dir;
	
	bool autoScale;

	int paxis_index, naxis_index;
	
};

#endif
