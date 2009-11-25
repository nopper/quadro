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

#include "colorring.h"
#include "math.h"
//#include "cmath"


static const QPointF points[3] = {
	QPointF(1.0, 0.0),
	QPointF(-0.5, 0.866),
	QPointF(-0.5, -0.866)
};

static const QPointF points2[4] = {
	QPointF(.0, -1.0),
	QPointF(-1.0, 0.0),
	QPointF(.0, 1.0),
	QPointF(1.0, 0.0)
};

ColorRing::ColorRing(QWidget *parent) :QWidget(parent), RING(.8f), RING2(RING * RING), grabring(false), grabtriangle(false)
{
	col = QColor(120, 140, 160);
	//triangleMode = false;
	setStyle(0);
}

float ColorRing::r() const
{
	return col.redF();
}

float ColorRing::g() const
{
	return col.greenF();
}

float ColorRing::b() const
{
	return col.blueF();
}

float ColorRing::h() const
{
	return col.hueF();
}

float ColorRing::s() const
{
	return col.saturationF();
}

float ColorRing::v() const
{
	return col.valueF();
}

QColor ColorRing::color() const
{
	return col;
}

void ColorRing::setR(float f)
{
	QColor c2 = col;
	col.setRedF(f);
	if(col.redF() != c2.redF()) {
		emit rChanged(col.redF());
		emit colorChanged(col);
		repaint();
	}
}

void ColorRing::setG(float f)
{
	QColor c2 = col;
	col.setGreenF(f);
	if(col.greenF() != c2.greenF()) {
		emit gChanged(col.greenF());
		emit colorChanged(col);
		repaint();
	}
}

void ColorRing::setB(float f)
{
	QColor c2 = col;
	col.setBlueF(f);
	if(col.blueF() != c2.blueF()) {
		emit bChanged(col.blueF());
		emit colorChanged(col);
		repaint();
	}
}

void ColorRing::setH(float f)
{
	QColor c2 = col;
	col.setHsvF(f, col.saturationF(), col.valueF());
	if(col.hueF() != c2.hueF()) {
		emit hChanged(col.hueF());
		emit colorChanged(col);
		repaint();
	}
}

void ColorRing::setS(float f)
{
	QColor c2 = col;
	col.setHsvF(col.hueF(), f, col.valueF());
	if(col.saturationF() != c2.saturationF()) {
		emit sChanged(col.saturationF());
		emit colorChanged(col);
		repaint();
	}
}

void ColorRing::setV(float f)
{
	QColor c2 = col;
	col.setHsvF(col.hueF(), col.saturationF(), f);
	if(col.valueF() != c2.valueF()) {
		emit vChanged(col.valueF());
		emit colorChanged(col);
		repaint();
	}
}

void ColorRing::setColor(QColor c)
{
	if(col != c) {
		col = c;
		emit colorChanged(col);
		repaint();
	}
}

void ColorRing::setStyle(int i)
{
	switch(i) {
	case 0:
		triangleMode = true;
		break;
	case 1:
	default:
		triangleMode = false;
		break;
	}
}

void ColorRing::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	QRect r = contentsRect();
	r.moveTo(-r.width()/2, -r.height()/2);
	painter.translate(r.width()/2, r.height()/2);
	//painter.fillRect(r, QApplication::palette().window().color());
	QConicalGradient gr(0, 0, 360);
	for(int i = 0; i< 12; i++) {
		gr.setColorAt(float(i)/12, QColor::fromHsvF(float(i)/12, 1.f, 1.f));
	}
	painter.setBrush(QBrush(gr));
	painter.setPen(Qt::NoPen);
	painter.drawPie(r, 0, 5760);
	
	painter.scale(RING, RING);
	painter.setBrush(QApplication::palette().window().color());
	painter.drawPie(r, 0, 5760);
	painter.rotate(-col.hueF() * 360);
	painter.scale(r.width()*.5f, r.height()*.5f);

	painter.setPen(Qt::SolidLine);
	painter.setPen(QPen(QColor("white")));
	painter.setBrush(QBrush(QColor(0,0,0,0)));
	painter.drawEllipse(QPointF(1.12f, .0f), 0.1f, 0.1f);
	painter.setPen(Qt::NoPen);

	if(triangleMode) {
		QConicalGradient gr2(points[2], 270.f);
		gr2.setColorAt(.0f, QColor::fromHsvF(col.hueF(), .0f, 1.f));
		gr2.setColorAt(1.f/6.f, QColor::fromHsvF(col.hueF(), 1.f, 1.f));
		painter.setBrush(QBrush(gr2));
		painter.drawPolygon(points, 3);
		//painter.setCompositionMode(QPainter::CompositionMode_Multiply);
		QLinearGradient gr3(points[2], (points[0]+points[1])*.5f);
		gr3.setColorAt(.0f, QColor::fromRgbF(.0f, .0f, .0f, 1.f));
		gr3.setColorAt(1.f, QColor::fromRgbF(.0f, .0f, .0f, 0.f));
		painter.setBrush(QBrush(gr3));
		painter.drawPolygon(points, 3);
		//painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

		QPointF p1 = points[0] * col.saturationF() + points[1] * (1.f - col.saturationF());
		QPointF p2 = p1 * col.valueF() + points[2] * (1.f - col.valueF());
		painter.setPen(Qt::SolidLine);
		painter.setPen(QPen(QColor("white")));
		painter.setBrush(QBrush(QColor(0,0,0,0)));
		painter.drawEllipse(p2, 0.1f, 0.1f);
	} else { // box mode
		QLinearGradient gr2(points2[0], points2[1]);
		gr2.setColorAt(.0f, QColor::fromHsvF(col.hueF(), 1.f, 1.f));
		gr2.setColorAt(1.f, QColor::fromHsvF(col.hueF(), .0f, 1.f));
		painter.setBrush(QBrush(gr2));
		painter.drawPolygon(points2, 4);
		QLinearGradient gr3(points2[1], points2[2]);
		gr3.setColorAt(.0f, QColor::fromRgbF(.0f, .0f, .0f, 1.f));
		gr3.setColorAt(1.f, QColor::fromRgbF(.0f, .0f, .0f, 0.f));
		painter.setBrush(QBrush(gr3));
		painter.drawPolygon(points2, 4);

		QPointF p1 = points2[0] * col.saturationF() + points2[1] * (1.f - col.saturationF());
		QPointF p2 = p1 + (points2[2] - points2[1]) * col.valueF();
		painter.setPen(Qt::SolidLine);
		painter.setPen(QPen(QColor("white")));
		painter.setBrush(QBrush(QColor(0,0,0,0)));
		painter.drawEllipse(p2, 0.1f, 0.1f);
	}
}

void ColorRing::mousePressEvent ( QMouseEvent * event )
{
	float x = event->x(), y = event->y();
	x = ((x/width()) - .5f) * 2;
	y = ((y/height()) - .5f) * 2;
	grabring = false;
	grabtriangle = false;
	setColorFromPoint(x, y);
}

void ColorRing::mouseMoveEvent ( QMouseEvent * event )
{
	float x = event->x(), y = event->y();
	x = ((x/width()) - .5f) * 2;
	y = ((y/height()) - .5f) * 2;
	setColorFromPoint(x, y);
}

void ColorRing::setColorFromPoint(float x, float y)
{
	float r2 = x*x + y*y;
	QRect r = contentsRect();
	if(grabring && grabtriangle) {
		grabring = grabtriangle = false;
	}
	if(!grabring && !grabtriangle) {
		if(r2 > RING2) {
			grabring = true;
		} else {
			grabtriangle = true;
		}
	}
	if(grabring) {
		float angle = (atan2f(y,-x)/M_PI);
		angle = (angle * 0.5f) + 0.5f;
		col.setHsvF(angle, col.saturationF(), col.valueF());
	} else if(grabtriangle) {
		QMatrix m;
		m.scale(1.f/RING, 1.f/RING);
		m.rotate(col.hueF() * 360);
		if(triangleMode) {
			QPointF p = m.map(QPointF(x, y)), p2;
			QLineF satL(points[0], points[1]);
			QLineF l1(points[2], p);
			switch(satL.intersect(l1, &p2)) {
			case QLineF::BoundedIntersection:
			case QLineF::UnboundedIntersection:
				{
					float sat = QLineF(points[1], p2).length()/satL.length();
					if(satL.length() < QLineF(points[0], p2).length()) sat = 0;
					float val = QLineF(points[2], p).length()/QLineF(points[2], p2).length();
					if(QLineF(p, p2).length() > QLineF(points[2], p2).length()) val = .0f;
					if(sat > 1.f) sat = 1.f;
					if(sat < .0f) sat = 0.f;
					if(val > 1.f) val = 1.f;
					if(val < .0f) val = .0f;
					col.setHsvF(col.hueF(), sat, val);
					break;
				}
			case QLineF::NoIntersection:
				break;
			}
		} else { // box mode
			QMatrix m2;
			m2.rotate(45);
			m.rotate(45);
			QPointF p = m.map(QPointF(x, y)), p1  = m2.map(points2[1]), p2 = m2.map(points2[2]), p0 = m2.map(points2[0]);
			float sat = (p.x() - p1.x()) / (p0.x() - p1.x());
			float val = (p.y() - p1.y()) / (p2.y() - p1.y());
			if(sat > 1.f) sat = 1.f;
			if(sat < .0f) sat = 0.f;
			if(val > 1.f) val = 1.f;
			if(val < .0f) val = .0f;
			col.setHsvF(col.hueF(), sat, val);
			//QMessageBox::information(0, "", QString("x=%1, y=%2, p1.x=%3, p1.y=%4, p2.x=%5, p2.y=%6").arg(QString::number(p.x())).arg(QString::number(p.y())).arg(QString::number(p1.x())).arg(QString::number(p1.y())).arg(QString::number(p2.x())).arg(QString::number(p2.y())));
		}
	}
	emit hChanged(col.hueF());
	emit colorChanged(col);
	repaint();
}
