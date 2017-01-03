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


#include "gpiopin.h"
#include "gpiopin_p.h"

#include <QDir>
#include <QDebug>
#include <QFileInfo>


namespace KGpio {

GpioPin::GpioPin(int id, QObject *parent)
    : QObject(parent)
    , d(new GpioPinPrivate)
{
    d->pinNumber = id;
    d->error = NoError;
    if (!d->nodeExists()) {
        qDebug() << "exporting..." << d->nodePath();
        d->exportPin();
    }
    qDebug() << "NOde??" << d->nodeExists();
    d->checkErrors();

}

GpioPin::~GpioPin()
{
    delete d;
}

GpioPin::PinError GpioPin::error() const
{
    return d->error;
}

QString GpioPin::errorString() const
{
    if (d->error == GpioPin::NoError) {
        return tr("No Error");
    }
    if (d->error == GpioPin::Uninitialized) {
        return tr("Uninitialized");
    }
    if (d->error == GpioPin::NoSysFs) {
        return tr("No Sysfs");
    }
    if (d->error == GpioPin::PermissionDenied) {
        return tr("Permission Denied");
    }
    if (d->error == GpioPin::DirectionNotWritable) {
        return tr("direction file not writable");
    }
    if (d->error == GpioPin::PermissionDenied) {
        return tr("value file not writable");
    }

    return tr("Unknown Error");
}

void GpioPinPrivate::checkErrors()
{

    // These are what matters in this broken state
    QFileInfo sysfsexport(QString(QStringLiteral("%1export")).arg(KGpio::self()->sysfsPath()));
    if (!sysfsexport.exists()) {
        //qWarning() << "sysfs class gpio not found at " << KGpio::self()->sysfsPath();
        error = GpioPin::NoSysFs;
        return;
    } else if (!sysfsexport.isWritable()) {
        //qWarning() << "sysfs class gpio found, but not writable for the current user: " << KGpio::self()->sysfsPath();
        error = GpioPin::PermissionDenied;
        return;
    }

    // On to the more interesting problems...

    const QStringList writableFiles({QStringLiteral("direction"), QStringLiteral("value")});

    for (const auto &f : writableFiles) {
        const QString _f = QString(QStringLiteral("%1%2")).arg(nodePath(), f);
        if (!QFileInfo(_f).exists() || !QFileInfo(_f).isWritable()) {
            qWarning() << "Permission problem: " << _f << " exists?" << QFileInfo(_f).exists() << "writable?" << QFileInfo(_f).isWritable();
            error = GpioPin::DirectionNotWritable;
            return;
        }
    }

}

bool GpioPinPrivate::exportPin()
{
    QFile d_file(QString(QStringLiteral("%1export")).arg(KGpio::self()->sysfsPath()));
    d_file.open(QIODevice::WriteOnly | QIODevice::Text);

    if(!d_file.isOpen()){
        //qDebug() << "- Error, unable to open" << "outputFilename" << "for output";
        return false;
    }
    QTextStream outStream(&d_file);
    outStream << QString::number(pinNumber) << endl;
    d_file.close();
    return true;
}

bool GpioPinPrivate::unexportPin()
{
    return true;
}

bool GpioPinPrivate::nodeExists() const
{
    return QFileInfo(QString(QStringLiteral("%1direction")).arg(nodePath())).exists();
 //   return QFileInfo(nodePath()).exists();
}

QString GpioPinPrivate::nodePath() const
{
    return QString(QStringLiteral("/sys/class/gpio/gpio%1/")).arg(QString::number(pinNumber));
}

GpioPin::Direction GpioPin::direction() const
{
    QFile d_file(QString(QStringLiteral("%1direction")).arg(d->nodePath()));
    d_file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(!d_file.isOpen()){
        //qDebug() << "- Error, unable to open" << "outputFilename" << "for output";
        d->error = GpioPin::PermissionDenied;
        return GpioPin::In;
    }
    QByteArray inputData = d_file.readAll();
    //qDebug() << "Direction is: " << inputData << "in?" << (inputData.startsWith(QStringLiteral("in").toLocal8Bit()));
    if (inputData.startsWith(QStringLiteral("in").toLocal8Bit())) {
        return GpioPin::In;
    }
    return GpioPin::Out;
}

void GpioPin::setDirection(const GpioPin::Direction &dir)
{
    QFile d_file(QString(QStringLiteral("%1direction")).arg(d->nodePath()));
    d_file.open(QIODevice::WriteOnly | QIODevice::Text);

    if(!d_file.isOpen()){
        //qDebug() << "- Error, unable to open" << "outputFilename" << "for output";
        d->error = DirectionNotWritable;
        return;
    }
    QTextStream outStream(&d_file);
    if (dir == GpioPin::In) {
        outStream << QStringLiteral("in\n");
    } else {
        outStream << QStringLiteral("out\n");
    }
    d_file.close();
}

GpioPin::Value GpioPin::value() const
{
    return GpioPin::High;
}

void GpioPin::setValue(const GpioPin::Value &val)
{
    QFile d_file(QString(QStringLiteral("%1value")).arg(d->nodePath()));
    d_file.open(QIODevice::WriteOnly | QIODevice::Text);

    if(!d_file.isOpen()){
        d->error = ValueNotWritable;
        return;
        //qDebug() << "- Error, unable to open" << "outputFilename" << "for output";
    }
    QTextStream outStream(&d_file);
    if (val == GpioPin::Low) {
        outStream << QStringLiteral("0\n");
    } else {
        outStream << QStringLiteral("1\n");
    }
    d_file.close();
}


}