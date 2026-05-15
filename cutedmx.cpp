#include "cutedmx.h"

#include <QJsonDocument>
#include <QJsonArray>

#include <QDebug>

CuteDMX::CuteDMX(QObject *parent)
    : QObject{parent},
    m_active(false)
{
    m_frame.resize(513, 0);
}

CuteDMX::~CuteDMX()
{
    m_thread.requestInterruption();
    m_thread.quit();
    m_thread.wait();
}

void CuteDMX::newWorker()
{
    m_worker=new CuteDMXWorker();
    m_worker->setPort(m_device);
    m_worker->moveToThread(&m_thread);

    connect(&m_thread, &QThread::started, m_worker, &CuteDMXWorker::loop);
    connect(&m_thread, &QThread::finished, m_worker, &QObject::deleteLater);
    connect(m_worker, &CuteDMXWorker::isRunning, this, &CuteDMX::actived);
    connect(m_worker, &CuteDMXWorker::invalidPort, this, &CuteDMX::invalidPort);
    connect(m_worker, &CuteDMXWorker::destroyed, this, &CuteDMX::workerDestroyed);
    connect(m_worker, &CuteDMXWorker::destroyed, &m_thread, &QThread::quit);
}

void CuteDMX::setPort(const QString device)
{
    m_device=device;
}

void CuteDMX::setValue(uint channel, uint8_t value, bool delayd)
{
    if (channel>512 || channel<1) {
        qWarning("Invalid channel");
        return;
    }

    m_frame[channel]=value;

    if (!delayd && m_thread.isRunning())
        m_worker->updateFrame(m_frame);
}

uint8_t CuteDMX::value(uint channel)
{
    if (channel>512 || channel<1) {
        qWarning("Invalid channel");
        return 0;
    }

    return m_frame[channel];
}

void CuteDMX::updateFrame()
{
    if (!m_thread.isRunning())
        return;

    m_worker->updateFrame(m_frame);
}

void CuteDMX::blackOut()
{
    QByteArray black(513, 0);

    if (!m_thread.isRunning())
        return;

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

void CuteDMX::actived(bool enabled)
{
    qDebug() << "active" << enabled;
    
    m_active=enabled;
    emit activeChanged();
    
    emit isActive(enabled);
}

void CuteDMX::invalidPort()
{
    emit portError();
}

void CuteDMX::workerDestroyed(QObject *obj)
{
    qDebug() << "workerDestroyed";
    m_worker=nullptr;
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
    if (m_thread.isRunning())
        return;

    newWorker();

    m_thread.start();
}

void CuteDMX::stop()
{
    m_thread.requestInterruption();
}

bool CuteDMX::active() const
{
    return m_active;
}
