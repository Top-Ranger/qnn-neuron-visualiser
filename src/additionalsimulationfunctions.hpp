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

#ifndef ADDITIONALSIMULATIONFUNCTIONS_HPP
#define ADDITIONALSIMULATIONFUNCTIONS_HPP

#include <randomhelper.h>

#include <QVector>

namespace AdditionalSimulationFunctions {
static QVector<qint32> generateHugeMaze()
{
    QVector<qint32> list;
    list.reserve(152);
    int number = RandomHelper::getRandomInt(1, 5);
    list << number;
    for(int i = 0; i < 150; ++i)
    {
        list << 0;
    }
    if(RandomHelper::getRandomBool())
    {
        list << number;
    }
    else
    {
        list << RandomHelper::getRandomInt(1, 5);
    }
    return list;
}
}

#endif // ADDITIONALSIMULATIONFUNCTIONS_HPP
