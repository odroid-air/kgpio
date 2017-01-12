/*
 *  Copyright 2015 Swbastian Kügler <sebas@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation;
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#include <QDebug>
#include <QFile>
#include <QProcess>
#include <QString>

#include <stdio.h>

QString readFile(const QString &filename)
{
    QFile d_file(filename);
    d_file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(!d_file.isOpen()){
        qDebug() << "- Error, unable to open" << "outputFilename" << "for output";
        return QString();
    }
    QByteArray inputData = d_file.readAll();
//     qDebug() << "Value is: " << inputData << "in?" << (inputData.startsWith(QStringLiteral("1").toLocal8Bit()));
    return inputData;
}

bool writeToFile(const QString &filename, const QString &content)
{
    QFile d_file(filename);
    d_file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Unbuffered);

    if (!d_file.isOpen()){
        qDebug() << "- Error, unable to open" << filename << "for output";
        return false;
    }
    QTextStream outStream(&d_file);
    outStream << content << endl;
    return true;
}

void brutalSync()
{
    /*
    The files in sysfs don't support O_SYNC (but should be sync'ed anyway,
    which they aren't, even using echo). fsync doesn't work because of that,
    but the 5k pound hammer (QProcess(sync) helps), so use that for now unti
    we find a real solution
    */
    QProcess p;
    p.start(QStringLiteral("sync"));
    p.waitForFinished();
    return;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("\n"
               " Usage :\n"
               " ./synctest /path/to/writeable/file\n");
        return 1;
    }

    QString zero = QStringLiteral("0");
    QString one = QStringLiteral("1");

    QString filename(argv[1]);
    //qDebug() << "Testing with:" << filename;

    writeToFile(filename, one);
    //return 0;
    brutalSync();
    auto a1 = readFile(filename);

    writeToFile(filename, zero);
    auto a2 = readFile(filename);

    brutalSync();
    auto a3 = readFile(filename);

    qDebug() << "initial value: (expect 1)" << a1;
    qDebug() << " ..after writing zero (expect 0): " << a2;
    qDebug() << " ..after brutal sync (expect 0):  " << a3;

    bool passed = a1.startsWith("1") && a2 == a3 && a3.startsWith("0");
    if (passed) {
        qDebug() << "synctest passed. :)";
    } else {
        qDebug() << "synctest failed. :(";
    }

    return passed ? 0 : 1;
}

