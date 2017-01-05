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

#include "kgpio.h"
#include "gpiopin.h"

#include <QtTest/QtTest>
#include <QtCore/QFileInfo>
#include <QSignalSpy>

namespace KGpio {

class KGpioTest : public QObject
{
    Q_OBJECT


private Q_SLOTS:
    void initTestCase();

    void testPinCreation();
    void testPins();

    void cleanupTestCase();
};


/**
 * @see QTest::initTestCase()
 */
void KGpioTest::initTestCase()
{
}

void KGpioTest::cleanupTestCase()
{
    delete KGpio::KGpio::self();
}

void KGpioTest::testPinCreation()
{
    auto pin = KGpio::KGpio::self()->pin(83);
    QVERIFY(pin != nullptr);

    auto pin83 = KGpio::KGpio::self()->pin(83);
    auto pin84 = KGpio::KGpio::self()->pin(84);
    auto pin85 = KGpio::KGpio::self()->pin(85);
    QCOMPARE(pin, pin83);
    QVERIFY(pin != pin84);
    QVERIFY(pin != pin85);

    //QVERIFY(pin->error() == GpioPin::PermissionDenied);
    KGpio::KGpio::self()->setSysfsPath("/blafarghl/rammstein/delta/1337");
    auto pin86 = KGpio::KGpio::self()->pin(86);
    QVERIFY(pin86->error() == GpioPin::NoSysFs);

    delete KGpio::KGpio::self();

    //auto pin87 = KGpio::KGpio::self()->pin(87);
    //QVERIFY(pin87->error() == GpioPin::PermissionDenied);


}

void KGpioTest::testPins()
{
    auto pin = KGpio::KGpio::self()->pin(83);
    QSignalSpy initSpy(pin, &GpioPin::initialized);

    QVERIFY(initSpy.wait());

    qDebug() << "pin direction:" << ((pin->direction() == GpioPin::In) ? "In!" : "Out!");

    pin->setDirection(GpioPin::Out);

    int interval = 66;
    int i = 0;
    if (pin->error() != GpioPin::NoError) {
        qDebug() << "pin error:" << pin->errorString() ;
        return;
    }

    while (i <= 10) {
        pin->setValue(GpioPin::High);
        QTest::qWait(interval);
        pin->setValue(GpioPin::Low);
        QTest::qWait(interval);
        i++;
    }
    pin->setValue(GpioPin::High);
    QTest::qWait(200);
    pin->setValue(GpioPin::Low);
    QTest::qWait(200);
    pin->setValue(GpioPin::High);
    QTest::qWait(200);
    pin->setValue(GpioPin::Low);
    QTest::qWait(200);
}

} // ns

QTEST_GUILESS_MAIN(KGpio::KGpioTest)

#include "kgpiotest.moc"