#include <QtCore/QCoreApplication>
#include <QDebug>
#include "dbfreader.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "Hi";

    DbfReader dbfReader;

    dbfReader.openDbfFile("test.DBF");
    
    return a.exec();
}
