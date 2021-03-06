/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore>
#include <qtapsensor.h>

QT_USE_NAMESPACE

class TapSensorFilter : public QTapFilter
{
public:
    TapSensorFilter(QTapSensor* sensor): mySensor(sensor){}

    bool filter(QTapReading *reading)
    {
        int diff = ( reading->timestamp() - stamp );
        stamp = reading->timestamp();
        QString output;
        switch (reading->tapDirection()) {
            case QTapReading::X:
            case QTapReading::X_Both:
                output = "X";         break;
            case QTapReading::Y:
            case QTapReading::Y_Both:
                output = "Y";         break;
            case QTapReading::Z:
            case QTapReading::Z_Both:  
                output = "Z";         break;
            case QTapReading::X_Pos:     output = "X pos";     break;
            case QTapReading::Y_Pos:     output = "Y pos";     break;
            case QTapReading::Z_Pos:     output = "Z pos";     break;
            case QTapReading::X_Neg:     output = "X neg";     break;
            case QTapReading::Y_Neg:     output = "Y neg";     break;
            case QTapReading::Z_Neg:     output = "Z neg";     break;
            case QTapReading::Undefined: output = "Undefined"; break;
            default: output = "Invalid enum value";
        }



        QTextStream out(stdout);
        out << "Tap: ";
        if(reading->isDoubleTap()){
            out << "Double ";
            counter++;
            if (counter>2){
                mySensor->stop();
                mySensor->setProperty("returnDoubleTapEvents", false);
                mySensor->start();
            }
        }
        else
            out << "Single ";
        out << output<< QString(" (%1 ms since last, %2 Hz)").arg(diff / 1000, 5).arg( 1000000.0 / diff, 3, 'f', 1) << endl;
        return false; // don't store the reading in the sensor
    }
private:
    QTapSensor* mySensor;
    int counter;
    qtimestamp stamp;
};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QStringList args = app.arguments();
    int rate_place = args.indexOf("-r");
    int rate_val = 0;
    if (rate_place != -1)
        rate_val = args.at(rate_place + 1).toInt();


    QTapSensor doublesensor;
    doublesensor.connectToBackend();
    TapSensorFilter filter(&doublesensor);


    doublesensor.setProperty("returnDoubleTapEvents", true);
    if (rate_val > 0) {
        doublesensor.setDataRate(rate_val);
    }
    doublesensor.addFilter(&filter);
    doublesensor.start();
    if (!doublesensor.isActive()) {
        qWarning("Tapsensor (double) didn't start!");
        return 1;
    }

    QTapSensor singlesensor;
    singlesensor.connectToBackend();
    singlesensor.setProperty("returnDoubleTapEvents", false);

    bool isDouble = singlesensor.property("returnDoubleTapEvents").toBool();


    if (rate_val > 0) {
        singlesensor.setDataRate(rate_val);
    }
    singlesensor.addFilter(&filter);
    singlesensor.start();

    isDouble = singlesensor.property("returnDoubleTapEvents").toBool();

    if (!singlesensor.isActive()) {
        qWarning("Tapsensor (single) didn't start!");
        return 1;

    }


    return app.exec();
}
