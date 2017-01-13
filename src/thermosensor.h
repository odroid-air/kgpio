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
#ifndef THERMOSENSOR_H
#define THERMOSENSOR_H

#include <QtCore/QCoreApplication>
#include <QtCore/QString>

#include <kgpio_export.h>

#include <gpiopin.h>

namespace KGpio {

class ThermoSensorPrivate;

/**
    This class represents a digital lightsensor, it works as follows:

    - The pin is initialized (exported, waited for permission change from
    udev
    - The pin direction is set to Out, which in turn sets its value to Low
    - The capacitors are now being charged, when they're charge enough,
    value will go from Low to High
    - We monitor the value and record the brightness when it changes to High
    - We now reset the pin by switching direction to Out (sets value to
    Low), and then to In
    - ... and we wait again

    See also:
    * https://www.raspberrypi.org/learning/physical-computing-with-python/ldr/
    * https://learn.adafruit.com/basic-resistor-sensor-reading-on-raspberry-pi/basic-photocell-reading
    * https://pimylifeup.com/raspberry-pi-light-sensor/

    @short generic class for reading/writing gpio pins
    @author Sebastian Kügler <sebas@kde.org>
 */
class KGPIO_EXPORT ThermoSensor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal temperature READ temperature NOTIFY temperatureChanged)
    Q_PROPERTY(QString sensorId READ sensorId WRITE setSensorId NOTIFY sensorIdChanged)
    Q_PROPERTY(QStringList availableSensorIds READ availableSensorIds CONSTANT)

public:

    /**
     * Base constructor.
     */
    ThermoSensor(QObject *parent = nullptr);

    virtual ~ThermoSensor();

    qreal temperature() const;
    void setTemperature(const qreal &temp);

    QString sensorId() const;
    void setSensorId(const QString &id);

    QStringList availableSensorIds() const;

Q_SIGNALS:
    void initialized();
    void sensorIdChanged();
    void temperatureChanged();

private:
    ThermoSensorPrivate *const d;
};

} // namespace


Q_DECLARE_METATYPE(KGpio::ThermoSensor*)

#endif
