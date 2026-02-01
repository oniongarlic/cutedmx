#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QDebug>

#include <QSerialPortInfo>
#include "serialportinfomodel.h"

#include "cutedmx.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qmlRegisterType<CuteDMX>("org.tal.dmx", 1, 0, "CuteDMX");
    qmlRegisterType<SerialPortInfoModel>("org.tal.dmx", 1, 0, "SerialPortInfoModel");

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("cutedmx", "Main");

    return app.exec();
}
