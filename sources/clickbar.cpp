#include "clickbar.h"
#include <QtGui>

ClickBar::ClickBar(QWidget *parent) : QWidget(parent)
{
	min = 0.0f;
	max = 1.0f;
	begin = QColor(0, 0, 0);
	end = QColor(255, 255, 255);
	pos = .5f;
	rgbstyle = true;
}

float ClickBar::position() const {
    return min + pos*(max - min);
}

QColor ClickBar::beginColor() const {
    return begin;
}

QColor ClickBar::endColor() const {
    return end;
}

bool ClickBar::rgbStyle() const {
	return rgbstyle;
}

void ClickBar::setPosition(float p)
{
	float p2 = (p - min) / (max - min);
	if(p2 != pos) {
		pos = p2;
		emit positionChanged(position());
		repaint();
	}
}

void ClickBar::setBeginColor(QColor b)
{
	if(b != begin) {
		begin = b;
		emit beginColorChanged(begin);
		repaint();
	}
}

void ClickBar::setEndColor(QColor e)
{
	if(end != e) {
		end = e;
		emit endColorChanged(end);
		repaint();
	}
}

void ClickBar::setRgbStyle(bool s)
{
	if(rgbstyle != s) {
		rgbstyle = s;
		emit styleChanged(rgbstyle);
		repaint();
	}
}

void ClickBar::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);
	QPainter painter(this);
	QRect r = contentsRect();
	//painter.setBrush(QApplication::palette().window().color());
	//painter.drawRect(r);
	QLinearGradient gr(r.bottomLeft(), r.bottomRight());
	//gr.setCoordinateMode(QGradient::ObjectBoundingMode);
	if(rgbstyle) {
		gr.setColorAt(.0f, begin);
		gr.setColorAt(1.f, end);
	} else {
		float h = end.hueF() - begin.hueF(), s = end.saturationF() - begin.saturationF(), v = end.valueF() - begin.valueF();
		float h0 = begin.hueF(), s0 = begin.saturationF(), v0 = begin.valueF();
		for(int i = 0; i < 10; i++) {
			float d = float(i)/10;
			gr.setColorAt(d, QColor::fromHsvF(h0+h*d, s0+s*d, v0+v*d));
		}
	}
	//painter.setBrush(QBrush(gr));
	QRect r2 = r;
        r2.setHeight ((int)(r.height() * .5f));
	painter.fillRect(r2, QBrush(gr));
        r.setWidth ((int)(r.width() * pos));
	//painter.setPen(Qt::NoPen);
	painter.fillRect(r, QBrush(gr));
	//painter.setBrush(QApplication::palette().highlight().color());
	//painter.drawRect(contentsRect());
}

void ClickBar::mousePressEvent ( QMouseEvent * event )
{
	if(width()) {
		pos = float(event->x())/width();
		emit positionChanged(pos);
		repaint();
	}
}

void ClickBar::mouseMoveEvent ( QMouseEvent * event )
{
	pos = float(event->x()) / width();
	emit positionChanged(pos);
	repaint();
}

void ClickBar::setBounds(float _min, float _max)
{
	if(_min != min || _max != max) {
		min = _min;
		max = _max;
		emit positionChanged(position());
	}
}
