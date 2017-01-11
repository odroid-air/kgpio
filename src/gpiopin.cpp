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
#include <QElapsedTimer>
#include <QFileInfo>
#include <QProcess>
#include <QTimer>


namespace KGpio {

GpioPin::GpioPin(int id, QObject *parent)
    : QObject(parent)
    , d(new GpioPinPrivate(this))
{
    d->pinNumber = id;
    d->error = NoError;
    if (!d->nodeExists()) {
        qDebug() << "exporting..." << d->nodePath();
        d->exportPin();
    }
    //qDebug() << "NOde??" << d->nodeExists();
    d->checkErrors();

    if (d->error == GpioPin::NoError) {
        QTimer::singleShot(0, this, [this] {
            //qDebug() << "inited in ctor";
            Q_EMIT initialized();

        });
    } else {
        d->elapsedTimer = new QElapsedTimer();
        d->elapsedTimer->start();
        d->initializationTimer = new QTimer(this);
        d->initializationTimer->setInterval(1);
        connect(d->initializationTimer, &QTimer::timeout, this, [this] {
            d->checkInitialized();
            if (d->error == NoError) {
                qDebug() << "+++ Initialized pin" << d->pinNumber << "after:" << d->elapsedTimer->elapsed();

                d->initializationTimer->deleteLater();
                d->initializationTimer = nullptr;
                delete d->elapsedTimer;
                d->elapsedTimer = nullptr;

                Q_EMIT initialized();
            } else if (d->elapsedTimer->elapsed() > 500) {
                qDebug() << "+++ Initializion of pin" << d->pinNumber << "FAILED after:" << d->elapsedTimer->elapsed();
                d->initializationTimer->deleteLater();
                d->initializationTimer = nullptr;
                delete d->elapsedTimer;
                d->elapsedTimer = nullptr;

                Q_EMIT initializationFailed();
            }
        });
        d->initializationTimer->start();
    }
}

GpioPin::~GpioPin()
{
    if (d->manageNode) {
        d->unexportPin();
    }
    delete d;
}

void GpioPinPrivate::checkInitialized()
{
    // check and wait for some files to be initialized
    const QStringList writableFiles({QStringLiteral("direction"), QStringLiteral("value")});
    error = GpioPin::NoError;

    for (const auto &f : writableFiles) {
        const QString _f = QString(QStringLiteral("%1%2")).arg(nodePath(), f);
        if (!QFileInfo(_f).exists() || !QFileInfo(_f).isWritable()) {
            //qWarning() << "Permission problem: " << _f << " exists?" << QFileInfo(_f).exists() << "writable?" << QFileInfo(_f).isWritable();
            error = GpioPin::DirectionNotWritable;
            return;
        }
    }
    checkErrors();
}

int GpioPin::number() const
{
    return d->pinNumber;
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

bool GpioPinPrivate::writeToFile(const QString &filename, const QString &content)
{
    //auto _content = QString(QStringLiteral("%1\n")).arg(content);
    auto _content = content;
    QFile d_file(filename);
    d_file.open(QIODevice::WriteOnly | QIODevice::Text);

//     qDebug() << "Writing" << content << "to" << filename;
    if (!d_file.isOpen()){
        qDebug() << "- Error, unable to open" << filename << "for output";
        return false;
    }
    QTextStream outStream(&d_file);
    outStream << _content;
//     auto wret = d_file.write(content.toLocal8Bit());
//     if (wret == -1) {
//         qDebug() << "- Write Error, " << filename << " e: " << d_file.error();
//     }
//     d_file.flush();
    d_file.close();
//     qDebug() << "Wrote ..." << _content << filename;
    QProcess p;
    p.start(QStringLiteral("sync"));
    p.waitForFinished();
    return true;
}

bool GpioPinPrivate::exportPin()
{
    const QString fname = QString(QStringLiteral("%1export")).arg(KGpio::self()->sysfsPath());
    if (writeToFile(fname, QString::number(pinNumber))) {
        manageNode = true;
        return true;
    }
    return false;
}

bool GpioPinPrivate::unexportPin()
{
    const QString fname = QString(QStringLiteral("%1unexport")).arg(KGpio::self()->sysfsPath());
    return writeToFile(fname, QString::number(pinNumber));
}

bool GpioPinPrivate::nodeExists() const
{
    return QFileInfo(QString(QStringLiteral("%1direction")).arg(nodePath())).exists();
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
    const QString fname = QString(QStringLiteral("%1direction")).arg(d->nodePath());
    if (dir == GpioPin::In) {
        d->writeToFile(fname, QStringLiteral("in"));
    } else {
        d->writeToFile(fname, QStringLiteral("out"));
    }
}

GpioPin::Value GpioPin::value() const
{
    QFile d_file(QString(QStringLiteral("%1value")).arg(d->nodePath()));
    d_file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(!d_file.isOpen()){
        qDebug() << "- Error, unable to open" << "outputFilename" << "for output";
        d->error = GpioPin::PermissionDenied;
        return GpioPin::Low;
    }
    QByteArray inputData = d_file.readAll();
//     qDebug() << "Value is: " << inputData << "in?" << (inputData.startsWith(QStringLiteral("1").toLocal8Bit()));
    if (inputData.startsWith(QStringLiteral("1").toLocal8Bit())) {
        return GpioPin::High;
    }
    return GpioPin::Low;
}

void GpioPin::setValue(const GpioPin::Value &val)
{
    const QString fname = QString(QStringLiteral("%1value")).arg(d->nodePath());
    QFileInfo fi(fname);

    if (val == GpioPin::Low) {
        d->writeToFile(fname, QStringLiteral("0"));
    } else {
        d->writeToFile(fname, QStringLiteral("1"));
    }
    if (val != value()) {
        qDebug() << "FAILED TO WRITE" << val << value() << direction() << fname;
        qDebug() << "   FileInfo: exists" << fi.exists() << "writable? " << fi.isWritable();
    } else {
        //qDebug() << "VAL OK!!";
    }
}

void GpioPin::writeValue(const QString& val)
{
    const QString fname = QString(QStringLiteral("%1value")).arg(d->nodePath());
    d->writeToFile(fname, val);
}



} // ns