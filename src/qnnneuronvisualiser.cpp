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

#include "qnnneuronvisualiser.h"
#include "ui_qnnneuronvisualiser.h"
#include "additionalsimulationfunctions.hpp"
#include "output.h"

#include <math.h>
#include <QStringList>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QBuffer>
#include <QTime>

// NN
#include <network/abstractneuralnetwork.h>
#include <network/feedforwardnetwork.h>
#include <network/continuoustimerecurrenneuralnetwork.h>
#include <network/gasnet.h>
#include <network/modulatedspikingneuronsnetwork.h>

// Genes
#include <network/genericgene.h>
#include <network/lengthchanginggene.h>

// SIM
#include <simulation/genericsimulation.h>
#include <simulation/tmazesimulation.h>
#include <simulation/rebergrammarsimulation.h>


QnnNeuronVisualiser::QnnNeuronVisualiser(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QnnNeuronVisualiser),
    _nn_model(NULL),
    _sim_model(NULL)
{
    qsrand(QTime::currentTime().msecsTo(QTime(0,0,0)));
    ui->setupUi(this);

    _nn_model = new QStringListModel(this);
    ui->nnView->setModel(_nn_model);
    _sim_model = new QStringListModel(this);
    ui->simView->setModel(_sim_model);

    QStringList nn;
    nn << "ContinuousTimeRecurrenNeuralNetwork";
    nn << "ContinuousTimeRecurrenNeuralNetwork (tanh)";
    nn << "ContinuousTimeRecurrenNeuralNetwork (size changing)";
    nn << "ContinuousTimeRecurrenNeuralNetwork (size changing, tanh)";
    nn << "GasNet";
    nn << "ModulatedSpikingNeuronsNetwork (a)";
    nn << "ModulatedSpikingNeuronsNetwork (b)";
    nn << "ModulatedSpikingNeuronsNetwork (c)";
    nn << "ModulatedSpikingNeuronsNetwork (d)";
    nn << "ModulatedSpikingNeuronsNetwork (full)";
    nn << "ModulatedSpikingNeuronsNetwork (none)";

    _nn_model->setStringList(nn);

    QStringList sim;
    sim << "GenericSimulation";
    sim << "TMazeSimulation";
    sim << "TMazeSimulation (huge)";
    sim << "ReberGrammarSimulation (DetectGrammar)";
    sim << "ReberGrammarSimulation (CreateWords)";
    sim << "ReberGrammarSimulation (embedded, DetectGrammar)";
    sim << "ReberGrammarSimulation (embedded, CreateWords)";

    _sim_model->setStringList(sim);
}

QnnNeuronVisualiser::~QnnNeuronVisualiser()
{
    delete ui;
}

void QnnNeuronVisualiser::on_pushButton_clicked()
{
    AbstractNeuralNetwork *network = NULL;
    GenericSimulation *simulation = NULL;
    GenericGene *gene;

    QByteArray neuronData;
    QByteArray gasData;
    QBuffer neuronBuffer(&neuronData);
    QBuffer gasBuffer(&gasData);

    QMessageBox window;

    QString selection;

    if(ui->simView->currentIndex().data().toString() == ""
            || ui->nnView->currentIndex().data().toString() == "")
    {
        QMessageBox::information(this,
                                 tr("Invalid selection"),
                                 tr("Please select an item from every category"));
        return;
    }

    if(ui->lineEdit_gene->text() == "")
    {
        QMessageBox::information(this,
                                 tr("Invalid selection"),
                                 tr("Please select a gene file"));
        return;
    }

    QFile gene_file(ui->lineEdit_gene->text());

    // Load gene
    if(GenericGene::canLoadThisGene(&gene_file))
    {
        gene = GenericGene::loadThisGene(&gene_file);
    }
    else if(LengthChangingGene::canLoadThisGene(&gene_file))
    {
        gene = LengthChangingGene::loadThisGene(&gene_file);
    }
    else
    {
        QMessageBox::information(this,
                                 tr("Broken gene file"),
                                 tr("Can not load any gene from file."));
        return;
    }

    // parse SIM
    selection = ui->simView->currentIndex().data().toString();
    if(selection == "GenericSimulation")
    {
        simulation = new GenericSimulation();
    }
    else if(selection == "TMazeSimulation")
    {
        TMazeSimulation::config config;
        config.trials = 1;
        simulation = new TMazeSimulation(config);
    }
    else if(selection == "TMazeSimulation (huge)")
    {
        TMazeSimulation::config config;
        config.max_timesteps = 500;
        config.generateTMaze = &AdditionalSimulationFunctions::generateHugeMaze;
        config.trials = 1;
        simulation = new TMazeSimulation(config);
    }
    else if(selection == "ReberGrammarSimulation (DetectGrammar)")
    {
        ReberGrammarSimulation::config config;
        config.trials_create = 1;
        config.trials_detect = 1;
        simulation = new ReberGrammarSimulation(config);
    }
    else if(selection == "ReberGrammarSimulation (CreateWords)")
    {
        ReberGrammarSimulation::config config;
        config.trials_create = 1;
        config.trials_detect = 1;
        config.mode = ReberGrammarSimulation::CreateWords;
        simulation = new ReberGrammarSimulation(config);
    }
    else if(selection == "ReberGrammarSimulation (embedded, DetectGrammar)")
    {
        ReberGrammarSimulation::config config;
        config.trials_create = 1;
        config.trials_detect = 1;
        config.embedded = true;
        simulation = new ReberGrammarSimulation(config);
    }
    else if(selection == "ReberGrammarSimulation (embedded, CreateWords)")
    {
        ReberGrammarSimulation::config config;
        config.trials_create = 1;
        config.trials_detect = 1;
        config.mode = ReberGrammarSimulation::CreateWords;
        config.embedded = true;
        simulation = new ReberGrammarSimulation(config);
    }
    else
    {
        showUnknownSelectionWindow(selection);
        goto on_pushButton_clicked_cleanup;
    }

    // parse NN
    selection = ui->nnView->currentIndex().data().toString();
    if(selection == "ContinuousTimeRecurrenNeuralNetwork")
    {
        ContinuousTimeRecurrenNeuralNetwork::config config;
        config.neuron_save = &neuronBuffer;
        network = new ContinuousTimeRecurrenNeuralNetwork(simulation->needInputLength(), simulation->needOutputLength(), config);
    }
    else if(selection == "ContinuousTimeRecurrenNeuralNetwork (tanh)")
    {
        ContinuousTimeRecurrenNeuralNetwork::config config;
        config.activision_function = &tanh;
        config.neuron_save = &neuronBuffer;
        network = new ContinuousTimeRecurrenNeuralNetwork(simulation->needInputLength(), simulation->needOutputLength(), config);
    }
    else if(selection == "ContinuousTimeRecurrenNeuralNetwork (size changing)")
    {
        ContinuousTimeRecurrenNeuralNetwork::config config;
        config.size_changing = true;
        config.network_default_size_grow = 1;
        config.neuron_save = &neuronBuffer;
        network = new ContinuousTimeRecurrenNeuralNetwork(simulation->needInputLength(), simulation->needOutputLength(), config);
    }
    else if(selection == "ContinuousTimeRecurrenNeuralNetwork (size changing, tanh)")
    {
        ContinuousTimeRecurrenNeuralNetwork::config config;
        config.size_changing = true;
        config.network_default_size_grow = 1;
        config.activision_function = &tanh;
        config.neuron_save = &neuronBuffer;
        network = new ContinuousTimeRecurrenNeuralNetwork(simulation->needInputLength(), simulation->needOutputLength(), config);
    }
    else if(selection == "GasNet")
    {
        GasNet::config config;
        config.neuron_save = &neuronBuffer;
        config.gas_save = &gasBuffer;
        network = new GasNet(simulation->needInputLength(), simulation->needOutputLength(), config);
    }
    else if(selection == "ModulatedSpikingNeuronsNetwork (a)")
    {
        ModulatedSpikingNeuronsNetwork::config config;
        config.a_modulated = true;
        config.b_modulated = false;
        config.c_modulated = false;
        config.d_modulated = false;
        config.neuron_save = &neuronBuffer;
        config.gas_save = &gasBuffer;
        network = new ModulatedSpikingNeuronsNetwork(simulation->needInputLength(), simulation->needOutputLength(), config);
    }
    else if(selection == "ModulatedSpikingNeuronsNetwork (b)")
    {
        ModulatedSpikingNeuronsNetwork::config config;
        config.a_modulated = false;
        config.b_modulated = true;
        config.c_modulated = false;
        config.d_modulated = false;
        config.neuron_save = &neuronBuffer;
        config.gas_save = &gasBuffer;
        network = new ModulatedSpikingNeuronsNetwork(simulation->needInputLength(), simulation->needOutputLength(), config);
    }
    else if(selection == "ModulatedSpikingNeuronsNetwork (c)")
    {
        ModulatedSpikingNeuronsNetwork::config config;
        config.a_modulated = false;
        config.b_modulated = false;
        config.c_modulated = true;
        config.d_modulated = false;
        config.neuron_save = &neuronBuffer;
        config.gas_save = &gasBuffer;
        network = new ModulatedSpikingNeuronsNetwork(simulation->needInputLength(), simulation->needOutputLength(), config);
    }
    else if(selection == "ModulatedSpikingNeuronsNetwork (d)")
    {
        ModulatedSpikingNeuronsNetwork::config config;
        config.a_modulated = false;
        config.b_modulated = false;
        config.c_modulated = false;
        config.d_modulated = true;
        config.neuron_save = &neuronBuffer;
        config.gas_save = &gasBuffer;
        network = new ModulatedSpikingNeuronsNetwork(simulation->needInputLength(), simulation->needOutputLength(), config);
    }
    else if(selection == "ModulatedSpikingNeuronsNetwork (full)")
    {
        ModulatedSpikingNeuronsNetwork::config config;
        config.a_modulated = true;
        config.b_modulated = true;
        config.c_modulated = true;
        config.d_modulated = true;
        config.neuron_save = &neuronBuffer;
        config.gas_save = &gasBuffer;
        network = new ModulatedSpikingNeuronsNetwork(simulation->needInputLength(), simulation->needOutputLength(), config);
    }
    else if(selection == "ModulatedSpikingNeuronsNetwork (none)")
    {
        ModulatedSpikingNeuronsNetwork::config config;
        config.a_modulated = false;
        config.b_modulated = false;
        config.c_modulated = false;
        config.d_modulated = false;
        config.neuron_save = &neuronBuffer;
        config.gas_save = &gasBuffer;
        network = new ModulatedSpikingNeuronsNetwork(simulation->needInputLength(), simulation->needOutputLength(), config);
    }
    else
    {
        showUnknownSelectionWindow(selection);
        goto on_pushButton_clicked_cleanup;
    }

    simulation->initialise(network, gene);

    window.setWindowTitle(tr("Running simulation"));
    window.setText(tr("The current simulation is running"));
    window.setWindowFlags(((windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowCloseButtonHint));
    window.show();
    simulation->getScore();
    window.close();

    if(neuronData != QByteArray())
    {
        Output *output = new Output(neuronData);
        output->setWindowTitle("Neuron output");
        QObject::connect(output, SIGNAL(finished(int)), output, SLOT(deleteLater()));
        output->show();
    }

    if(gasData != QByteArray())
    {
        Output *output = new Output(gasData);
        output->setWindowTitle("gas output");
        QObject::connect(output, SIGNAL(finished(int)), output, SLOT(deleteLater()));
        output->show();
    }

    // cleanup
on_pushButton_clicked_cleanup:

    delete simulation;
    delete network;
    delete gene;
}

void QnnNeuronVisualiser::on_actionAbout_triggered()
{
    QMessageBox::information(this,
                             tr("About QnnNeuronVisualiser"),
                             tr("QnnNeuronVisualiser is a simple graphical interface to show the neuron activities in networks\nAuthor: Marcus Soll\nLicense: GPL3+\nThis program uses qnn, which is licensed under the LGPL3+"));
}

void QnnNeuronVisualiser::on_actionAbout_Qt_triggered()
{
    QApplication::aboutQt();
}

void QnnNeuronVisualiser::on_actionQuit_triggered()
{
    QApplication::quit();
}

void QnnNeuronVisualiser::showUnknownSelectionWindow(QString s)
{
    QMessageBox::warning(this,
                         tr("Unknown selection"),
                         QString(tr("Unknown selection: &1")).arg(s));
}

void QnnNeuronVisualiser::on_toolButton_gene_clicked()
{
    QFileDialog dialog(this, tr("Open gene file"), "", "gene file (*.gene)");
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    if(dialog.exec() == QFileDialog::Accepted && dialog.selectedFiles()[0].length() > 0)
    {
        ui->lineEdit_gene->setText(dialog.selectedFiles()[0]);
    }
}

void QnnNeuronVisualiser::on_actionAbout_QCustomPlot_triggered()
{
    QMessageBox::information(this,
                             tr("About QCustomPlot"),
                             tr("This program uses QCustomPlot, which is available under the GPL3+\nFor more information see http://qcustomplot.com/"));
}
