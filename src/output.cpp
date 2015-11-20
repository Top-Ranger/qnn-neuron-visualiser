/*
 * Copyright (C) 2015 Marcus Soll
 * This file is part of qnn-neuron-visualiser.
 *
 * qnn-neuron-visualiser is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * qnn-neuron-visualiser is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with qnn-neuron-visualiser. If not, see <http://www.gnu.org/licenses/>.
 */


#include "output.h"
#include "ui_output.h"

#include <QBuffer>
#include <QTextStream>
#include <QDebug>
#include <QFileDialog>

Output::Output(QByteArray data, QWidget *parent) :
    QDialog(parent),
    _data(data),
    ui(new Ui::Output)
{
    ui->setupUi(this);
    QBuffer buffer(&_data);
    if(!buffer.open(QIODevice::ReadOnly))
    {
        qCritical() << "Can not open stream:" << buffer.errorString();
    }

    QTextStream stream(&buffer);
    QString header = stream.readLine();
    ui->comboBox->addItems(header.split(";", QString::SkipEmptyParts));
    buffer.close();
}

Output::~Output()
{
    delete ui;
}


void Output::on_comboBox_currentIndexChanged(const QString &arg1)
{
    ui->plot->clearGraphs();
    ui->plot->addGraph();
    ui->plot->graph(0)->setPen(QPen(Qt::red));

    QBuffer buffer(&_data);
    if(!buffer.open(QIODevice::ReadOnly))
    {
        qCritical() << "Can not open stream:" << buffer.errorString();
    }
    QTextStream stream(&buffer);
    QString s = stream.readLine();
    QStringList sl = s.split(";", QString::SkipEmptyParts);
    int column = -1;
    for(int i = 0; i < sl.size(); ++i)
    {
        if(sl[i] == arg1)
        {
            column = i;
            break;
        }
    }

    if(column == -1)
    {
        qCritical() << "Can not find column:" << arg1;
        return;
    }

    int row = 0;
    while(!stream.atEnd())
    {
        s = stream.readLine();
        sl = s.split(";", QString::SkipEmptyParts);

        if(sl.size() <= column)
        {
            continue;
        }

        ui->plot->graph(0)->addData(row, sl[column].toDouble());

        ++row;
    }

    ui->plot->rescaleAxes();
    ui->plot->replot();
    buffer.close();
}

void Output::on_pushButton_save_clicked()
{
        QFileDialog dialog(this, tr("Save Plot"), "", "png-image (*.png)");
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        dialog.setDefaultSuffix("png");
        if(dialog.exec() == QFileDialog::Accepted && dialog.selectedFiles()[0].length() > 0)
        {
            ui->plot->savePng(dialog.selectedFiles()[0]);
        }
}
