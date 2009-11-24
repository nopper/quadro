#include "colordialog.h"
#include "ui_colordialog.h"

ColorDialog::ColorDialog(const QColor &color, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ColorDialog)
{
    m_ui->setupUi(this);
    mPicker = new ColorPicker(this);
    mPicker->setColor(color);

    m_ui->mLayout->addWidget(mPicker);
}

ColorDialog::~ColorDialog()
{
    delete m_ui;
}

void ColorDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
