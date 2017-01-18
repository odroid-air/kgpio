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

class KGpioTest : public QObject
{
    Q_OBJECT

    QString readFile();

private Q_SLOTS:
    void initTestCase();

    void testPinCreation();
    void testPins();
    void testPinWrite();
    void testRaw();

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
    //return;
    auto pin = KGpio::KGpio::self()->pin(100);
    QVERIFY(pin != nullptr);

    auto pin100 = KGpio::KGpio::self()->pin(100);
    auto pin108 = new GpioPin();
    pin108->setNumber(108);
    pin108->setValue(GpioPin::High);
    auto pin97 = KGpio::KGpio::self()->pin(97);
    QCOMPARE(pin, pin100);
    QVERIFY(pin != pin108);
    QVERIFY(pin != pin97);

    //QVERIFY(pin->error() == GpioPin::PermissionDenied);
    KGpio::KGpio::self()->setSysfsPath("/blafarghl/rammstein/delta/1337");
    auto pin86 = KGpio::KGpio::self()->pin(86);
    QVERIFY(pin86->error() == GpioPin::NoSysFs);

    delete KGpio::KGpio::self();
    delete pin108;
    //auto pin87 = KGpio::KGpio::self()->pin(87);
    //QVERIFY(pin87->error() == GpioPin::PermissionDenied);
}

void KGpioTest::testPins()
{
    return;
    auto pin = KGpio::KGpio::self()->pin(100);
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

    QCOMPARE(pin->value(), GpioPin::Low);
    while (i <= 10) {
        pin->setValue(GpioPin::High);
        QCOMPARE(pin->value(), GpioPin::High);
        QTest::qWait(interval);
        pin->setValue(GpioPin::Low);
        QCOMPARE(pin->value(), GpioPin::Low);
        QTest::qWait(interval);
        i++;
    }
    pin->setValue(GpioPin::High);
    QTest::qWait(50);
    pin->setValue(GpioPin::Low);
    QTest::qWait(50);
    pin->setValue(GpioPin::High);
    QTest::qWait(50);
    pin->setValue(GpioPin::Low);
    QTest::qWait(50);
}

void KGpioTest::testPinWrite()
{
    auto pin = KGpio::KGpio::self()->pin(104);

    QSignalSpy initSpy(pin, &GpioPin::initialized);

    pin->setDirection(GpioPin::Out);
    QCOMPARE(pin->direction(), GpioPin::Out);

    QVERIFY(initSpy.wait());
    QCOMPARE(pin->value(), GpioPin::Low);

    qDebug() << "pin direction:" << ((pin->direction() == GpioPin::In) ? "In!" : "Out!");
    pin->setValue(GpioPin::High);
    return;
    QCOMPARE(pin->value(), GpioPin::High);

    pin->setValue(GpioPin::Low);
    QCOMPARE(pin->value(), GpioPin::Low);
}

QString KGpioTest::readFile()
{
    QFile f("/sys/devices/virtual/gpio/gpio104/value");
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    return f.readAll();
}

void KGpioTest::testRaw()
{
    return;
    {
        QFile f("/sys/devices/virtual/gpio/gpio104/value");
        f.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream stream(&f);
        stream << 0;
//         stream << QStringLiteral("0\n");
        auto  ee = fsync(f.handle());
        //f.flush();
        f.close();
        qDebug() << "Wrote zero to" << f.fileName() << " OK ? " << (ee == 0);
        QProcess p;
        p.start("sync");
        p.waitForFinished();

    }
    QCOMPARE(readFile(), QStringLiteral("0\n"));
}
} // ns

QTEST_GUILESS_MAIN(KGpio::KGpioTest)

#include "kgpiotest.moc"
