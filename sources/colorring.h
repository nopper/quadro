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


#ifndef COLORRING_H
#define COLORRING_H

#include <QtGui>

class ColorRing: public QWidget
{
	Q_OBJECT
	Q_PROPERTY(float color_r READ r WRITE setR)
	Q_PROPERTY(float color_g READ g WRITE setG)
	Q_PROPERTY(float color_b READ b WRITE setB)
	Q_PROPERTY(float color_h READ h WRITE setH)
	Q_PROPERTY(float color_s READ s WRITE setS)
	Q_PROPERTY(float color_v READ v WRITE setV)
	Q_PROPERTY(QColor color READ color WRITE setColor)
	
 public:
	ColorRing(QWidget *parent = 0);
	float r() const;
	float g() const;
	float b() const;
	float h() const;
	float s() const;
	float v() const;
	QColor color() const;

 public slots:
	void setR(float);
	void setG(float);
	void setB(float);
	void setH(float);
	void setS(float);
	void setV(float);
	void setColor(QColor);
	void setStyle(int);

 signals:
	void rChanged(float);
	void gChanged(float);
	void bChanged(float);
	void hChanged(float);
	void sChanged(float);
	void vChanged(float);
	void colorChanged(QColor);

 protected:
     virtual void paintEvent(QPaintEvent *event);
	 virtual void mousePressEvent ( QMouseEvent * event );
	 virtual void mouseMoveEvent ( QMouseEvent * event );

 private:
	 QColor col;
	 void setColorFromPoint(float x, float y);
	 const float RING, RING2;
	 bool grabring, grabtriangle;
	 bool triangleMode;
};

#endif //COLORRING_H
