#include <QtCore>
#include <QHumiditySensor>
QT_USE_NAMESPACE

class HumidityFilter : public QHumidityFilter
{
public:
    qtimestamp stamp;
    bool filter(QHumidityReading *reading)
    {
        int diff = ( reading->timestamp() - stamp );
        stamp = reading->timestamp();
        QTextStream out(stdout);
        out << "r humidity value: " << reading->relativeHumidity()
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
    QHumiditySensor sensor;
    if (!sensor.connectToBackend())
        qWarning() << "Could not connect to humidity backend";

    if (args.indexOf("-a") > 0) {
        sensor.setProperty("alwaysOn",true);
    }

    if (rate_val > 0) {
        sensor.setDataRate(rate_val);
    }
    HumidityFilter filter;
    sensor.addFilter(&filter);
    sensor.start();
    if (!sensor.isActive()) {
        qWarning("Humidity Sensor didn't start!");
        return 1;
    }

    return app.exec();
}
