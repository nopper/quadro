#include "quadro.h"
#include "qlayersdialog.h"
#include "colorpicker.h"
#include "qtooldialog.h"
#include "colordialog.h"

Quadro::Quadro(QWidget *parent)
    : QMainWindow(parent)
{
    mQuadroArea = new QuadroArea(QSize(640, 480), this);
    mQuadroArea->update();

    mScrollArea = new QScrollArea(this);
    mScrollArea->setWidgetResizable(true);
    mScrollArea->setBackgroundRole(QPalette::Dark);
    mScrollArea->setWidget(mQuadroArea);

    connect(mScrollArea->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(scrolled(int)));
    connect(mScrollArea->horizontalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(scrolled(int)));

    setCentralWidget(mScrollArea);
    setWindowState(Qt::WindowMaximized);

    QToolBar *toolBar = new QToolBar(this);
    toolBar->setIconSize(QSize(16, 16));
    addToolBar(Qt::BottomToolBarArea, toolBar);

    QAction *act = toolBar->addAction(QIcon(":/icons/open.png"), tr("Open"));
    connect(act, SIGNAL(triggered()), this, SLOT(open()));

    act = toolBar->addAction(QIcon(":/icons/save.png"), tr("Save"));
    connect(act, SIGNAL(triggered()), this, SLOT(save()));

    act = toolBar->addAction(QIcon(":/icons/undo.png"), tr("Undo"));
    connect(act, SIGNAL(triggered()), mQuadroArea, SLOT(undo()));

    act = toolBar->addAction(QIcon(":/icons/redo.png"), tr("Redo"));
    connect(act, SIGNAL(triggered()), mQuadroArea, SLOT(redo()));

    act = toolBar->addAction(QIcon(":/icons/zoomin.png"), tr("Zoom"));
    connect(act, SIGNAL(triggered()), this, SLOT(setZoom()));

    act = toolBar->addAction(QIcon(":/icons/layers.png"), tr("Layers"));
    connect(act, SIGNAL(triggered()), this, SLOT(manageLayers()));

    act = toolBar->addAction(QIcon(":/icons/tools.png"), tr("Tools"));
    connect(act, SIGNAL(triggered()), this, SLOT(selectTool()));

    act = toolBar->addAction(QIcon(":/icons/colors.png"), tr("Color"));
    connect(act, SIGNAL(triggered()), this, SLOT(selectColor()));

    act = toolBar->addAction(QIcon(":/icons/about.png"), tr("About"));
    connect(act, SIGNAL(triggered()), this, SLOT(about()));
}

void Quadro::open()
{
    if (mQuadroArea->isModified())
        save();

    QFileDialog dialog(this, tr("Select a file"), QString(),
                       tr("Image Files (*.png *.jpg *.bmp)"));

    if (dialog.exec())
        mQuadroArea->open(dialog.selectedFiles().at(0));
}

void Quadro::save()
{
    QMessageBox box(QMessageBox::Question, tr("Save image"),
                    tr("Do you want to save image?"),
                    QMessageBox::Save | QMessageBox::Cancel, this);

    if (box.exec() ==  QMessageBox::Save)
    {
        box.close();

        QStringList fileList;
        QFileDialog dialog(this, tr("Save image to"), QString(),
                           tr("Image Files (*.png *.jpg *.bmp)"));
        dialog.setAcceptMode(QFileDialog::AcceptSave);

        if (dialog.exec())
            mQuadroArea->saveTo(dialog.selectedFiles().at(0));
    }
}

void Quadro::scrolled(int value)
{
    Q_UNUSED(value);
    mQuadroArea->update();
    mQuadroArea->updateSlider();
}

void Quadro::setZoom()
{
    if (mQuadroArea->tool() == ZOOMER)
    {
        mQuadroArea->setZoomFactor(1.0);
        mQuadroArea->unsetTool();
    }
    else
    {
        mQuadroArea->setTool(ZOOMER);
        mQuadroArea->setZoomFactor(1.2);
    }
}

void Quadro::manageLayers()
{
    QLayersDialog dialog(mQuadroArea, this);
    dialog.setWindowState(Qt::WindowMaximized);

    if (dialog.exec() == QDialog::Accepted)
        mQuadroArea->update();
}

void Quadro::selectTool()
{
    QToolDialog *dialog = new QToolDialog(mQuadroArea, this);

    if (dialog->exec() == QDialog::Accepted)
        mQuadroArea->setTool(dialog->toolType(), dialog->toolSize(),
                             dialog->selectedColor());

    delete dialog;
}

void Quadro::selectColor()
{
    ColorDialog dialog(mQuadroArea->brushColor(), this);

    if (dialog.exec())
        mQuadroArea->setBrushColor(dialog.selectedColor());
}

void Quadro::about()
{
    QMessageBox box(QMessageBox::Information, tr("About Quadro"),
        tr("<p><b>Quadro</b></p>"
           "<p>(C) 2009 - Francesco Piccinno (stack.box@gmail.com)</p>"
           "<p>A simple paint program for Nokia s60v5 inpired "
           "by Brushes app.<p>"
           "<p>Take a look to "
           "http://github.com/nopper/quadro for updates</p>"),
        QMessageBox::Ok, this);
    box.exec();
}
