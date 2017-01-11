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


#include "lightsensor.h"
#include "lightsensor_p.h"

#include <QDir>
#include <QDebug>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QTimer>


namespace KGpio {

LightSensor::LightSensor(int id, QObject *parent)
    : GpioPin(id, parent)
    , d(new LightSensorPrivate(this))
{
    qDebug() << "New LightSensor" << id;
    connect(this, &GpioPin::initialized, this, [this] {
        qDebug() << " INIT LightSensor" << number();
        d->elapsedSensorTimer = new QElapsedTimer();
        d->elapsedSensorTimer->start();
        d->valueMonitor = new QTimer(this);
        d->valueMonitor->setInterval(10);
        connect(d->valueMonitor, &QTimer::timeout, this, [this] {
                d->updateValue();
            }
        );
        //d->valueMonitor->start();
        setDirection(GpioPin::Out);
        //setValue(GpioPin::Low);
//         setValue(GpioPin::Low);
        setDirection(GpioPin::In);
        //d->sampleTimer->start();
        //d->updateValue();

        d->elapsedSensorTimer->start();
        d->valueMonitor->start();
    });

    return;

    d->sampleTimer = new QTimer(this);
    d->sampleTimer->setInterval(1500);
    connect(d->sampleTimer, &QTimer::timeout, this, [this] {
//         qDebug() << "Sample fired";
        setDirection(GpioPin::Out);
        //setValue(GpioPin::Low);
//         qDebug() << "VAL1:" << value();
//         Q_ASSERT(value() == GpioPin::Low);
        setDirection(GpioPin::In);
//         qDebug() << "VAL2:" << value();
        if (value() == GpioPin::Low) {
            d->elapsedSensorTimer->start();
            d->valueMonitor->start();
        }
    });
}

LightSensor::~LightSensor()
{
    delete d->elapsedSensorTimer;
    delete d;
    //GpioPin::~GpioPin();
}

void LightSensorPrivate::updateValue()
{
//     qDebug() << "up...";
    //<< t << brightness;
    const auto val = q->value();
    if (val == GpioPin::High && elapsedSensorTimer->elapsed() > valueMonitor->interval()) {
        auto t = elapsedSensorTimer->elapsed() - (valueMonitor->interval() / 2);
        if (t > 0) {
            brightness = 1.0 / t;
            qDebug() << "Got an updated value" << t << brightness;
        } else {
            brightness = 1;
            qDebug() << "Got an updated value 11111 .. Very bright??";
        }
        //valueMonitor->stop();
        emit q->valueChanged();
        emit q->brightnessChanged();
        q->setDirection(GpioPin::Out);
        //qDebug() << "value low again?" << q->value();
        //q->setValue(GpioPin::Low);
        q->setDirection(GpioPin::In);
        elapsedSensorTimer->start();
        //sampleTimer->start();
    }
}

qreal LightSensor::brightness() const
{
    return d->brightness;
}


} // ns