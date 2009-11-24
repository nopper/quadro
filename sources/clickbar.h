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

#ifndef CLICKBAR_H
#define CLICKBAR_H

#include <QWidget>

class ClickBar : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(float position READ position WRITE setPosition)
    Q_PROPERTY(QColor beginColor READ beginColor WRITE setBeginColor)
    Q_PROPERTY(QColor endColor READ endColor WRITE setEndColor)
    Q_PROPERTY(bool rgbStyle READ rgbStyle WRITE setRgbStyle)
 public:
    ClickBar(QWidget *parent = 0);
    float position() const;
    QColor beginColor() const;
    QColor endColor() const;
	bool rgbStyle() const;
	void setBounds(float min, float max);

 public slots:
    void setPosition(float);
    void setBeginColor(QColor);
    void setEndColor(QColor);
    void setRgbStyle(bool);

 signals:
    void positionChanged(float);
    void beginColorChanged(QColor);
    void endColorChanged(QColor);
    void styleChanged(bool);

 protected:
     virtual void paintEvent(QPaintEvent *event);
	 virtual void mousePressEvent ( QMouseEvent * event );
	 virtual void mouseMoveEvent ( QMouseEvent * event );

 private:
    QColor begin, end;
    float pos, min, max;
    bool rgbstyle;
};

#endif // CLICKBAR_H
