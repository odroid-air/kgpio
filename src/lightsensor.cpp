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
    //qDebug() << "New LightSensor" << id;
    connect(this, &GpioPin::initialized, this, [this] {
        qDebug() << " LightSensor initialized: " << number();
        d->elapsedSensorTimer = new QElapsedTimer();
        d->elapsedSensorTimer->start();
        d->valueMonitor = new QTimer(this);
        d->valueMonitor->setInterval(5);
        connect(d->valueMonitor, &QTimer::timeout, this, [this] {
                d->updateValue();
            }
        );
        setDirection(GpioPin::Out);
        setDirection(GpioPin::In);

        d->elapsedSensorTimer->start();
        d->valueMonitor->start();
    });

    d->sampleTimer = new QTimer(this);
    d->sampleTimer->setInterval(333);
    connect(d->sampleTimer, &QTimer::timeout, this, [this] {
        d->sampleTimer->stop();
        setDirection(GpioPin::In);
        d->elapsedSensorTimer->start();
        d->valueMonitor->start();
    });
}

LightSensor::~LightSensor()
{
    delete d->elapsedSensorTimer;
    delete d;
}

void LightSensorPrivate::updateValue()
{
    const auto val = q->value();
    if (val == GpioPin::High && elapsedSensorTimer->elapsed() > valueMonitor->interval()) {
        auto t = elapsedSensorTimer->elapsed();
        qreal new_brightness = 0;
        if (t > 0) {
            new_brightness = 1.0 / t;
            //qDebug() << "Got an updated value" << t << brightness;
        } else {
            new_brightness = 1;
//             qDebug() << "Got an updated value 11111 .. Very bright??";
        }
        valueMonitor->stop();
        q->setDirection(GpioPin::Out);
        // if we change less than 1%, don't bother until a higher value comes in
        if (brightness * 0.99 < new_brightness && brightness * 1.01 > new_brightness) {
            //qDebug() << "We're actually too close." << (brightness / new_brightness);
        } else {
            brightness = new_brightness;
            //qDebug() << "Got an updated value" << t << brightness;
            emit q->valueChanged();
            emit q->brightnessChanged();
        }
        sampleTimer->start();
    }
}

qreal LightSensor::brightness() const
{
    return d->brightness;
}


} // ns
