#ifndef CUTEDMX_H
#define CUTEDMX_H

#include <QObject>
#include <QThread>
#include <QtSerialPort/QSerialPort>

#include "cutedmxworker.h"

class CuteDMX : public QObject
{
    Q_OBJECT
public:
    explicit CuteDMX(QObject *parent = nullptr);
    ~CuteDMX();

    Q_INVOKABLE void setPort(const QString device);

    Q_INVOKABLE void setValue(uint channel, uint8_t value, bool delayd=false);
    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

    Q_INVOKABLE void updateFrame();
    Q_INVOKABLE void blackOut();

signals:

private:
    QThread m_thread;
    QByteArray m_frame;

    CuteDMXWorker *m_worker;
};

#endif // CUTEDMX_H
