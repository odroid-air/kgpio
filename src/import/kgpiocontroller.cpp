/************************************************************************
 *                                                                      *
 *   Copyright 2017 Sebastian Kügler <sebas@kde.org>                    *
 *                                                                      *
 *   This library is free software; you can redistribute it and/or      *
 *   modify it under the terms of the GNU Lesser General Public         *
 *   License as published by the Free Software Foundation; either       *
 *   version 2.1 of the License, or (at your option) version 3, or any  *
 *   later version accepted by the membership of KDE e.V. (or its       *
 *   successor approved by the membership of KDE e.V.), which shall     *
 *   act as a proxy defined in Section 6 of version 3 of the license.   *
 *                                                                      *
 *   This library is distributed in the hope that it will be useful,    *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  *
 *   Lesser General Public License for more details.                    *
 *                                                                      *
 *   You should have received a copy of the GNU Lesser General Public   *
 *   License along with this library.  If not, see                      *
 *   <http://www.gnu.org/licenses/>.                                    *
 *                                                                      *
 ************************************************************************/


#include "kgpiocontroller.h"
#include <gpiopin.h>

#include "debug.h"

#include <QSharedPointer>
#include <QTimer>

using namespace KGpio;


class KGpioControllerPrivate {
public:
    KGpioController *q;
    QList<KGpio::GpioPin*> pins;
};

KGpioController::KGpioController(QObject* parent) :
    QObject(parent),
    d(new KGpioControllerPrivate)
{
    d->q = this;

    loadPin(108);
    loadPin(97);
    loadPin(100);
}

KGpioController::~KGpioController()
{
}

void KGpioController::loadPin(const int pinNumber)
{

    auto pin = KGpio::KGpio::self()->pin(pinNumber);
    connect(pin, &GpioPin::initialized, this, [this, pin] {
        qDebug() << "new pin!" << pin->number();
        pin->setDirection(GpioPin::Out);
        d->pins.append(pin);
        Q_EMIT pinsChanged();
    });

}

void KGpioController::pinAdded(KGpio::GpioPin* pin)
{
    d->pins.append(pin);
    Q_EMIT pinsChanged();
}

void KGpioController::pinRemoved(KGpio::GpioPin* pin)
{
    d->pins.removeAll(pin);
    Q_EMIT pinsChanged();
}

QQmlListProperty<KGpio::GpioPin> KGpioController::pins()
{
    QQmlListProperty<KGpio::GpioPin> lst(this, d->pins);
    return lst;
}

//} // ns

