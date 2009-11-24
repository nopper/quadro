#include "qtooldialog.h"

QToolDialog::QToolDialog(QuadroArea *ref, QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    mQuadroArea = new QuadroArea(QSize(300, 300), this);
    ui.mLayout->addWidget(mQuadroArea);

    mQuadroArea->setBrushColor(ref->brushColor());
    mQuadroArea->setBrushWidth(ref->brushWidth());

    m_toolSize = mQuadroArea->brushWidth();
    m_toolAlpha = mQuadroArea->brushColor().alpha();
    m_toolType = BRUSH1;

    QButtonGroup *group = new QButtonGroup(this);
    group->addButton(ui.eraserBtn);
    group->addButton(ui.brush1Btn);
    group->addButton(ui.brush2Btn);

    ui.mSizeSlider->setValue(m_toolSize);
    ui.mAlphaSlider->setValue(m_toolAlpha);

    redraw();
}

QToolDialog::~QToolDialog()
{
}

const QColor QToolDialog::selectedColor()
{
    QColor color = mQuadroArea->brushColor();
    color.setAlpha(m_toolAlpha);
    return color;
}

void QToolDialog::toolAlphaChanged(int value)
{
    QColor color = mQuadroArea->brushColor();
    color.setAlpha(value);

    mQuadroArea->setBrushColor(color);
    redraw();
}

void QToolDialog::toolSizeChanged(int value)
{
    m_toolSize = value;
    mQuadroArea->setBrushWidth(value);
    redraw();
}

void QToolDialog::redraw()
{
    // Let's draw a shape
    mQuadroArea->clearImage();

    QPainterPath path;

    path.moveTo(50, 50);
    path.cubicTo(QPoint(50, 90), QPoint(75, 82), QPoint(130, 100));

    mQuadroArea->drawPath(path);
}

void QToolDialog::toolSelect()
{
    if (ui.eraserBtn->isChecked())
        m_toolType = ERASER;
    else if (ui.brush1Btn->isChecked())
        m_toolType = BRUSH1;
    else if (ui.brush2Btn->isChecked())
        m_toolType = BRUSH2;

    mQuadroArea->setTool(m_toolType);
    redraw();
}
