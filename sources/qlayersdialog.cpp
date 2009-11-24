#include "qlayersdialog.h"
#include "ui_qlayersdialog.h"

#define LAYER_ROLE Qt::UserRole + 1
#define ASSIGNED_ROLE Qt::UserRole + 2
#define OPACITY_ROLE Qt::UserRole + 3

void Thumb::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QLayersDialog *p = dynamic_cast<QLayersDialog*> (parent());

    QPainter painter(this);
    painter.setPen(QPen(QBrush(QColor(100, 100, 100)), 3, Qt::DashLine));

    if (p && p->currentLayer())
        painter.drawImage(rect(), *p->currentLayer()->image(),
                          p->currentLayer()->image()->rect());
    else
        painter.drawText(rect(), tr("No image"), QTextOption(Qt::AlignCenter));

    painter.drawRect(rect().adjusted(0, 0, -1, -1));
}

QLayersDialog::QLayersDialog(QuadroArea *quadro, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::QLayersDialog)
{
    mQuadroArea = quadro;
    m_ui->setupUi(this);
    mGarbage.clear();

    int index = 0;
    QuadroLayer *layer;
    QStandardItem *item;

    foreach (layer, mQuadroArea->layers())
    {
        item = new QStandardItem(layer->name());
        item->setFlags(Qt::ItemIsEditable | Qt::ItemIsUserCheckable | \
                       Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(QVariant(layer->isActive() ? Qt::Checked : Qt::Unchecked),
                      Qt::CheckStateRole);

        item->setData(QVariant::fromValue((void *)layer), LAYER_ROLE);
        item->setData(QVariant(true), ASSIGNED_ROLE);
        item->setData(QVariant(layer->opacity()), OPACITY_ROLE);

        if (index == mQuadroArea->activeLayer())
        {
            mActiveItem = item;
            QFont font = mActiveItem->font();
            font.setBold(true);
            mActiveItem->setFont(font);
        }

        index++;
        mModel.appendRow(item);
    }

    m_ui->mList->setModel(&mModel);

    connect(m_ui->mList->selectionModel(),
            SIGNAL(currentChanged(QModelIndex, QModelIndex)),
            this, SLOT(selectionChanged(QModelIndex, QModelIndex)));
}

QLayersDialog::~QLayersDialog()
{
    mActiveItem = NULL;
    mQuadroArea = NULL;

    QuadroLayer *layer;

    foreach (layer, mGarbage)
        delete layer;

    delete m_ui;
}

void QLayersDialog::selectionChanged(const QModelIndex &current,
                                     const QModelIndex &previous)
{
    Q_UNUSED(current);
    Q_UNUSED(previous);

    m_ui->mFrame->update();
    int row = m_ui->mList->currentIndex().row();

    if (row < 0)
        return;

    QStandardItem * item = mModel.item(row, 0);
    QuadroLayer *layer = (QuadroLayer *)item->data(LAYER_ROLE).value<void *>();

    m_ui->mOpacitySlider->setValue(layer->opacity());
}

QuadroLayer* QLayersDialog::currentLayer()
{
    int row = m_ui->mList->currentIndex().row();

    if (row < 0)
        return NULL;

    QStandardItem * item = mModel.item(row, 0);
    QuadroLayer *layer = (QuadroLayer *)item->data(LAYER_ROLE).value<void *>();

    if (layer /*&& item->data(ASSIGNED_ROLE).value<bool>()*/)
        return layer;

    return NULL;
}

void QLayersDialog::moveUp()
{
    int row = m_ui->mList->currentIndex().row();

    if (row <= 0)
        return;

    mModel.insertRow(row - 1, mModel.takeItem(row, 0));
    mModel.removeRow(row + 1);
    m_ui->mList->setCurrentIndex(mModel.index(row - 1, 0));
}

void QLayersDialog::moveDown()
{
    int row = m_ui->mList->currentIndex().row();

    if (row < 0 || row == mModel.rowCount() - 1)
        return;

    row++;
    mModel.insertRow(row - 1, mModel.takeItem(row, 0));
    mModel.removeRow(row + 1);
    m_ui->mList->setCurrentIndex(mModel.index(row, 0));
}

void QLayersDialog::setAlpha(int value)
{
    int row = m_ui->mList->currentIndex().row();

    if (row < 0)
        return;

    QStandardItem *item = mModel.item(row, 0);
    item->setData(QVariant(value), OPACITY_ROLE);
}

void QLayersDialog::setActiveLayer()
{
    int row = m_ui->mList->currentIndex().row();

    if (row < 0)
        return;

    QStandardItem *item = mModel.item(row, 0);

    if (mActiveItem == item)
        return;

    QFont font = item->font();

    if (mActiveItem)
        mActiveItem->setFont(font);

    font.setBold(true);
    item->setFont(font);

    mActiveItem = item;
}

void QLayersDialog::newLayer()
{
    int row = m_ui->mList->currentIndex().row();

    if (row < 0)
        return;

    QuadroLayer *layer = new QuadroLayer(tr("New Layer"), 255, true,
                                         mQuadroArea->layers().at(0)->size(),
                                         QColor(255, 255, 255, 0));

    QStandardItem *item = new QStandardItem(tr("New Layer"));
    item->setFlags(Qt::ItemIsEditable | Qt::ItemIsUserCheckable | \
                   Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    item->setData(QVariant(Qt::Checked), Qt::CheckStateRole);
    item->setData(QVariant::fromValue((void *)layer), LAYER_ROLE);
    item->setData(QVariant(100), OPACITY_ROLE);

    mModel.insertRow(row + 1, item);
    m_ui->mList->setCurrentIndex(mModel.index(row + 1, 0));
}

void QLayersDialog::deleteLayer()
{
    int row = m_ui->mList->currentIndex().row();

    if (row < 0 || mModel.rowCount() == 1)
        return;

    QStandardItem *item = mModel.item(row, 0);
    QuadroLayer *layer = (QuadroLayer *)item->data(LAYER_ROLE).value<void *>();

    mModel.removeRow(row);
    mGarbage.append(layer);
}

void QLayersDialog::accept()
{
    int opacity;
    bool assigned;
    QuadroLayer *layer;
    QStandardItem *item;

    mQuadroArea->layers().clear();
    mQuadroArea->undoes().clear();

    for (int i = 0; i < mModel.rowCount(); i++)
    {
        item = mModel.item(i, 0);

        assigned = item->data(ASSIGNED_ROLE).value<bool>();
        layer = (QuadroLayer *)item->data(LAYER_ROLE).value<void *>();
        opacity = item->data(OPACITY_ROLE).value<int>() % 256;

        if (assigned)
            layer->setOpacity(opacity);

        layer->setActive(item->checkState() == Qt::Checked ? true : false);
        layer->setName(item->text());

        mQuadroArea->layers().append(layer);

        if (mActiveItem == item)
            mQuadroArea->setActiveLayer(i);
    }

    QDialog::accept();
}

void QLayersDialog::reject()
{
    QStandardItem *item;
    for (int i = 0; i < mModel.rowCount(); i++)
    {
        item = mModel.item(i, 0);
        if (!item->data(ASSIGNED_ROLE).value<bool>())
        {
            QuadroLayer *layer = \
                (QuadroLayer *)item->data(LAYER_ROLE).value<void *>();
            delete layer;
        }
    }

    QDialog::reject();
}

void QLayersDialog::changeEvent(QEvent *e)
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
