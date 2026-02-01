#ifndef CUTEDMXWORKER_H
#define CUTEDMXWORKER_H

#include <QObject>
#include <QMutex>
#include <QtSerialPort/QSerialPort>

class CuteDMXWorker : public QObject
{
    Q_OBJECT
public:
    explicit CuteDMXWorker(QObject *parent = nullptr);

    void setPort(const QString device);

public slots:
    void updateFrame(QByteArray &newframe);
    void loop();

signals:
    void isRunning(bool running);

private:
    QMutex m_mutex;
    QSerialPort *m_port;    
    QByteArray m_frame;
    QString m_device;
    bool send();    
};

#endif // CUTEDMXWORKER_H
