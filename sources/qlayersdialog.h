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

#ifndef QLAYERSDIALOG_H
#define QLAYERSDIALOG_H

#include <QtGui/QDialog>
#include "quadroarea.h"

namespace Ui
{
    class QLayersDialog;
}

class Thumb : public QFrame
{
    Q_OBJECT
public:
    Thumb(QWidget *parent = 0) : QFrame(parent) {}
protected:
    void paintEvent(QPaintEvent *event);
};

class QLayersDialog : public QDialog
{
    Q_OBJECT
public:
    QLayersDialog(QuadroArea *quadro, QWidget *parent = 0);
    ~QLayersDialog();

    QuadroLayer* currentLayer();

private slots:
    void moveUp();
    void moveDown();
    void setAlpha(int value);
    void newLayer();
    void deleteLayer();
    void setActiveLayer();
    void selectionChanged(const QModelIndex &current,
                          const QModelIndex &previous);

public slots:
    void accept();
    void reject();

protected:
    void changeEvent(QEvent *e);

private:
    QuadroArea *mQuadroArea;
    QList<QuadroLayer*> mGarbage;

    Ui::QLayersDialog *m_ui;
    QStandardItemModel mModel;
    QStandardItem *mActiveItem;
};

#endif // QLAYERSDIALOG_H
