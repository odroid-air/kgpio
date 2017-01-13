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


#include "thermosensor.h"
#include "thermosensor_p.h"

#include <QDir>
#include <QDebug>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QTimer>


namespace KGpio {

ThermoSensor::ThermoSensor(QObject *parent)
    : QObject(parent)
    , d(new ThermoSensorPrivate(this))
{
    QDir devicesDir(d->sysfsDevicesPath);
    d->availableSensorIds = devicesDir.entryList(QDir::NoDot | QDir::NoDotDot | QDir::Dirs);
    qDebug() << "New ThermoSensor" << d->availableSensorIds;
}

ThermoSensor::~ThermoSensor()
{
    delete d;
}

QStringList ThermoSensor::availableSensorIds() const
{
    return d->availableSensorIds;
}

qreal ThermoSensor::temperature() const
{
    return d->temperature;
}

QString ThermoSensor::sensorId() const
{
    return d->sensorId;
}

void ThermoSensor::setSensorId(const QString &id)
{
    bool first = false;
    if (d->sensorId != id) {
        d->sensorId = id;
        if (d->sysfsFile.isEmpty()) {
            first = true;
        }

        d->sysfsFile = QString(QStringLiteral("%1%2/w1_slave")).arg(d->sysfsDevicesPath, id);
        if (!QFileInfo(d->sysfsFile).exists()) {

            qDebug() << "Error, sysfs file doesn't exist!" << d->sysfsFile;
            qDebug() << "Perhaps try modprobe w1_therm";
        }
        qDebug() << "SYSFS:" << d->sysfsFile;

        if (!d->pollTimer) {
            d->pollTimer = new QTimer(this);
            d->pollTimer->setInterval(5000);
            connect(d->pollTimer, &QTimer::timeout, this, [this] {
                    d->readTemperature();
                }
            );
            qDebug() << "timer set up";
            d->readTemperature();
        }
        if (first) {
            qDebug() << "EMIT initialized";
            QTimer::singleShot(0, [this] { Q_EMIT initialized(); });
            //Q_EMIT initialized();
        }
        d->pollTimer->start();
    }

}

void ThermoSensor::setTemperature(const qreal &temperature)
{
    if (d->temperature != temperature) {
        d->temperature = temperature;
        Q_EMIT temperatureChanged();
    }
}

bool ThermoSensorPrivate::readTemperature()
{
    QFile d_file(sysfsFile);
    //qDebug() << "reading ..." << sysfsFile;
    d_file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(!d_file.isOpen()){
        qDebug() << "- Error, unable to open" << sysfsFile << "for reading";
        if (pollTimer) {
            pollTimer->stop();
        }
        return false;
    }
    QByteArray inputData = d_file.readAll();
    //qDebug() << "Read information: " << inputData;

    q->setTemperature(parseTemperature(inputData));
    return true;
}

qreal ThermoSensorPrivate::parseTemperature(const QByteArray &raw) const
{
    QString _in = QString::fromLocal8Bit(raw);
    auto lst = _in.split(QStringLiteral(" "));
    _in = lst.last();
    //qDebug() << "lst ..." << _in;
    if (!_in.startsWith(QStringLiteral("t=")) || !_in.endsWith(QStringLiteral("\n"))) {
        qWarning() << "Error parsing string, expected \"t=12345\n\", but got" << _in;
        return -1337.0;
    }
    _in = _in.trimmed(); // kill \n
    //qDebug() << "_in.trimmed()" << _in << _in.right(_in.count() - 2);
    _in = _in.right(_in.count() - 2);

    int millidegree = _in.toInt();
    qreal degree = (qreal)(millidegree) / 1000;
    qDebug() << "degree ..." << _in << millidegree << degree;


    return degree;

}

} // ns