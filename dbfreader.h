#ifndef DBFREADER_H
#define DBFREADER_H

#include <QList>
#include <QStringList>

class QFile;

class DbfReader
{
public:
    DbfReader();
    QList<QStringList> openDbfFile(QString path);
private:
    QFile *dbfFile;

    QList<QStringList> table;

    enum ColumnType
    {
        BinaryShort = 2,
        BinaryLong = 4,
        BinaryDouble = 8,
        Binary = 66,
        Char = 67,
        Date = 68,
        Float = 70,
        General = 71,
        Logical = 76,
        Memo = 77,
        Numeric = 78,
        Picture = 80,
        Variable = 86
    };
};

#endif // DBFREADER_H
