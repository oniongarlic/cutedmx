#include "serialportinfomodel.h"

SerialPortInfoModel::SerialPortInfoModel(QObject *parent)
    : QAbstractListModel(parent)
{
    refresh();
}

int SerialPortInfoModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_ports.size();
}

QVariant SerialPortInfoModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_ports.size())
        return {};

    const QSerialPortInfo &info = m_ports.at(index.row());

    switch (role) {
    case PortNameRole:
        return info.portName();
    case SystemLocationRole:
        return info.systemLocation();
    case DescriptionRole:
        return info.description();
    case ManufacturerRole:
        return info.manufacturer();
    case SerialNumberRole:
        return info.serialNumber();
    case VendorIdentifierRole:
        return info.vendorIdentifier();
    case ProductIdentifierRole:
        return info.productIdentifier();
    case HasVendorIdentifierRole:
        return info.hasVendorIdentifier();
    case HasProductIdentifierRole:
        return info.hasProductIdentifier();
    default:
        return {};
    }
}

QHash<int, QByteArray> SerialPortInfoModel::roleNames() const
{
    return {
        { PortNameRole,            "portName" },
        { SystemLocationRole,      "systemLocation" },
        { DescriptionRole,         "description" },
        { ManufacturerRole,        "manufacturer" },
        { SerialNumberRole,        "serialNumber" },
        { VendorIdentifierRole,    "vendorIdentifier" },
        { ProductIdentifierRole,   "productIdentifier" },
        { HasVendorIdentifierRole, "hasVendorIdentifier" },
        { HasProductIdentifierRole,"hasProductIdentifier" }
    };
}

void SerialPortInfoModel::refresh()
{
    beginResetModel();
    m_ports = QSerialPortInfo::availablePorts();
    endResetModel();
}
