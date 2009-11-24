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

#ifndef QTOOLDIALOG_H
#define QTOOLDIALOG_H

#include <QtGui/QDialog>
#include <QtCore/QSignalMapper>
#include "ui_qtooldialog.h"
#include "quadroarea.h"

class QToolDialog : public QDialog
{
    Q_OBJECT

public:
    QToolDialog(QuadroArea *ref, QWidget *parent = 0);
    ~QToolDialog();

    ToolEnum toolType() { return m_toolType; }
    int toolSize() { return m_toolSize; }

    const QColor selectedColor();

private slots:
    void toolSizeChanged(int);
    void toolAlphaChanged(int);
    void toolSelect();
private:
    void redraw();
    Ui::QToolDialogClass ui;
    QuadroArea *mQuadroArea;

    ToolEnum m_toolType;
    int m_toolAlpha;
    int m_toolSize;
};

#endif // QTOOLDIALOG_H
