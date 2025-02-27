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
#include "Math_operations.h"
#include "neutrino.h"

// physWavelets

Math_operations::Math_operations(neutrino *nparent) : nGenericPan(nparent)
{
    my_w.setupUi(this);
    connect(my_w.calculate, SIGNAL(pressed()), this, SLOT(doOperation()));
    connect(my_w.copyButton, SIGNAL(pressed()), this, SLOT(copyResult()));
    connect(my_w.operation, SIGNAL(currentIndexChanged(int)), this, SLOT(enableGroups(int)));
    operatorResult=NULL;

    for(int i=0;i<my_w.operation->count();i++){
        my_w.operation->itemText(i);
        if (my_w.operation->itemText(i).startsWith("-")) {
            qDebug() << i;
            separator.push_back(i);
        }
    }
    show();

}

void Math_operations::copyResult () {
    if (!operatorResult) doOperation();
    if (operatorResult) {
        nPhysD *newoperatorResult=new nPhysD(*operatorResult);
        nparent->addPhys(newoperatorResult);
    }
}

void Math_operations::enableGroups (int num) {
    qDebug() << ">>>>>>>>>>>>>" << num;

    my_w.first->setEnabled(true);
    my_w.second->setEnabled(true);
    my_w.radioImage1->setEnabled(true);
    my_w.radioImage2->setEnabled(true);
    my_w.image1->setEnabled(true);
    my_w.image2->setEnabled(true);
    my_w.radioNumber1->setEnabled(true);
    my_w.radioNumber2->setEnabled(true);
    my_w.num1->setEnabled(true);
    my_w.num2->setEnabled(true);

    if(num > separator[0] ) {
        if (num < separator[1]) {
            my_w.radioImage1->setChecked(true);
            my_w.radioNumber1->setEnabled(false);
            my_w.num1->setEnabled(false);
            my_w.radioNumber2->setChecked(true);
            my_w.radioImage2->setEnabled(false);
            my_w.image2->setEnabled(false);
        } else {
            my_w.radioImage1->setChecked(true);
            my_w.radioNumber1->setEnabled(false);
            my_w.num1->setEnabled(false);
            my_w.second->setEnabled(false);
        }
    }
}

void Math_operations::doOperation () {
    saveDefaults();
    if(currentBuffer) {
        nPhysD *image1=getPhysFromCombo(my_w.image1);
        nPhysD *image2=getPhysFromCombo(my_w.image2);

        for (int k=0;k<my_w.operation->count();k++) {
            DEBUG("Operator " << k << " " << my_w.operation->currentIndex() << " " << separator[0] << " " << separator[1] << " " << my_w.operation->itemText(k).toStdString());
        }
        DEBUG("Operator " << my_w.operation->currentIndex() << " " << separator[0] << " " << separator[1] << " " << my_w.operation->currentText().toStdString());
        nPhysD *myresult=NULL;
        if (my_w.operation->currentIndex() < separator[0]) { // two images neeeded
            nPhysD *operand1=NULL;
            nPhysD *operand2=NULL;

            if (my_w.radioNumber1->isChecked() && my_w.radioNumber2->isChecked()) {
                my_w.statusbar->showMessage("One operand must be an image",5000);
                return;
            }

            if (my_w.radioNumber1->isChecked() && image2) {
                double val=QLocale().toDouble(my_w.num1->text());
                qDebug() <<"radio 1" << val;
                operand1=new nPhysD(image2->getW(),image2->getH(),val,my_w.num1->text().toStdString());
                operand1->set_origin(image2->get_origin());
                operand1->set_scale(image2->get_scale());
            }

            if (my_w.radioNumber2->isChecked() && image1) {
                double val=QLocale().toDouble(my_w.num2->text());
                qDebug() <<"radio 2" << val;
                operand2=new nPhysD(image1->getW(),image1->getH(),val,my_w.num2->text().toStdString());
                operand2->set_origin(image1->get_origin());
                operand2->set_scale(image1->get_scale());
            }

            if (my_w.radioImage1->isChecked()) {
                operand1=new nPhysD(*image1);
            }
            if (my_w.radioImage2->isChecked()) {
                operand2=new nPhysD(*image2);
            }

            if (operand1->get_scale() != operand2->get_scale())  {
                QMessageBox::warning(this,tr("Scale problem"),tr("The images do not have the same scale"),QMessageBox::Ok);
            }

            QRectF r1=QRectF(-operand1->get_origin().x(),-operand1->get_origin().y(),operand1->getW(),operand1->getH());
            QRectF r2=QRectF(-operand2->get_origin().x(),-operand2->get_origin().y(),operand2->getW(),operand2->getH());

            // obs
            QRectF rTot=r1.intersected(r2);;
            qDebug() << my_w.operation->currentText();

            myresult=new nPhysD(rTot.width(),rTot.height(), 1.0);
            myresult->setName(my_w.operation->currentText().toStdString()+" "+operand1->getName()+" "+operand2->getName());
            myresult->setFromName(operand1->getFromName()+" "+operand2->getFromName());
            myresult->set_origin(-rTot.left(),-rTot.top());
            vec2f shift1=-(myresult->get_origin() - operand1->get_origin());
            vec2f shift2=-(myresult->get_origin() - operand2->get_origin());
            for (size_t j=0; j<myresult->getH(); j++) {
                for (size_t i=0; i<myresult->getW(); i++) {
                    double val1=operand1->point(i+shift1.x(),j+shift1.y());
                    double val2=operand2->point(i+shift2.x(),j+shift2.y());
                    if (std::isfinite(val1) && std::isfinite(val2)) {
                        switch (my_w.operation->currentIndex()) {
                            case 0:
                                myresult->set(i,j,val1+val2);
                                break;
                            case 1:
                                myresult->set(i,j,val1-val2);
                                break;
                            case 2:
                                myresult->set(i,j,val1*val2);
                                break;
                            case 3:
                                myresult->set(i,j,val1/val2);
                                break;
                            case 4:
                                myresult->set(i,j,0.5*(val1+val2));
                                break;
                            case 5:
                                myresult->set(i,j,std::min(val1,val2));
                                break;
                            case 6:
                                myresult->set(i,j,std::max(val1,val2));
                                break;
                            case 7:
                                myresult->set(i,j,fmod(val1,val2));
                                break;
                        }
                    } else {
                        myresult->set(i,j,std::isfinite(val1)?val1:val2);
                    }

                }
            }
            delete operand1;
            delete operand2;
        } else 	if (my_w.operation->currentIndex() < separator[1]) { // 1 image and 1(or more) scalars
            if (image1) {
                if (my_w.operation->currentIndex()==separator[0]+1) {
                    bool ok;
                    double scalar=QLocale().toDouble(my_w.num2->text(),&ok);
                    if (ok) {
                        myresult=new nPhysD(*image1);
                        physMath::phys_pow(*myresult, scalar);
                    } else {
                        my_w.statusbar->showMessage(tr("ERROR: Expected 2 values"));
                    }
                } else if (my_w.operation->currentIndex()==separator[0]+2) {
                    QStringList scalars=my_w.num2->text().split(" ");
                    if  (scalars.size()==1){
                        bool ok;
                        double scalar=QLocale().toDouble(my_w.num2->text(),&ok);
                        if (ok) {
                            myresult=new nPhysD(*image1);
                            physMath::phys_fast_gaussian_blur(*myresult, scalar);
                        } else {
                            my_w.statusbar->showMessage(tr("ERROR: Exepcted a float radius"));
                        }
                    } else if (scalars.size()==2) {
                        bool ok1, ok2;
                        double scalar1=QLocale().toDouble(scalars.at(0),&ok1);
                        double scalar2=QLocale().toDouble(scalars.at(1),&ok2);
                        if(ok1 && ok2) {
                            myresult=new nPhysD(*image1);
                            physMath::phys_fast_gaussian_blur(*myresult, scalar1, scalar2);
                        } else {
                            my_w.statusbar->showMessage(tr("ERROR: Exepcted 2 values"));
                        }
                    }
                } else if (my_w.operation->currentIndex()==separator[0]+3) {
                    QStringList scalars=my_w.num2->text().split(" ");
                    if  (scalars.size()==1){
                        bool ok;
                        int scalar=my_w.num2->text().toInt(&ok);
                        if (ok) {
                            myresult=new nPhysD(*image1);
                            physMath::phys_median_filter(*myresult, scalar);
                        } else {
                            my_w.statusbar->showMessage(tr("ERROR: Scalar should be an integer"));
                        }
                    }
                } else if (my_w.operation->currentIndex()==separator[0]+4) {
                    bool ok;
                    double scalar=QLocale().toDouble(my_w.num2->text(),&ok);
                    if (ok) {
                        myresult=new nPhysD(image1->rotated(scalar));
                    } else {
                        my_w.statusbar->showMessage(tr("ERROR: Value should be a float"));
                    }
                } else if (my_w.operation->currentIndex()==separator[0]+5) {
                    bool ok;
                    double scalar=QLocale().toDouble(my_w.num2->text(),&ok);
                    if (ok) {
                        myresult=new nPhysD(*image1);
                        physMath::phys_gauss_laplace(*myresult,scalar);
                    } else {
                        my_w.statusbar->showMessage(tr("ERROR: Value should be a float"));
                    }
                } else if (my_w.operation->currentIndex()==separator[0]+6) {
                    bool ok;
                    double scalar=QLocale().toDouble(my_w.num2->text(),&ok);
                    if (ok) {
                        myresult=new nPhysD(*image1);
                        physMath::phys_gauss_sobel(*myresult,scalar);
                    } else {
                        my_w.statusbar->showMessage(tr("ERROR: Value should be a float"));
                    }
                } else if (my_w.operation->currentIndex()==separator[0]+7) {
                    bool ok;
                    double scalar=QLocale().toDouble(my_w.num2->text(),&ok);
                    if (ok) {
                        myresult=new nPhysD(*image1);
                        physMath::phys_integratedNe(*myresult,scalar);
                    } else {
                        my_w.statusbar->showMessage(tr("ERROR: Value should be a float"));
                    }
                } else if (my_w.operation->currentIndex()==separator[0]+8) {
                    bool ok;
                    double scalar=QLocale().toDouble(my_w.num2->text(),&ok);
                    if (ok) {
                        myresult=new nPhysD(*image1);
                        physMath::phys_remainder(*myresult,scalar);
                    } else {
                        my_w.statusbar->showMessage(tr("ERROR: Value should be a float"));
                    }
                } else if (my_w.operation->currentIndex()==separator[0]+9) { //Resize
                    QStringList scalars=my_w.num2->text().split(" ");
                    if (scalars.size()==1) {
                        bool ok1;
                        double scalar1=QLocale().toDouble(scalars.at(0),&ok1);
                        if (ok1) {
                            vec2i newsize= image1->getSize()*scalar1;
                            myresult=new nPhysD(physMath::phys_resample(*image1, newsize));
                        } else {
                            my_w.statusbar->showMessage(tr("ERROR: Exepcted 1 double"));
                        }
                    } else if (scalars.size()==2) {
                        bool ok1, ok2;
                        int scalar1=QLocale().toInt(scalars.at(0),&ok1);
                        int scalar2=QLocale().toInt(scalars.at(1),&ok2);
                        if (ok1 && ok2) {
                            myresult=new nPhysD(physMath::phys_resample(*image1, vec2i(scalar1, scalar2)));
                        } else {
                            my_w.statusbar->showMessage(tr("ERROR: Exepcted 2 int sperated by space"));
                        }
                    } else {
                        my_w.statusbar->showMessage(tr("ERROR: Exepcted 2 values"));
                    }
                } else if (my_w.operation->currentIndex()==separator[0]+10) { //add noise
                    bool ok;
                    double scalar=QLocale().toDouble(my_w.num2->text(),&ok);
                    if (ok) {
                        myresult=new nPhysD(*image1);
                        physMath::add_noise(*myresult,scalar);
                    } else {
                        my_w.statusbar->showMessage(tr("ERROR: Value should be a float"));
                    }
                }
            }

        } else { // 1 image and 1(or more) scalars
            myresult=new nPhysD(*image1);
            myresult->setName(image1->getName());
            if (my_w.operation->currentIndex()==separator[1]+1) {
                physMath::phys_transpose(*myresult);
            } else if (my_w.operation->currentIndex()==separator[1]+2) {
                physMath::phys_log10(*myresult);
            } else if (my_w.operation->currentIndex()==separator[1]+3) {
                physMath::phys_log(*myresult);
            } else if (my_w.operation->currentIndex()==separator[1]+4) {
                physMath::phys_abs(*myresult);
            } else if (my_w.operation->currentIndex()==separator[1]+5) {
                physMath::phys_square(*myresult);
            } else if (my_w.operation->currentIndex()==separator[1]+6) {
                physMath::phys_sqrt(*myresult);
            } else if (my_w.operation->currentIndex()==separator[1]+7) {
                physMath::phys_sin(*myresult);
            } else if (my_w.operation->currentIndex()==separator[1]+8) {
                physMath::phys_cos(*myresult);
            } else if (my_w.operation->currentIndex()==separator[1]+9) {
                physMath::phys_tan(*myresult);
            } else if (my_w.operation->currentIndex()==separator[1]+10) {
                physMath::phys_laplace(*myresult);
            } else if (my_w.operation->currentIndex()==separator[1]+11) {
                physMath::phys_sobel(*myresult);
            }
        }
        qDebug() << "here";
        if (myresult) {
            myresult->reset_display();
            myresult->TscanBrightness();
            myresult->setShortName(my_w.operation->currentText().toStdString());
            if (myresult->getSurf()>0) operatorResult=nparent->replacePhys(myresult,operatorResult);
        }
    }
   qDebug() << "here";
}

