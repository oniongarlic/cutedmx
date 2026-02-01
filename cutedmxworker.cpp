#include "cutedmxworker.h"

#include <QThread>
#include <QDebug>

CuteDMXWorker::CuteDMXWorker(QObject *parent)
    : QObject{parent}
{
    m_frame.resize(513, 0);
    m_device="/dev/ttyUSB0";
}

void CuteDMXWorker::setPort(const QString device)
{
    m_device=device;
}

void CuteDMXWorker::updateFrame(QByteArray &newframe)
{
    QMutexLocker locker(&m_mutex);

    if (newframe.size()!=513) {
        qWarning("Invalid frame size, ignoring");
        return;
    }

    m_frame=newframe;
}

void CuteDMXWorker::loop()
{
    m_port=new QSerialPort(this);
    m_port->setPortName(m_device);
    m_port->setBaudRate(250000);
    m_port->setDataBits(QSerialPort::Data8);
    m_port->setParity(QSerialPort::NoParity);
    m_port->setStopBits(QSerialPort::TwoStop);
    m_port->setFlowControl(QSerialPort::NoFlowControl);

    if (!m_port->open(QIODevice::WriteOnly)) {
        qCritical() << "Failed to open serial port" << m_device;
        delete m_port;
        return;
    }

    qDebug() << "DMX active on port" << m_device;

    emit isRunning(true);

    while (m_port->isOpen()) {

        send();

        if (QThread::currentThread()->isInterruptionRequested()) {
            qDebug("InterruptionRequested, byebye");
            m_port->close();            
        }
        QThread::yieldCurrentThread();
    }

    delete m_port;

    emit isRunning(false);
}

bool CuteDMXWorker::send()
{    
    m_port->setBreakEnabled(true);
    QThread::usleep(120);
    m_port->setBreakEnabled(false);
    QThread::usleep(12);

    m_mutex.lock();
    auto r=m_port->write(m_frame, 513);    
    m_mutex.unlock();

    QThread::msleep(25);

    return m_port->waitForBytesWritten(100);
}
