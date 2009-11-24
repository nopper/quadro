/* Qaquarelle
 * Copyright (C) 2009 Vasiliy Makarov <drmoriarty.0@gmail.com>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this software; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "colorpicker.h"
#include "ui_colorpicker.h"

ColorPicker::ColorPicker(QWidget *parent) :
    QWidget(parent), m_ui(new Ui::ColorPicker)
{
    m_ui->setupUi(this);

    connect(m_ui->circle, SIGNAL(colorChanged(QColor)), this, SLOT(colorFromRing(QColor)));
    setStyle(1);
    m_ui->circle->setColor(QColor("magenta"));
}

ColorPicker::~ColorPicker()
{
    delete m_ui;
}

QColor ColorPicker::color()
{
    return m_ui->circle->color();
}

void ColorPicker::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ColorPicker::setStyle(int st)
{
    style = st;
    m_ui->bar_1->disconnect();
    m_ui->bar_2->disconnect();
    m_ui->bar_3->disconnect();
    if(st) {  // RGB
        m_ui->label_1->setText("R");
        m_ui->label_2->setText("G");
        m_ui->label_3->setText("B");
        connect(m_ui->bar_1, SIGNAL(positionChanged(float)), m_ui->circle, SLOT(setR(float)));
        connect(m_ui->bar_2, SIGNAL(positionChanged(float)), m_ui->circle, SLOT(setG(float)));
        connect(m_ui->bar_3, SIGNAL(positionChanged(float)), m_ui->circle, SLOT(setB(float)));
        m_ui->bar_1->setRgbStyle(true);
        m_ui->bar_2->setRgbStyle(true);
        m_ui->bar_3->setRgbStyle(true);
    } else {  // HSV
        m_ui->label_1->setText("H");
        m_ui->label_2->setText("S");
        m_ui->label_3->setText("V");
        connect(m_ui->bar_1, SIGNAL(positionChanged(float)), m_ui->circle, SLOT(setH(float)));
        connect(m_ui->bar_2, SIGNAL(positionChanged(float)), m_ui->circle, SLOT(setS(float)));
        connect(m_ui->bar_3, SIGNAL(positionChanged(float)), m_ui->circle, SLOT(setV(float)));
        m_ui->bar_1->setRgbStyle(false);
        m_ui->bar_2->setRgbStyle(false);
        m_ui->bar_3->setRgbStyle(false);
    }
    colorFromRing(m_ui->circle->color());
}

void ColorPicker::colorFromRing(QColor col)
{
    if(style) { // RGB
        m_ui->bar_1->setBeginColor(QColor(0, col.green(), col.blue()));
        m_ui->bar_1->setEndColor(QColor(255, col.green(), col.blue()));
        m_ui->bar_2->setBeginColor(QColor(col.red(), 0, col.blue()));
        m_ui->bar_2->setEndColor(QColor(col.red(), 255, col.blue()));
        m_ui->bar_3->setBeginColor(QColor(col.red(), col.green(), 0));
        m_ui->bar_3->setEndColor(QColor(col.red(), col.green(), 255));

        m_ui->bar_1->setPosition(col.redF());
        m_ui->bar_2->setPosition(col.greenF());
        m_ui->bar_3->setPosition(col.blueF());
    } else {    // HSV
        m_ui->bar_1->setBeginColor(QColor::fromHsv(0, 255, 255));
        m_ui->bar_1->setEndColor(QColor::fromHsv(359, 255, 255));

        int hue = col.hue() % 360;
        m_ui->bar_2->setBeginColor(QColor::fromHsv(hue, 0, col.value()));
        m_ui->bar_2->setEndColor(QColor::fromHsv(hue, 255, col.value()));
        m_ui->bar_3->setBeginColor(QColor::fromHsv(hue, col.saturation (), 0));
        m_ui->bar_3->setEndColor(QColor::fromHsv(hue, col.saturation (), 255));

        m_ui->bar_1->setPosition(col.hueF());
        m_ui->bar_2->setPosition(col.saturationF());
        m_ui->bar_3->setPosition(col.valueF());
    }

    emit colorChanged (col);
}

void ColorPicker::setPickerStyle(int i)
{
    m_ui->circle->setStyle(i);
}

void ColorPicker::setColor(QColor col)
{
    m_ui->circle->setColor(col);
}
