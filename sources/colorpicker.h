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

#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <QtGui/QWidget>

namespace Ui {
    class ColorPicker;
}

class ColorPicker : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_DISABLE_COPY(ColorPicker)
public:
    explicit ColorPicker(QWidget *parent = 0);
    virtual ~ColorPicker();
    QColor color();
    QColor backColor();

private slots:
    void setStyle(int);
    void colorFromRing(QColor);

public slots:
    void setColor(QColor);
    void setPickerStyle(int);

signals:
    void colorChanged(QColor);
    void backColorChanged(QColor);

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::ColorPicker *m_ui;
    int style;
};

#endif // COLORPICKER_H
