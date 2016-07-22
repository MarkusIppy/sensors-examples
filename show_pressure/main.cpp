#include <QtCore>
#include <QPressureSensor>
QT_USE_NAMESPACE

class PressureFilter : public QPressureFilter
{
public:
    qtimestamp stamp;
    bool filter(QPressureReading *reading)
    {
        int diff = ( reading->timestamp() - stamp );
        stamp = reading->timestamp();
        QTextStream out(stdout);
        out << "pressure value: " << reading->pressure()
            << QString(" (%1 ms since last, %2 Hz)").arg(diff / 1000, 5).arg( 1000000.0 / diff, 3, 'f', 1) << endl;
        return false; // don't store the reading in the sensor
    }
};

int main(int argc, char *argv[])
{

    QCoreApplication app(argc, argv);
    QStringList args = app.arguments();
    int rate_place = args.indexOf("-r");
    int rate_val = 0;
    if (rate_place != -1)
        rate_val = args.at(rate_place + 1).toInt();
    QPressureSensor sensor;
    if (!sensor.connectToBackend())
        qWarning() << "Could not connect to temperature backend";

    if (args.indexOf("-a") > 0) {
        sensor.setProperty("alwaysOn",true);
    }

    if (rate_val > 0) {
        sensor.setDataRate(rate_val);
    }
    PressureFilter filter;
    sensor.addFilter(&filter);
    sensor.start();
    if (!sensor.isActive()) {
        qWarning("Pressure Sensor didn't start!");
        return 1;
    }

    return app.exec();
}
