#include "cutedmx.h"

#include <QJsonDocument>
#include <QJsonArray>

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

void CuteDMX::setPort(const QString device)
{
    m_worker->setPort(device);
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

bool CuteDMX::fromJson(QString json)
{
    auto d=QJsonDocument::fromJson(json.toUtf8());
    if (!d.isArray() || d.isEmpty())
        return false;

    auto a=d.array();
    if (a.empty())
        return false;

    if (a.size()!=513)
        return false;

    // ignore first
    for (uint i=1;i<512;i++) {
        m_frame[i]=static_cast<unsigned char>(a.at(i).toInt());
    }

    return true;
}

QString CuteDMX::toJson()
{
    QJsonArray a;
    for (unsigned char b : std::as_const(m_frame)) {
        a.append(static_cast<int>(b));
    }

    QJsonDocument d(a);
    return d.toJson(QJsonDocument::Compact);
}

void CuteDMX::start()
{
    m_thread.start();
}

void CuteDMX::stop()
{
    m_thread.requestInterruption();
}
