#pragma once

#include <QAbstractTableModel>
#include <QSettings>
#include <QVector>
#include <QString>
#include <array>

struct DmxRow
{
    Q_GADGET

public:
    QString name;
    int delay = 0;
    int order = 0;
    QByteArray dmx;

    DmxRow()
    {
        dmx.resize(512, 0);
    }
};

class DmxModel : public QAbstractTableModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    static constexpr int DmxChannelCount = 512;

    enum Columns
    {
        NameColumn,
        DelayColumn,
        OrderColumn,
        DmxColumn,
        ColumnCount
    };

    explicit DmxModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;

    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override;

    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex& index,
                 const QVariant& value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    bool moveRows(const QModelIndex& sourceParent,
                  int sourceRow,
                  int count,
                  const QModelIndex& destinationParent,
                  int destinationChild) override;

    int count() const;

    Q_INVOKABLE int addRow(const QString name, int delay);
    void addRow(const DmxRow& row);
    Q_INVOKABLE void removeRow(int row);

    Q_INVOKABLE void clear();

    Q_INVOKABLE quint8 dmxValue(int row, int channel) const;
    Q_INVOKABLE void setDmxValue(int row, int channel, quint8 value);

    const DmxRow& rowData(int row) const;

    Q_INVOKABLE bool save(const QString& organization, const QString& application) const;
    Q_INVOKABLE bool load(const QString& organization, const QString& application);

signals:
    void countChanged();

private:
    void updateOrderFields();

private:
    QVector<DmxRow> m_rows;
};