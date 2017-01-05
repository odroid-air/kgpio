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


#include "kgpio.h"
#include "kgpio_p.h"
#include "gpiopin.h"

#include <QDir>
#include <QDebug>
#include <QFile>
#include <QMap>

namespace KGpio {

static KGpio* s_kgpio = nullptr;


KGpio::KGpio()
    : d(new KGpioPrivate)
{
    if (!QFileInfo(d->sysfsDefaultPath).exists()) {
        qWarning() << "sysfs class gpio not found at " << d->sysfsDefaultPath;
    }
    QDir sysfs(d->sysfsDefaultPath);
    qDebug() << "sysfspath exists?" << sysfs.exists() << QFileInfo(d->sysfsDefaultPath).exists();
}

KGpio::~KGpio()
{
    s_kgpio = nullptr;
    qDeleteAll(d->pins);
    delete d;
}

KGpio* KGpio::self()
{
    if (s_kgpio == nullptr) {
        s_kgpio = new KGpio();
    }
    return s_kgpio;
}


GpioPin *KGpio::pin(int id)
{
    if (!d->pins.keys().contains(id)) {
        auto pin = new GpioPin(id);
        d->pins.insert(id, pin);
    }
    return d->pins.value(id);
}

QString KGpio::sysfsPath()
{
    return d->sysfsPath;
}

void KGpio::setSysfsPath(const QString &path)
{
    d->sysfsPath = path;
}

bool KGpio::testMode()
{
    return d->sysfsPath == d->sysfsDefaultPath;
}



}