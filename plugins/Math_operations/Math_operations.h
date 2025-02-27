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
#include "ui_Math_operations.h"

#ifndef __Math_operations
#define __Math_operations

class neutrino;

class Math_operations : public nGenericPan {
    Q_OBJECT

    std::vector<int> separator;

public:	
    Q_INVOKABLE Math_operations(neutrino *);

    Ui::Math_operations my_w;
    nPhysD *operatorResult;

public slots:
    void doOperation();
    void copyResult();
    void enableGroups(int);
};

NEUTRINO_PLUGIN(Math_operations,Analysis,":icons/math.png");

#endif
