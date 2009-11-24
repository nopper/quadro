/* Quadro
 * Copyright (C) 2009 Francesco Piccinno <stack.box@gmail.com>
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

#ifndef QUADROAREA_H
#define QUADROAREA_H

#include <QtCore>
#include <QtGui>

enum ToolEnum
{
    ERASER = 0,
    BRUSH1 = 1,
    BRUSH2 = 2,
    PICKER = 3,
    ZOOMER = 4,
};

class QuadroLayer
{
public:
    QuadroLayer(const QString &name, int opacity, bool active, \
                const QSize &size, const QColor &color);

    QuadroLayer(const QString &name, int opacity, bool active, QImage image)
    {
        mName = name;
        mOpacity = opacity;
        mActive = active;
        mImage = image.copy();
        mOrigImage = mImage.copy();
    }

    int opacity() { return mOpacity; }
    void setOpacity(int value) { mOpacity = value; }

    bool isActive() { return mActive; }
    void setActive(bool value) { mActive = value; }

    QString name() const { return mName; }
    void setName(const QString& name) { mName = name; }

    QImage* image() { return &mImage; }
    QImage* originalImage() { return &mOrigImage; }

    const QSize size() { return mOrigImage.size(); }
private:
    int mOpacity;
    bool mActive;
    QString mName;

    QImage mImage,
           mOrigImage;
};

typedef struct {
    int x1, y1, x2, y2;
} Line;

class QuadroStroke
{
public:
    QuadroStroke(ToolEnum tool, int width, const QColor &color)
    {
        mTool = tool;
        mWidth = width;
        mColor = color;
        mLines.clear();
    }

    ~QuadroStroke()
    {
        mLines.clear();
    }

    void addLine(int x1, int y1, int x2, int y2)
    {
        Line line;
        line.x1 = x1; line.y1 = y1;
        line.x2 = x2; line.y2 = y2;
        mLines.append(line);
    }

    ToolEnum mTool;
    int mWidth;
    QColor mColor;
    QList <Line> mLines;
};

class QuadroArea : public QWidget
{
    Q_OBJECT
public:
    QuadroArea(QWidget *parent = 0);
    QuadroArea(const QSize &size, QWidget *parent = 0);
    QuadroArea(const QString &fileName, QWidget *parent = 0);
    ~QuadroArea();

    void setTool(ToolEnum tool, bool reg = true);
    void setTool(ToolEnum tool, int width, const QColor &color);
    void unsetTool();
    ToolEnum tool() { return mTool; }

    void setBrushWidth(int width);
    int brushWidth() { return mBrushWidth; }

    void setBrushColor(const QColor &color) { mColor = color; }
    const QColor brushColor() { return mColor; }

    void setZoomFactor(float f);
    float zoomFactor() { return mZoomFactor; }

    QList<QuadroLayer*>& layers() { return mLayers; }
    QList<QuadroStroke*>& undoes() { return mUndoes; }

    void setActiveLayer(int layer);
    int activeLayer() { return mCurLayer; }

    void drawPath(const QPainterPath &path);
    void updateSlider();

    void clearImage()
    {
        mLayers.at(mCurLayer)->image()->fill(qRgba(255, 255, 255, 255));
    }

    bool isModified() { return mModified; }

    void saveTo(const QString &fileName);
    void open(const QString &fileName);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private slots:
    void switchPickMode(const QPoint &point);

    bool undo();
    bool redo();

public slots:
    void zoomSliderChanged(int value);

private:
    void cleanUp();
    void createUi();
    void resizeImage(QImage *image, const QSize& size);

    void modifyImage(QImage *image, int x, int y);
    void drawLine(QImage *image, int x1, int y1, int x2, int y2);

    void applyStroke(QImage *image, const QuadroStroke *stroke);

    void drawLineTo(const QPoint& endPoint);
    void pickColorAt(const QPoint &);
    void setZoomRect(const QPoint &point);
    void drawPicker(QPainter &painter, const QPoint &);

    QPoint convertPos(const QPoint &point);
    void loadTools();

    QColor mColor;
    QPoint mLastPoint;

    QImage mBrush;

    int mCurLayer;
    QImage *mImage,
           *mOrigImage;
    QList<QuadroLayer*> mLayers;

    int mBrushWidth;
    ToolEnum mTool;
    ToolEnum mPrevTool;
    QList <QImage> mPatterns;

    bool mTracking;
    bool mModified;

    float mZoomFactor;
    QSlider *mZoomSlider;

    int mCurUndo;
    QuadroStroke *mCurStroke;
    QList<QuadroStroke*> mUndoes;
};

#endif // QUADROAREA_H
