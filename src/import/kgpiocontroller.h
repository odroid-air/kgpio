/************************************************************************
 *                                                                      *
 *   Copyright 2017 Sebastian Kügler <sebas@kde.org>                    *
 *                                                                      *
 *   This library is free software; you can redistribute it and/or      *
 *   modify it under the terms of the GNU Lesser General Public         *
 *   License as published by the Free Software Foundation; either       *
 *   version 2.1 of the License, or (at your option) version 3, or any  *
 *   later version accepted by the membership of KDE e.V. (or its       *
 *   successor approved by the membership of KDE e.V.), which shall     *
 *   act as a proxy defined in Section 6 of version 3 of the license.   *
 *                                                                      *
 *   This library is distributed in the hope that it will be useful,    *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  *
 *   Lesser General Public License for more details.                    *
 *                                                                      *
 *   You should have received a copy of the GNU Lesser General Public   *
 *   License along with this library.  If not, see                      *
 *   <http://www.gnu.org/licenses/>.                                    *
 *                                                                      *
 ************************************************************************/


#ifndef KGPIOCONTROLLER_H
#define KGPIOCONTROLLER_H

#include <QObject>
#include <QQmlListProperty>

#include <kgpio.h>
#include <gpiopin.h>

//namespace KGpio {

class KGpioControllerPrivate;

class KGPIO_EXPORT KGpioController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<KGpio::GpioPin> pins READ pins NOTIFY pinsChanged)

    public:
        explicit KGpioController(QObject* parent = nullptr);
        virtual ~KGpioController();

        QQmlListProperty<KGpio::GpioPin> pins();

    public Q_SLOTS:
        void pinAdded(KGpio::GpioPin* pin);
        void pinRemoved(KGpio::GpioPin* pin);

    Q_SIGNALS:
        void pinsChanged();

    private:
        QScopedPointer<KGpioControllerPrivate> d;
};

//} // ns

#endif // KGPIOCONTROLLER_H
