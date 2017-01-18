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
#include "lightsensor.h"

#include <QtTest/QtTest>
#include <QtCore/QFileInfo>
#include <QSignalSpy>

namespace KGpio {

class TestLightSensor : public QObject
{
    Q_OBJECT

    QString readFile();

private Q_SLOTS:
    void initTestCase();
    void testLowLevelLightSensor();
    void testLightSensor();
    void cleanupTestCase();
};


/**
 * @see QTest::initTestCase()
 */
void TestLightSensor::initTestCase()
{
}

void TestLightSensor::cleanupTestCase()
{
    delete KGpio::KGpio::self();
}


void TestLightSensor::testLowLevelLightSensor()
{
//     return;
    auto light = new LightSensor(104, this);
    QSignalSpy initSpy(light, &GpioPin::initialized);
    QVERIFY(initSpy.wait());

    light->setDirection(GpioPin::Out);
    light->setValue(GpioPin::Low);
    qDebug() << "Light:" << light->direction() << light->value() << light->brightness();
    QCOMPARE(light->value(), GpioPin::Low);

    light->setDirection(GpioPin::In);
    QCOMPARE(light->value(), GpioPin::Low);
    bool go = true;
    int i = 0;
    while (go) {
        i++;
//         qDebug() << "low..." << light->value();
        if (light->value() == GpioPin::High) {
            qDebug() << "Now high!" << i << light->value();
            go = false;
        } else {
            QTest::qWait(3);
        }
    }

    QCOMPARE(light->value(), GpioPin::High);
    light->setDirection(GpioPin::Out);
    light->setValue(GpioPin::Low);
    light->deleteLater();

}
void TestLightSensor::testLightSensor()
{
    //return;
    auto light = new LightSensor(104, this);
    QSignalSpy initSpy(light, &GpioPin::initialized);
    QVERIFY(initSpy.wait());

    QSignalSpy brightnessSpy(light, &LightSensor::brightnessChanged);
    QSignalSpy valueSpy(light, &GpioPin::valueChanged);

    QVERIFY(brightnessSpy.wait(5000));
    qDebug() << "Light ... brightness" << light->brightness();
    qDebug() << "Light ... value" << light->value();

    QCOMPARE(valueSpy.count(), 1);

    QTest::qWait(20000);
}



} // ns

QTEST_GUILESS_MAIN(KGpio::TestLightSensor)

#include "testlightsensor.moc"
