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
#include "neutrino.h"
#include "Mouse_info.h"


Mouse_info::Mouse_info (neutrino *parent) : nGenericPan(parent)
{

	my_w.setupUi(this);

	connect(my_w.removeRow, SIGNAL(released()),this, SLOT(remove_point()));
	connect(my_w.copyPoints, SIGNAL(released()),this, SLOT(copyPoints()));
	connect(my_w.exportTxt, SIGNAL(released()),this, SLOT(export_txt()));

    connect(nparent, SIGNAL(mouseAtMatrix(QPointF)), this, SLOT(setMouse(QPointF)));
	connect(my_w.rx, SIGNAL(editingFinished()), this, SLOT(updateOrigin()));
	connect(my_w.ry, SIGNAL(editingFinished()), this, SLOT(updateOrigin()));
	connect(my_w.sc_x, SIGNAL(editingFinished()), this, SLOT(updateScale()));
    connect(my_w.sc_y, SIGNAL(editingFinished()), this, SLOT(updateScale()));
    connect(my_w.unit_x, SIGNAL(editingFinished()), this, SLOT(updateUnits()));
    connect(my_w.unit_y, SIGNAL(editingFinished()), this, SLOT(updateUnits()));

    connect(nparent->my_w->my_view, SIGNAL(mousePressEvent_sig(QPointF)), this, SLOT(addPoint(QPointF)));

    connect(nparent, SIGNAL(bufferChanged(nPhysD*)), this, SLOT(updateLabels()));

    mouse=nparent->my_w->my_view->my_mouse.pos();
	updateLabels();
    show(true);
}

void Mouse_info::addPoint(QPointF position) {
	if (my_w.getpoints->isChecked()) {
		int pos=my_w.points->rowCount();
		my_w.points->insertRow(pos);
		QTableWidgetItem *xitem= new QTableWidgetItem(QLocale().toString(position.x()));
		QTableWidgetItem *yitem= new QTableWidgetItem(QLocale().toString(position.y()));
		QTableWidgetItem *x2item= new QTableWidgetItem(my_w.px->text());
		QTableWidgetItem *y2item= new QTableWidgetItem(my_w.py->text());
		xitem->setTextAlignment(Qt::AlignHCenter + Qt::AlignVCenter);
		yitem->setTextAlignment(Qt::AlignHCenter + Qt::AlignVCenter);
		x2item->setTextAlignment(Qt::AlignHCenter + Qt::AlignVCenter);
		y2item->setTextAlignment(Qt::AlignHCenter + Qt::AlignVCenter);
		xitem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
		yitem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
		x2item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
		y2item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
		my_w.points->setItem(pos, 0, xitem);
		my_w.points->setItem(pos, 1, yitem);
		my_w.points->setItem(pos, 2, x2item);
		my_w.points->setItem(pos, 3, y2item);
		QTableWidgetItem *zitem= new QTableWidgetItem();
		if (currentBuffer) {
			zitem->setText(QLocale().toString(currentBuffer->point(position.x(),position.y())));
		} else {
			zitem->setText(QString(""));
		}

		zitem->setTextAlignment(Qt::AlignHCenter + Qt::AlignVCenter);
		zitem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
		my_w.points->setItem(pos, 4, zitem);
		my_w.points->resizeRowToContents(pos);
		statusBar()->showMessage(tr("Added point ")+QLocale().toString(pos+1),2000);
	}
}

void Mouse_info::updateOrigin() {
	if (currentBuffer) {
		bool ok1, ok2;
        double valx=QLocale().toDouble(my_w.rx->text(),&ok1);
        double valy=QLocale().toDouble(my_w.ry->text(),&ok2);
		if (ok1&&ok2) {
			currentBuffer->set_origin(vec2f(valx,valy));
		}
	}
    nparent->my_w->my_view->update();
}

void Mouse_info::updateScale() {
	if (currentBuffer) {
		bool ok1, ok2;
        double valx=QLocale().toDouble(my_w.sc_x->text(),&ok1);
        double valy=QLocale().toDouble(my_w.sc_y->text(),&ok2);
		if (ok1&&ok2) {
			currentBuffer->set_scale(vec2f(valx,valy));
		}
	}
    nparent->my_w->my_view->update();
}

void Mouse_info::updateUnits() {
    if (currentBuffer) {
        currentBuffer->prop["unitsX"] = my_w.unit_x->text().toStdString();
        currentBuffer->prop["unitsY"] = my_w.unit_y->text().toStdString();
    }
    nparent->my_w->my_view->update();
}


void Mouse_info::setMouse(QPointF pos) {
	mouse=pos;
	updateLabels();
}

void Mouse_info::updateLabels() {
	my_w.ix->setText(QLocale().toString(mouse.x()));
	my_w.iy->setText(QLocale().toString(mouse.y()));
		
	QPointF dist=QPointF(mouse);
	QPointF resolution=QPointF(1,1);
	if (currentBuffer) {
		dist=mouse-QPointF(currentBuffer->get_origin().x(),currentBuffer->get_origin().y());
		resolution=QPointF(currentBuffer->get_scale().x(),currentBuffer->get_scale().y());
		disconnect(my_w.rx, SIGNAL(textChanged(QString)), this, SLOT(updateOrigin()));
		disconnect(my_w.ry, SIGNAL(textChanged(QString)), this, SLOT(updateOrigin()));
		disconnect(my_w.sc_x, SIGNAL(textChanged(QString)), this, SLOT(updateScale()));
		disconnect(my_w.sc_y, SIGNAL(textChanged(QString)), this, SLOT(updateScale()));	
        connect(my_w.unit_x, SIGNAL(editingFinished()), this, SLOT(updateUnits()));
        connect(my_w.unit_y, SIGNAL(editingFinished()), this, SLOT(updateUnits()));
        my_w.rx->setText(QLocale().toString(currentBuffer->get_origin().x()));
		my_w.ry->setText(QLocale().toString(currentBuffer->get_origin().y()));
        my_w.sc_x->setText(QLocale().toString(currentBuffer->get_scale().x()));
        my_w.sc_y->setText(QLocale().toString(currentBuffer->get_scale().y()));
        my_w.unit_x->setText(QString::fromStdString(currentBuffer->prop["unitsX"].get_str()));
        my_w.unit_y->setText(QString::fromStdString(currentBuffer->prop["unitsY"].get_str()));
        connect(my_w.rx, SIGNAL(textChanged(QString)), this, SLOT(updateOrigin()));
		connect(my_w.ry, SIGNAL(textChanged(QString)), this, SLOT(updateOrigin()));
		connect(my_w.sc_x, SIGNAL(textChanged(QString)), this, SLOT(updateScale()));
		connect(my_w.sc_y, SIGNAL(textChanged(QString)), this, SLOT(updateScale()));
        connect(my_w.unit_x, SIGNAL(editingFinished()), this, SLOT(updateUnits()));
        connect(my_w.unit_y, SIGNAL(editingFinished()), this, SLOT(updateUnits()));
    }

	QPointF p=QPointF(resolution.x()*dist.x(),resolution.y()*dist.y());

	my_w.px->setText(QLocale().toString(p.x()));
	my_w.py->setText(QLocale().toString(p.y()));
	my_w.dx_dy->setText(QLocale().toString(p.x()/p.y()));
	my_w.dy_dx->setText(QLocale().toString(p.y()/p.x()));
    my_w.atan_dx_dy->setText(QLocale().toString(180.0/M_PI*atan2(p.x(),p.y())));
    my_w.atan_dy_dx->setText(QLocale().toString(180.0/M_PI*atan2(p.y(),p.x())));
	my_w.dist_px->setText(QLocale().toString(qSqrt(dist.x()*dist.x()+dist.y()*dist.y())));
	my_w.dist_real->setText(QLocale().toString(qSqrt(p.x()*p.x()+p.y()*p.y())));
}

QString 
Mouse_info::getPointText() {
	QString retText;
	for (int i=0; i<my_w.points->rowCount(); i++) {
		for (int j=0; j<my_w.points->columnCount();j++) {
			retText += my_w.points->item(i, j)->text() + "\t";
		}
		retText += "\n";
	}
	return retText;
}

void Mouse_info::copyPoints() {
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(getPointText());
	statusBar()->showMessage(tr("Point values copied"),2000);
}

void
Mouse_info::export_txt() {
    QString fnametmp=QFileDialog::getSaveFileName(this,tr("Save data"),property("NeuSave-fileTxt").toString(),tr("Text files (*.txt *.csv);;Any files (*)"));
	if (!fnametmp.isEmpty()) {
        setProperty("NeuSave-fileTxt",fnametmp);
		QFile t(fnametmp);
		t.open(QIODevice::WriteOnly| QIODevice::Text);
		QTextStream out(&t);
		out << getPointText();
		t.close();
		statusBar()->showMessage(tr("Export in file:")+fnametmp,2000);
	} else {
		statusBar()->showMessage(tr("Export canceled"),2000);
	}
}

void
Mouse_info::remove_point() {
	QString removedrows;
	foreach (QTableWidgetSelectionRange r, my_w.points->selectedRanges()) {
		for (int i=r.topRow(); i <=r.bottomRow(); i++) {
			my_w.points->removeRow(r.topRow());
			removedrows+=QString(" ")+QLocale().toString(i+1);
		}
	}
	if (removedrows.isEmpty()) {
		statusBar()->showMessage(tr("No row was selected"));
	} else {
		statusBar()->showMessage(tr("Removed Rows:")+removedrows,2000);
	}
}

