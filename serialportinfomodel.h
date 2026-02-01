#ifndef SERIALPORTINFOMODEL_H
#define SERIALPORTINFOMODEL_H

#include <QObject>

#include <QAbstractListModel>
#include <QSerialPortInfo>

class SerialPortInfoModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit SerialPortInfoModel(QObject *parent = nullptr);

    enum Roles {
        PortNameRole = Qt::UserRole + 1,
        SystemLocationRole,
        DescriptionRole,
        ManufacturerRole,
        SerialNumberRole,
        VendorIdentifierRole,
        ProductIdentifierRole,
        HasVendorIdentifierRole,
        HasProductIdentifierRole,
        IsBusyRole
    };
    Q_ENUM(Roles)

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void refresh();

private:
    QList<QSerialPortInfo> m_ports;
};


#endif // SERIALPORTINFOMODEL_H
