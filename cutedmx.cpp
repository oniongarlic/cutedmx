#include "cutedmx.h"

#include <QDebug>

CuteDMX::CuteDMX(QObject *parent)
    : QObject{parent}
{
    m_frame.resize(513, 0);

    m_worker=new CuteDMXWorker();
    m_worker->moveToThread(&m_thread);

    connect(&m_thread, &QThread::started, m_worker, &CuteDMXWorker::loop);
    connect(&m_thread, &QThread::finished, m_worker, &QObject::deleteLater);
}

CuteDMX::~CuteDMX()
{
    m_thread.requestInterruption();
    m_thread.quit();
    m_thread.wait();
}

void CuteDMX::setValue(uint channel, uint8_t value, bool delayd)
{
    if (channel>512 || channel<1) {
        qWarning("Invalid channel");
        return;
    }

    m_frame[channel]=value;

    if (!delayd)
        m_worker->updateFrame(m_frame);
}

void CuteDMX::updateFrame()
{
    m_worker->updateFrame(m_frame);
}

void CuteDMX::blackOut()
{
    QByteArray black(513, 0);

    m_worker->updateFrame(black);
}

void CuteDMX::start()
{
    m_thread.start();
}

void CuteDMX::stop()
{
    m_thread.requestInterruption();
}
