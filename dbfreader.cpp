#include <QFile>
#include <QByteArray>
#include <QDataStream>
#include <QString>
#include <QDebug>
#include <QTextCodec>


#include "dbfreader.h"

DbfReader::DbfReader()
{
    dbfFile = new QFile();

}

QList<QStringList> DbfReader::openDbfFile(QString path)
{
    QTextCodec *cStringCodec = QTextCodec::codecForCStrings();
    QTextCodec::setCodecForCStrings (QTextCodec::codecForName ("CP866"));
    bool parseSuccessfull = true;
    dbfFile->setFileName(path);
    if (dbfFile->open(QIODevice::ReadOnly)) {
        QDataStream dbfData(dbfFile);
        dbfData.setByteOrder(QDataStream::LittleEndian);

        quint8 type;
        quint8 year;
        quint8 month;
        quint8 day;
        quint32 numberOfRows;
        quint16 sizeOfHeader;
        quint16 sizeOfRow;
        dbfData >> type
                >> year
                >> month
                >> day
                >> numberOfRows
                >> sizeOfHeader
                >> sizeOfRow;

        dbfData.skipRawData(2); // Reserved
        dbfData.skipRawData(1); // Transaction info, skip
        dbfData.skipRawData(1); // Code info, skip
        dbfData.skipRawData(12); // Multiuser environment, skip
        dbfData.skipRawData(1); // Index info, skip

        quint8 codePage;
        dbfData >> codePage; // Code page

        dbfData.skipRawData(2); // Reserved

        QList<quint32> columnOffsets;
        QList<char *> rowByChars;

        quint16 columnsCount = (sizeOfHeader - 1 - 32) / 32;

        table.clear();
        QStringList header;
        for (quint16 column = 0; column < columnsCount; ++column) {
            char columnName[11]; // Column name
            quint8 columnType;
            quint32 columnDataOffset;
            quint8 columnSize;
            quint8 precision;

            dbfData.readRawData(columnName, 11);
            header.append(QString(columnName).trimmed());
            dbfData >> columnType
                    >> columnDataOffset
                    >> columnSize
                    >> precision;

            dbfData.skipRawData(2); // Reserved
            dbfData.skipRawData(1); // Workspace ID, not used, skip
            dbfData.skipRawData(2); // Multiuser mode, skip
            dbfData.skipRawData(1); // Set columns, skip
            dbfData.skipRawData(7); // Reserved
            dbfData.skipRawData(1); // .mdx index, skip

            columnOffsets.append(columnDataOffset);
        }

        table.append(header);

        for (quint16 column = 0; column < columnsCount; ++column) {
            if (column != columnsCount - 1) {
                rowByChars.append(new char[columnOffsets[column + 1] - columnOffsets[column]]);
            } else {
                rowByChars.append(new char[sizeOfRow - columnOffsets.last()]);
            }
        }
        quint8 endOfHeader; // MUST be 0x0d
        dbfData >> endOfHeader;
        if (endOfHeader == 0x0d) { // Can read rows
            QStringList str;

            for (quint32 row = 0; row < numberOfRows; ++row) {
                for (quint16 column = 0; column < columnsCount; ++column) {
                    if (column == 0) {
                        if (columnOffsets[column] != 0)
                            dbfData.skipRawData(columnOffsets[column]);
                    }
                    if (column != columnsCount - 1) {
                        dbfData.readRawData(rowByChars[column], columnOffsets[column + 1] - columnOffsets[column]);
                        str.append(QByteArray(rowByChars[column], columnOffsets[column + 1] - columnOffsets[column]).trimmed());
                    } else {
                        dbfData.readRawData(rowByChars[column], sizeOfRow - columnOffsets.last());
                        str.append(QByteArray(rowByChars[column], sizeOfRow - columnOffsets.last()).trimmed());
                    }

                }
                table.append(str);
                str.clear();
            }
        } else {
            parseSuccessfull = false;
            qDebug() << "Error in header";
        }

        for (quint16 column = 0; column < columnsCount; ++column) {
            delete [] rowByChars[column];
        }

        dbfFile->close();

    }
    QTextCodec::setCodecForCStrings(cStringCodec);
    return table;
}
