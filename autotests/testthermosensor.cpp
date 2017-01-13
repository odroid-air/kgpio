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
// #include "kgpiotest.h"


#include <unistd.h>

#include "kgpio.h"
#include "gpiopin.h"
#include "thermosensor.h"

#include <QtTest/QtTest>
#include <QtCore/QFileInfo>
#include <QSignalSpy>

namespace KGpio {

class TestThermoSensor : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    void testThermoSensor();

    void cleanupTestCase();
};


/**
 * @see QTest::initTestCase()
 */
void TestThermoSensor::initTestCase()
{
}

void TestThermoSensor::cleanupTestCase()
{
}

void TestThermoSensor::testThermoSensor()
{
    auto the = new ThermoSensor;
    QSignalSpy initSpy(the, &ThermoSensor::initialized);
    QSignalSpy tempSpy(the, &ThermoSensor::temperatureChanged);


    const QString sensor_id = "28-0000080b1db9";
    the->setSensorId(sensor_id);
    QVERIFY(initSpy.wait(2000));

    QVERIFY(the->sensorId() == sensor_id);

    QTest::qWait(6000);

    delete the;
}
} // ns

QTEST_GUILESS_MAIN(KGpio::TestThermoSensor)

#include "testthermosensor.moc"