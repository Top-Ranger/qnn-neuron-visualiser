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


#ifndef OUTPUT_H
#define OUTPUT_H

#include <QDialog>
#include <QByteArray>

namespace Ui {
class Output;
}

class Output : public QDialog
{
    Q_OBJECT

public:
    explicit Output(QByteArray data, QWidget *parent = 0);
    ~Output();

private slots:
    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_pushButton_save_clicked();

private:
    QByteArray _data;
    Ui::Output *ui;
};

#endif // OUTPUT_H
