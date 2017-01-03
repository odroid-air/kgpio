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
#ifndef GPIOPIN_H
#define GPIOPIN_H

#include <QtCore/QCoreApplication>
#include <QtCore/QString>

#include <kgpio_export.h>

namespace KGpio {

class GpioPinPrivate;

/**
 * GpioPin is a base class for reading and writing GPIO pins.
 * @short generic class for reading/writing gpio pins
 * @author Sebastian Kügler <sebas@kde.org>
 */
class KGPIO_EXPORT GpioPin : public QObject
{
    Q_OBJECT

public:

    enum PinError {
        NoError = 0,
        Uninitialized = 1, /** No sysfs node has been created */
        NoSysFs = 2, /** sysfs doesn't have gpio class */
        PermissionDenied = 4, /** can't write to sysfs/class/gpio */
        DirectionNotWritable = 8,
        ValueNotWritable = 16
    };
    Q_ENUMS(PinError)

    enum Direction {
        In = 0,
        Out = 1
    };
    Q_ENUMS(Direction)

    enum Value {
        Low = 0,
        High = 1
    };
    Q_ENUMS(Value)

    /**
     * Base constructor.
     */
    GpioPin(int id, QObject *parent = nullptr);

    virtual ~GpioPin();

    GpioPin::Direction direction() const;
    void setDirection(const GpioPin::Direction &dir);

    GpioPin::PinError error() const;
    QString errorString() const;

    GpioPin::Value value() const;
    void setValue(const GpioPin::Value &val);

private:
    GpioPinPrivate *const d;
};

} // namespace
#endif
