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

#ifndef QnnNeuronVisualiser_H
#define QnnNeuronVisualiser_H

#include <QMainWindow>
#include <QString>
#include <QStringListModel>

namespace Ui
{
class QnnNeuronVisualiser;
}

class QnnNeuronVisualiser : public QMainWindow
{
    Q_OBJECT

public:
    explicit QnnNeuronVisualiser(QWidget *parent = 0);
    ~QnnNeuronVisualiser();

private slots:
    void on_pushButton_clicked();
    void on_actionAbout_triggered();
    void on_actionAbout_Qt_triggered();
    void on_actionQuit_triggered();
    void on_toolButton_gene_clicked();
    void on_actionAbout_QCustomPlot_triggered();

private:
    void showUnknownSelectionWindow(QString s);
    Ui::QnnNeuronVisualiser *ui;

    QStringListModel *_nn_model;
    QStringListModel *_sim_model;
};

#endif // QnnNeuronVisualiser_H
