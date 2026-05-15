#include "dmxmodel.h"

DmxModel::DmxModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

int DmxModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return m_rows.size();
}

int DmxModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return ColumnCount;
}

QVariant DmxModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return {};

    const DmxRow& row = m_rows[index.row()];

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (index.column())
        {
        case NameColumn:
            return row.name;

        case DelayColumn:
            return row.delay;

        case OrderColumn:
            return row.order;

        default:
            break;
        }
    }

    return {};
}

QVariant DmxModel::headerData(int section,
                              Qt::Orientation orientation,
                              int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case NameColumn:
            return "name";

        case DelayColumn:
            return "delay";

        case OrderColumn:
            return "order";

        default:
            break;
        }
    }

    return {};
}

QHash<int, QByteArray> DmxModel::roleNames() const
{
    return {
        { NameColumn, "name" },
        { DelayColumn, "delay" },
        { OrderColumn, "order" },
        { DmxColumn, "dmx" }
    };
}

bool DmxModel::setData(const QModelIndex& index,
                       const QVariant& value,
                       int role)
{
    if (!index.isValid() || role != Qt::EditRole)
        return false;

    DmxRow& row = m_rows[index.row()];

    switch (index.column())
    {
    case NameColumn:
        row.name = value.toString();
        break;

    case DelayColumn:
        row.delay = value.toInt();
        break;

    case OrderColumn:
        row.order = value.toInt();
        break;

    default:
        return false;
    }

    emit dataChanged(index, index);
    return true;
}

Qt::ItemFlags DmxModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsSelectable
           | Qt::ItemIsEnabled
           | Qt::ItemIsEditable
           | Qt::ItemIsDragEnabled
           | Qt::ItemIsDropEnabled;
}

bool DmxModel::moveRows(const QModelIndex& sourceParent,
                        int sourceRow,
                        int count,
                        const QModelIndex& destinationParent,
                        int destinationChild)
{
    if (count != 1)
        return false;

    if (sourceRow < 0 || sourceRow >= m_rows.size())
        return false;

    if (destinationChild < 0 || destinationChild > m_rows.size())
        return false;

    if (sourceRow == destinationChild ||
        sourceRow + 1 == destinationChild)
    {
        return false;
    }

    beginMoveRows(sourceParent,
                  sourceRow,
                  sourceRow,
                  destinationParent,
                  destinationChild);

    int destinationIndex = destinationChild;

    if (destinationIndex > sourceRow)
        destinationIndex--;

    m_rows.move(sourceRow, destinationIndex);

    endMoveRows();

    updateOrderFields();

    return true;
}

void DmxModel::addRow(const DmxRow& row)
{
    const int index = m_rows.size();

    beginInsertRows(QModelIndex(), index, index);
    m_rows.push_back(row);
    endInsertRows();
    updateOrderFields();
    emit countChanged();
}

void DmxModel::removeRow(int row)
{
    if (row < 0 || row >= m_rows.size())
        return;

    beginRemoveRows(QModelIndex(), row, row);
    m_rows.removeAt(row);
    endRemoveRows();
    updateOrderFields();
    emit countChanged();
}

quint8 DmxModel::dmxValue(int row, int channel) const
{
    if (row < 0 || row >= m_rows.size())
        return 0;

    if (channel < 0 || channel >= DmxChannelCount)
        return 0;

    return m_rows[row].dmx[channel];
}

void DmxModel::setDmxValue(int row, int channel, quint8 value)
{
    if (row < 0 || row >= m_rows.size())
        return;

    if (channel < 0 || channel >= DmxChannelCount)
        return;

    m_rows[row].dmx[channel] = value;
}

const DmxRow& DmxModel::rowData(int row) const
{
    return m_rows[row];
}

int DmxModel::count() const
{
    return m_rows.size();
}

int DmxModel::addRow(const QString name, int delay)
{
    const int row = m_rows.size();

    beginInsertRows({}, row, row);

    DmxRow scene;
    scene.name = name;
    scene.delay = delay;
    scene.order = row;

    m_rows.append(scene);

    endInsertRows();

    emit countChanged();

    return row;
}

void DmxModel::clear()
{
    beginResetModel();
    m_rows.clear();
    endResetModel();
    emit countChanged();
}

bool DmxModel::save(const QString& organization, const QString& application) const
{
    QSettings settings(organization, application);
    settings.beginGroup("DmxModel");
    settings.remove("");
    settings.beginWriteArray("rows");

    for (int i = 0; i < m_rows.size(); ++i)
    {
        settings.setArrayIndex(i);

        const DmxRow& row = m_rows[i];

        settings.setValue("name", row.name);
        settings.setValue("delay", row.delay);
        settings.setValue("order", row.order);

        QByteArray dmxBytes;
        dmxBytes.resize(DmxChannelCount);

        for (int ch = 0; ch < DmxChannelCount; ++ch)
            dmxBytes[ch] = static_cast<char>(row.dmx[ch]);

        settings.setValue("dmx", dmxBytes);
    }

    settings.endArray();
    settings.endGroup();

    return settings.status() == QSettings::NoError;
}

bool DmxModel::load(const QString& organization, const QString& application)
{
    beginResetModel();

    m_rows.clear();

    QSettings settings(organization, application);
    settings.beginGroup("DmxModel");

    const int size = settings.beginReadArray("rows");

    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);

        DmxRow row;

        row.name = settings.value("name").toString();
        row.delay = settings.value("delay").toInt();
        row.order = settings.value("order").toInt();

        const QByteArray dmxBytes =
            settings.value("dmx").toByteArray();

        for (int ch = 0;
             ch < DmxChannelCount && ch < dmxBytes.size();
             ++ch)
        {
            row.dmx[ch] =
                static_cast<quint8>(dmxBytes[ch]);
        }

        m_rows.push_back(row);
    }

    settings.endArray();
    settings.endGroup();

    endResetModel();

    return settings.status() == QSettings::NoError;
}

void DmxModel::updateOrderFields()
{
    for (int i = 0; i < m_rows.size(); ++i)
        m_rows[i].order = i;

    if (!m_rows.isEmpty())
    {
        emit dataChanged(
            index(0, OrderColumn),
            index(m_rows.size() - 1, OrderColumn));
    }
}