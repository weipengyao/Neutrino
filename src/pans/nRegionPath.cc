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
#include "nRegionPath.h"
#include "neutrino.h"

nRegionPath::nRegionPath(neutrino *nparent, QString winname)
: nGenericPan(nparent, winname)
{

	my_w.setupUi(this);

	region =  new nLine(nparent);
	region->setParentPan(panName,1);
	// TODO: create something better to avoid line removal
	region->setPoints(QPolygonF()<<QPointF(10, 10)<<QPointF(10, 50)<<QPointF(50, 50));
	region->toggleClosedLine(true);

	decorate();

	connect(my_w.actionLoadPref, SIGNAL(triggered()), this, SLOT(loadSettings()));
	connect(my_w.actionSavePref, SIGNAL(triggered()), this, SLOT(saveSettings()));
	connect(my_w.actionLine, SIGNAL(triggered()), region, SLOT(togglePadella()));
	connect(my_w.actionBezier, SIGNAL(triggered()), region, SLOT(toggleBezier()));

	connect(my_w.doIt, SIGNAL(clicked()), SLOT(doIt()));
	connect(my_w.doMask, SIGNAL(clicked()), SLOT(doMask()));
	regionPhys=NULL;
}

void
nRegionPath::doIt() {
	if (currentBuffer) {
		saveDefaults();
		nPhysD *image=getPhysFromCombo(my_w.image);
		if (image) {
			QPolygon regionPoly=region->poly(region->numPoints).toPolygon();
			QRect rectRegion=region->boundingRect().toRect().intersected(QRect(0,0,image->getW(),image->getH()));
			nPhysD regionPath=image->sub(rectRegion.x(), rectRegion.y(), rectRegion.width(), rectRegion.height());
			regionPath.setShortName("Region mask");
			regionPath.setName("mask");
			regionPath.set_origin(image->get_origin()-vec2f(rectRegion.x(),rectRegion.y()));
			QProgressDialog progress("Extracting", "Stop", 0, regionPath.getW(), this);
			progress.setWindowModality(Qt::WindowModal);
			progress.show();
			QTime time;
			time.start();
			for (register size_t i=0; i<regionPath.getW(); i++) {
				if (progress.wasCanceled()) break;
				QApplication::processEvents();
				for (register size_t j=0; j<regionPath.getH(); j++) {
					if (!regionPoly.containsPoint(QPoint(i+rectRegion.x(),j+rectRegion.y()),Qt::OddEvenFill)) {
						regionPath.set(i,j,numeric_limits<double>::quiet_NaN( ));
					}

				}
				progress.setValue(i);
			}
//			regionPath.TscanBrightness();
//			qDebug() << ">>>>>>>>>>>>>>>>> time " << time.elapsed();
			regionPhys=nparent->replacePhys(&regionPath,regionPhys);
		}
	}
}

void
nRegionPath::doMask() {
	if (currentBuffer) {
		saveDefaults();
		nPhysD *image=getPhysFromCombo(my_w.image);
		if (image) {
			QPolygon regionPoly=region->poly(region->numPoints).toPolygon();
			QRect rectRegion=region->boundingRect().toRect().intersected(QRect(0,0,image->getW(),image->getH()));
			nPhysD *regionPath = new nPhysD(rectRegion.width(),rectRegion.height(),0.0);
			regionPath->setShortName("Region mask");
			regionPath->setName("mask");
			regionPath->set_origin(image->get_origin()-vec2f(rectRegion.x(),rectRegion.y()));
			QProgressDialog progress("Extracting", "Stop", 0, regionPath->getW(), this);
			progress.setWindowModality(Qt::WindowModal);
			progress.show();
			QTime time;
			time.start();
			for (register size_t i=0; i<regionPath->getW(); i++) {
				if (progress.wasCanceled()) break;
				QApplication::processEvents();
				for (register size_t j=0; j<regionPath->getH(); j++) {
					if (regionPoly.containsPoint(QPoint(i+rectRegion.x(),j+rectRegion.y()),Qt::WindingFill)) {
						regionPath->set(i,j,1);
					}
				}
				progress.setValue(i);
			}
//			qDebug() << ">>>>>>>>>>>>>>>>> time " << time.elapsed();
			regionPhys=nparent->replacePhys(regionPath,regionPhys);
		}
	}
}


