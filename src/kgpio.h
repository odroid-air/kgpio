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
#ifndef KGPIO_H
#define KGPIO_H

#include <QtCore/QCoreApplication>
#include <QtCore/QDate>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QHash>

#include <kgpio_export.h>

namespace KGpio {

class KGpioPrivate;
class GpioPin;

/**
 * KGpio is a factory class for reading and writing gpio pins.
 * @short factory class for reading/writing gpio pins
 * @author Sebastian Kügler <sebas@kde.org>
 */
class KGPIO_EXPORT KGpio
{
    Q_DECLARE_TR_FUNCTIONS(KGpio)

public:

    virtual ~KGpio();
    /**
     * Return the active plugin loader
     **/
    static KGpio* self();

    GpioPin* pin(int id);

    QString sysfsPath();
    void setSysfsPath(const QString &path);
    bool testMode();

private:
    KGpio();
    KGpioPrivate *const d;
};

} // namespace
#endif
