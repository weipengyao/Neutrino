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
#include "Shortcuts.h"
Shortcuts::Shortcuts(neutrino *nparent) : nGenericPan(nparent)
{
    my_w.setupUi(this);
    setWindowFlags(Qt::Tool);
    
    QFile lic(":/html/Shortcuts.html");
    if (lic.open(QFile::ReadOnly | QFile::Text)) {
        QString htmltext=QTextStream(&lic).readAll().replace("%CTRL%",QKeySequence(Qt::CTRL).toString(QKeySequence::NativeText));
        my_w.textBrowser->insertHtml(htmltext);
    }
    show(true);
}


