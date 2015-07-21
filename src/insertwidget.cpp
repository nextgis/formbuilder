/******************************************************************************
 *
 * Name:     insertwidget.cpp
 * Project:  NextGIS Form Builder
 * Purpose:  Widget class for moving interface elements over the device screen.
 * Author:   NextGIS
 *
 ******************************************************************************
 * Copyright (c) 2014, 2015 NextGIS
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#include "fb.h"


FBInsertWidget* FBInsertWidget::SHOWED = NULL;
FBInsertWidget::FBInsertWidget(QWidget* parent): QWidget(parent)
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setMinimumHeight(13);
    this->setHideStyle();
}

void FBInsertWidget::setShowStyle()
{
    this->setStyleSheet("QWidget"
                        "{"
                            "background-color: rgba(0,0,0,0);"
                            "border: 2px dashed red;"
                        "}");
}

void FBInsertWidget::setHideStyle()
{
    this->setStyleSheet("QWidget"
                        "{"
                            "background-color: rgba(0,0,0,0);"
                            "border: 2px dashed rgba(0,0,0,0);"
                        "}");
}

//http://stackoverflow.com/questions/18344135/why-do-
//stylesheets-not-work-when-subclassing-qwidget-and-using-q-object
void FBInsertWidget::paintEvent(QPaintEvent *pe)
{
  QStyleOption o;
  o.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
}
