/*
 *   Copyright 2012 by Sebastian Kügler <sebas@kde.org>

 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "kgpioplugin.h"
#include "kgpiocontroller.h"

#include <gpiopin.h>
#include <lightsensor.h>

#include <QtQml>
#include <QQmlEngine>


using namespace KGpio;

void KGpioPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_ASSERT(QLatin1String(uri) == QLatin1String("org.kde.kgpio"));
    Q_UNUSED(engine)
    //qRegisterMetaType<KGpioController*>("KGpioController");
}

void KGpioPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(QLatin1String(uri) == QLatin1String("org.kde.kgpio"));
    qmlRegisterType<KGpioController>(uri, 1, 0, "KGpioController");
    qmlRegisterUncreatableType<GpioPin>(uri, 1, 0, "GpioPin", QStringLiteral("I can't do that, Dave."));
    qmlRegisterType<LightSensor>(uri, 1, 0, "LightSensor");
}
