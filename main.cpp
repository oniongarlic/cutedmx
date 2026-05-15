#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

#include <QDebug>

#include <QSerialPortInfo>
#include "serialportinfomodel.h"

#include "cutedmx.h"
#include "dmxmodel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    app.setOrganizationName("TalOrg");
    app.setOrganizationDomain("tal.org");
    app.setApplicationName("CuteDMX");

    QQmlApplicationEngine engine;

    qmlRegisterType<CuteDMX>("org.tal.dmx", 1, 0, "CuteDMX");
    qmlRegisterType<DmxModel>("org.tal.dmx", 1, 0, "DmxModel");
    qmlRegisterType<SerialPortInfoModel>("org.tal.dmx", 1, 0, "SerialPortInfoModel");

    QQuickStyle::setStyle("Material");

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("cutedmx", "Main");

    return app.exec();
}
