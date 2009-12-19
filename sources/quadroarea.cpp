#include "quadroarea.h"
#include <QtGui/QPolygonF>

#define PICKLE_WIDTH 32
#define PADDING 2
#define CENTER_DISTANCE 3
#define CENTER_OFFSET PICKLE_WIDTH / 3
#define CURSOR_SIZE 4
#define ZOOM_WIDTH 2
#define ZOOM_STEP 1.02
#define SIGN(a) ((a) > 0 ? 1 : ((a) == 0 ? 0 : -1))
#define N_UNDO_LEVEL 10
#define THRESHOLD 5

QuadroLayer::QuadroLayer(const QString &name, int opacity, bool active, \
                         const QSize &size, const QColor &color)
{
    mName = name;
    mOpacity = opacity;
    mActive = active;

    mImage = QImage(size, QImage::Format_ARGB32);
    mImage.fill(color.rgba());

    mOrigImage = mImage.copy();
}

QuadroArea::QuadroArea(QWidget *parent)
    : QWidget(parent)
{
    mCurLayer = 0;
    mLayers.append(
        new QuadroLayer(QString("Layer %1").arg(mLayers.size() + 1),
                        255, true, size(), QColor(255, 255, 255, 255)));
    setActiveLayer(0);
    createUi();
}

QuadroArea::QuadroArea(const QSize &size, QWidget *parent)
    : QWidget(parent)
{
    mCurLayer = 0;
    mLayers.append(
        new QuadroLayer(QString("Layer %1").arg(mLayers.size() + 1),
                        255, true, size, QColor(255, 255, 255, 255)));
    setActiveLayer(0);
    createUi();
}

QuadroArea::QuadroArea(const QString &fileName, QWidget *parent)
    : QWidget(parent)
{
    open(fileName);
    createUi();
}

QuadroArea::~QuadroArea()
{
    cleanUp();
}

void QuadroArea::cleanUp()
{
    QuadroLayer *layer;
    foreach (layer, mLayers)
        delete layer;

    QuadroStroke *stroke;
    foreach (stroke, mUndoes)
        delete stroke;

    mLayers.clear();
    mUndoes.clear();
}

void QuadroArea::open(const QString &fileName)
{
    cleanUp();
    QImage image;

    if (!image.load(fileName))
    {
        QRect screen_size = QApplication::desktop()->screenGeometry();
        image = QImage(screen_size.size(), QImage::Format_ARGB32);
        image.fill(qRgba(255, 255, 255, 255));
    }

    if (image.format() == QImage::Format_RGB32 || \
        image.format() == QImage::Format_RGB16)
        image.convertToFormat(QImage::Format_ARGB32_Premultiplied);

    mCurLayer = 0;
    mLayers.append(
        new QuadroLayer(QString("Layer %1").arg(mLayers.size() + 1),
                        255, true, image));
    setActiveLayer(0);

    mModified = false;
}

void QuadroArea::saveTo(const QString &fileName)
{
    QImage image(mImage->size(), QImage::Format_ARGB32);
    QPainter painter(&image);

    QuadroLayer *layer;
    foreach (layer, mLayers)
    {
        painter.setOpacity(layer->opacity());
        painter.drawImage(QPoint(0, 0), *layer->image());
    }

    image.save(fileName);

    mModified = false;
}

void QuadroArea::createUi()
{
    mTracking = false;
    mModified = false;

    mTool = ERASER;
    mBrushWidth = 20;
    mColor = Qt::blue;
    mZoomFactor = 0;

    mCurUndo = 0;
    mCurStroke = NULL;
    mUndoes.clear();

    mZoomSlider = new QSlider(Qt::Vertical, this);
    mZoomSlider->setRange(70, 1600);
    mZoomSlider->hide();

    connect(mZoomSlider, SIGNAL(valueChanged(int)),
            this, SLOT(zoomSliderChanged(int)));

    loadTools();
    setTool(BRUSH1);

    setAttribute(Qt::WA_StaticContents);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(switchPickMode(QPoint)));

    setZoomFactor(1.0);
}

void QuadroArea::drawPath(const QPainterPath &path)
{
    QPolygonF polygon;
    QPointF prev, next;

    QList<QPolygonF> list = path.toSubpathPolygons();

    for (int i = 0; i < list.size(); i++)
    {
        polygon = list.at(i);

        for (int j = 0; j < polygon.size(); j++)
        {
            next = polygon.at(j);

            if (!prev.isNull())
                drawLine(mImage, (int)prev.x(), (int)prev.y(),
                         (int)next.x(), (int)next.y());
            prev = next;
        }
    }

    update();
}

void QuadroArea::setActiveLayer(int value)
{
    mCurLayer = value;
    QuadroLayer *layer =  mLayers.at(mCurLayer);

    mImage = layer->image();
    mOrigImage = layer->originalImage();
}

void QuadroArea::mousePressEvent(QMouseEvent *event)
{
    if (event->button() & Qt::LeftButton)
    {
        mLastPoint = event->pos();
        mTracking = true;

        if (mTool < PICKER)
            mCurStroke = new QuadroStroke(mTool, mBrushWidth, mColor);
    }
}

void QuadroArea::mouseMoveEvent(QMouseEvent *event)
{
    if (mTracking)
    {
        if (mTool < PICKER)
            drawLineTo(event->pos());
        else if (mTool == PICKER)
            pickColorAt(event->pos());
    }
}

void QuadroArea::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    if (event->button() != Qt::LeftButton)
        return;

    mTracking = false;

    if (mCurStroke)
    {
        // if we have undoed our mCurUndo points in the middle of mUndoes
        // so we have to clean up the elements following mCurUndo
        if (mCurUndo < mUndoes.size())
        {
            int i = mUndoes.size() - mCurUndo;
            while (i-- > 0)
               mUndoes.removeLast();
        }
        else if (mUndoes.size() >= N_UNDO_LEVEL)
        {
            QuadroStroke *stroke = mUndoes.first();

            applyStroke(mOrigImage, stroke);
            mUndoes.removeFirst();

            delete stroke;
        }
        mCurUndo = qMin(mCurUndo + 1, N_UNDO_LEVEL);
        mUndoes.append(mCurStroke);
        mCurStroke = NULL;
    }

    if (mTool == PICKER)
    {
        setTool(mPrevTool);
        update();
    }
}

void QuadroArea::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    int xoffset = 0, yoffset = 0;
    QPainter painter(this);

    if (width() > mImage->width() * mZoomFactor)
        xoffset = (int)(width() - mImage->width() * mZoomFactor) / 2;

    if (height() > mImage->height() * mZoomFactor)
        yoffset = (int)(height() - mImage->height() * mZoomFactor) / 2;

    painter.save();
    painter.translate(xoffset, yoffset);
    painter.scale(mZoomFactor, mZoomFactor);

    // Let's draw active layers
    QuadroLayer *layer;
    foreach (layer, mLayers)
    {
        if (!layer->isActive())
            continue;
        painter.setOpacity(layer->opacity());
        painter.drawImage(QPoint(0, 0), *layer->image());
    }
    painter.restore();

    if (mTool == PICKER)
        drawPicker(painter, mLastPoint);
}

void QuadroArea::updateSlider()
{
    QRect rect = visibleRegion().boundingRect();
    int x = rect.x() + (rect.width() - mZoomSlider->width());
    mZoomSlider->move(x, rect.y() + 5);

    if (mZoomFactor != (float)1.0 || mTool == ZOOMER)
        mZoomSlider->show();
    else
        mZoomSlider->hide();
}

void QuadroArea::resizeEvent(QResizeEvent *event)
{
    updateSlider();
    update();
    QWidget::resizeEvent(event);
}

void QuadroArea::switchPickMode(const QPoint &point)
{
    mTracking = true;
    setTool(PICKER);
    pickColorAt(point);
}

bool QuadroArea::undo()
{
    if (mCurUndo == 0)
        return false;

    mImage->fill(qRgba(255, 255, 255, 255));

    QPainter painter(mImage);
    painter.drawImage(QPoint(0, 0), *mOrigImage);

    for (int i = 1; i < mCurUndo; i++)
        applyStroke(mImage, mUndoes.at(i - 1));

    mCurUndo = qMax(mCurUndo - 1, 0);
    update();

    mModified = true;

    return true;
}

bool QuadroArea::redo()
{
    if (mCurUndo == N_UNDO_LEVEL || mCurUndo == mUndoes.size())
        return false;

    applyStroke(mImage, mUndoes.at(mCurUndo));
    mCurUndo = qMin(mCurUndo + 1, N_UNDO_LEVEL);

    update();

    mModified = true;

    return true;
}

void QuadroArea::applyStroke(QImage *image, const QuadroStroke *stroke)
{
    QPoint prevPoint = mLastPoint;
    QColor prevColor = mColor;
    int prevWidth = mBrushWidth;

    setTool(stroke->mTool, false);
    setBrushColor(stroke->mColor);
    setBrushWidth(stroke->mWidth);

    Line line;
    foreach (line, stroke->mLines)
        drawLine(image, line.x1, line.y1, line.x2, line.y2);

    setTool(mTool, false);
    setBrushColor(prevColor);
    setBrushWidth(prevWidth);
}

void QuadroArea::zoomSliderChanged(int value)
{
    float f = value / 100.0;
    setZoomFactor(f);
}

void QuadroArea::resizeImage(QImage *image, const QSize& size)
{
    if (image->size() == size)
        return;

    QImage newImage(size, QImage::Format_ARGB32);
    newImage.fill(qRgba(255, 255, 255, 255));

    QPainter painter(&newImage);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
}

void QuadroArea::modifyImage(QImage *image, int x, int y)
{
    QRgb *rawImage = (QRgb *)image->scanLine(0),
         *rawBrush = (QRgb *)mBrush.scanLine(0);

    QRgb value, pbrush;
    int r, g, b;
    int nr, ng, nb, av;
    double delta;

    int width = image->width();

    if (x >= 0 && x < image->width() && y >= 0 && y < image->height())
    {
        value = image->pixel(x, y);

        r = mColor.red();
        g = mColor.green();
        b = mColor.blue();

        int a[] = {
            qMax(x - qMax(mBrushWidth / 2, 0), 0),
            qMin(x + qMax(mBrushWidth / 2, 0), image->width())
        };
        int c[] = {
            qMax(y - qMax(mBrushWidth / 2, 0), 0),
            qMin(y + qMax(mBrushWidth / 2, 0), image->height())
        };
        int cursorXStart = qMax(0, CURSOR_SIZE - x);
        int cursorYStart = qMax(0, CURSOR_SIZE - y);

        for (int j = c[0], cy = cursorYStart; j < c[1]; j++, cy++)
            for (int i = a[0], cx = cursorXStart; i < a[1]; i++, cx++)
            {
                pbrush = rawBrush[cy * mBrushWidth + cx];

                if (mTool != ERASER && (qRed(pbrush)   < THRESHOLD || \
                                        qGreen(pbrush) < THRESHOLD || \
                                        qBlue(pbrush)  < THRESHOLD))
                    continue;

                delta = (double)qRed(pbrush) / 255.;
                value = image->pixel(i, j);

                av = (int)(mColor.alpha() * delta + qAlpha(value) * (1 - delta));

                if (mTool == ERASER)
                    rawImage[j * width + i] = qRgba(255, 255, 255, av);
                else
                {
                    nr = (int)(r * delta + qRed(value)   * (1 - delta));
                    ng = (int)(g * delta + qGreen(value) * (1 - delta));
                    nb = (int)(b * delta + qBlue(value)  * (1 - delta));

                    if (nr == ng && ng == nb && nb >= 255 - THRESHOLD)
                        continue;

                    if (av <= 0 + THRESHOLD)
                        continue;

                    rawImage[j * width + i] = qRgba(nr, ng, nb, av);
                }
            }
    }
}

void QuadroArea::drawLine(QImage *image, int x1, int y1, int x2, int y2)
{
    int dx = x2 - x1;
    int dy = y2 - y1;

    if (dx == 0 && dy == 0)
        return;

    if (qAbs(dx) > qAbs(dy))
    {
        double slope = (double)dy / (double)dx;
        int s = SIGN(dx);
        for (int i = 0; i != dx; i += s)
        {
            double magicY = i * slope + y1;
            modifyImage(image, i + x1, (int)magicY);
        }
    }
    else
    {
        double slope = (double)dx / (double)dy;
        int s = SIGN(dy);
        for (int j = 0; j != dy; j += s)
        {
            double magicX = j * slope + x1;
            modifyImage(image, (int)magicX, j + y1);
        }
    }
}

void QuadroArea::drawLineTo(const QPoint &endPoint)
{
    QPoint point = convertPos(mLastPoint);

    int x1 = point.x();
    int y1 = point.y();

    point = convertPos(endPoint);

    int x2 = point.x();
    int y2 = point.y();

    mCurStroke->addLine(x1, y1, x2, y2);
    drawLine(mImage, x1, y1, x2, y2);

    mModified = true;

    // Now let's find the updated area
    int rad = mBrushWidth / 2 + 2;
    QRect targetRect = QRect(x1, y1, x2, y2).normalized() \
                       .adjusted(-rad, -rad, +rad, +rad);

    targetRect.setWidth((int)qMax(targetRect.width() * mZoomFactor,
                                  mImage->width() * mZoomFactor));
    targetRect.setHeight((int)qMax(targetRect.height() * mZoomFactor,
                                   mImage->height() * mZoomFactor));

    targetRect.moveCenter(rect().center());

    if (targetRect.width() > width())
        targetRect.setLeft(0);

    if (targetRect.height() > height())
        targetRect.setTop(0);

    update(targetRect);
    mLastPoint = endPoint;
}

void QuadroArea::pickColorAt(const QPoint &point)
{
    setBrushColor(QColor(mImage->pixel(convertPos(point))));

    mLastPoint = point;
    update();
}

void QuadroArea::drawPicker(QPainter& painter, const QPoint &point)
{
    painter.setRenderHint(QPainter::Antialiasing, true);
    QColor black(180, 180, 180, 252), grey(206, 206, 206, 210);

    painter.setPen(QPen(black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawEllipse(point, PICKLE_WIDTH, PICKLE_WIDTH);

    painter.setPen(QPen(grey, 6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawEllipse(point, PICKLE_WIDTH - 3, PICKLE_WIDTH - 3);

    painter.setPen(QPen(black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawEllipse(point, PICKLE_WIDTH - 6, PICKLE_WIDTH - 6);

    painter.setPen(QPen(mColor, 10, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawEllipse(point, PICKLE_WIDTH - 11, PICKLE_WIDTH - 11);

    painter.setPen(QPen(black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawEllipse(point, PICKLE_WIDTH - 16, PICKLE_WIDTH - 16);

    // This is the time of the cross
    painter.setPen(QPen(black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawLine(
        point.x(), point.y() - CENTER_DISTANCE,
        point.x(), point.y() - CENTER_DISTANCE - CENTER_OFFSET);
    painter.drawLine(
        point.x(), point.y() + CENTER_DISTANCE,
        point.x(), point.y() + CENTER_DISTANCE + CENTER_OFFSET);
    painter.drawLine(
        point.x() - CENTER_DISTANCE, point.y(),
        point.x() - CENTER_DISTANCE - CENTER_OFFSET, point.y());
    painter.drawLine(
        point.x() + CENTER_DISTANCE, point.y(),
        point.x() + CENTER_DISTANCE + CENTER_OFFSET, point.y());
}

QPoint QuadroArea::convertPos(const QPoint &point)
{
    QRect imRect = QRect(0, 0,
                         (int)(mImage->width() * mZoomFactor),
                         (int)(mImage->height() * mZoomFactor));

    imRect.moveCenter(rect().center());

    if (imRect.width() > width())
        imRect.setLeft(0);

    if (imRect.height() > height())
        imRect.setTop(0);

    // Let's transform this fucking point ok?
    int x, y;

    x = qMax(point.x() - imRect.x(), 0);
    y = qMax(point.y() - imRect.y(), 0);

    x = (int)((float)x / mZoomFactor);
    y = (int)((float)y / mZoomFactor);

    return QPoint(x, y);
}
void QuadroArea::loadTools()
{
    for (int i = 1; i < 4; i++)
        mPatterns.append(QImage(QString(":/brushes/brush%1.png").arg(i)));
}

void QuadroArea::setTool(ToolEnum tool, bool reg)
{
    if (tool != mTool && reg)
        mPrevTool = mTool;

    mTool = tool;

    if (tool < PICKER)
    {
        mBrush = mPatterns.at((int)tool);
        setBrushWidth(mBrushWidth);
    }
    else
        updateSlider();
}

void QuadroArea::setTool(ToolEnum tool, int width, const QColor &color)
{
    mBrushWidth = width;
    mColor = color;
    setTool(tool);
}

void QuadroArea::unsetTool()
{
    setTool(mPrevTool, false);
    updateSlider();
}

void QuadroArea::setBrushWidth(int twidth)
{
    mBrushWidth = twidth;

    if (twidth != mBrush.width())
        mBrush = mPatterns.at(mTool).scaled(QSize(twidth, twidth),
                                            Qt::KeepAspectRatio,
                                            Qt::SmoothTransformation);
}

void QuadroArea::setZoomFactor(float f)
{
    int w, h;

    if (f == mZoomFactor)
        return;

    mZoomFactor = f;

    w = (int)(mImage->width() * mZoomFactor);
    h = (int)(mImage->height() * mZoomFactor);

    setMinimumSize(w, h);

    QScrollArea *p = dynamic_cast<QScrollArea *> (parent());

    if (p)
    {
        QScrollBar *bar = p->horizontalScrollBar();
        bar->setValue(int(mZoomFactor * bar->value() + \
                      ((mZoomFactor - 1) * bar->pageStep() / 2)));

        bar = p->verticalScrollBar();
        bar->setValue(int(mZoomFactor * bar->value() + \
                      ((mZoomFactor - 1) * bar->pageStep() / 2)));

        resize(p->width(), p->height());
    }

    updateSlider();
    repaint();
}
