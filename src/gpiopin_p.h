/********************************************************************
Copyright 2017 Sebastian Kügler <sebas@kde.org>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) version 3, or any
later version accepted by the membership of KDE e.V. (or its
successor approved by the membership of KDE e.V.), which shall
act as a proxy defined in Section 6 of version 3 of the license.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/


#ifndef GPIOPIN_P_H
#define GPIOPIN_P_H

#include "kgpio.h"

class QTimer;
class QElapsedTimer;

namespace KGpio {

class GpioPinPrivate
{
    Q_DECLARE_TR_FUNCTIONS(GpioPinPrivate)

public:
    GpioPinPrivate(GpioPin *_q)
        : q(_q)
        , error(GpioPin::Uninitialized)
        , pinNumber(0)
    {
    }
    ~GpioPinPrivate()
    {
    }
    GpioPin* q;

    void checkErrors();
    void checkInitialized();
    bool nodeExists() const;
    QString nodePath() const;
    bool exportPin();
    bool unexportPin();

    bool writeToFile(const QString &filename, const QString &content);

    GpioPin::PinError error;
    int pinNumber = 0;
    bool manageNode = false;

    QElapsedTimer* elapsedTimer = nullptr;
    QTimer* initializationTimer = nullptr;
};

}
#endif // KGPIO_P_H
