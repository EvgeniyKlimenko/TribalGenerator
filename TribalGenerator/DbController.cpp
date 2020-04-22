#include "DbController.h"

DbController::DbController()
{

}

DbController::~DbController()
{
    closeDb();
}

bool DbController::init()
{
    if(isDbOpen())
        return true;

    QSqlDatabase db(QSqlDatabase::addDatabase(QObject::tr("QSQLITE")));
    QString dbPath(qApp->applicationDirPath() + QObject::tr("/database/TribalGenDB.db"));
    db.setDatabaseName(dbPath);

    return db.open();
}

bool DbController::reset()
{
    if(isDbOpen())
        closeDb();

    return init();
}

QString DbController::getErrorString()
{
    return QSqlDatabase::database().lastError().text();
}

bool DbController::isDbOpen() const
{
    return QSqlDatabase::database().isOpen();
}

int g_colCount = 0;

bool DbController::fetchData(DataType& data, QString& errDescr)
{
    bool res = false;

    do
    {
        QSqlQuery query(QObject::tr("SELECT * FROM movements"));
        if(!query.exec())
        {
            errDescr = query.lastError().text();
            break;
        }

        QSqlRecord rec(query.record());
        int colCount = rec.count();
        int rowCount = query.size();

        if(!colCount || !rowCount)
            break;

        if(rowCount == -1)
        {
            rowCount = 0;
            while(query.next())
            {
                rowCount++;
            }
        }

        data.resize(colCount);
        for(DataType::iterator it = data.begin(); it != data.end(); it++)
        {
            it->resize(rowCount);
        }

        int rowIndex = 0;
        if(!query.first())
            break;

        do
        {
            for(int colIndex = 0; colIndex < colCount; colIndex++)
            {
                qDebug()<<query.value(colIndex).toString();

                data[colIndex][rowIndex] = query.value(colIndex);
            }

            rowIndex++;
        }while(query.next());

        res = true;

    }while(0);

    return res;
}

bool DbController::insertRecord(int index, const QString& category, const QVariant& value, QString& errDescr)
{
    bool res = false;

    qDebug()<<QObject::tr("Insert query. Index: ")<<index<<QObject::tr(", category: ")<<
               category<<QObject::tr(", value: ")<<value.toString()<<QObject::tr(".");

    do
    {
        QString statement(QObject::tr("INSERT INTO movements VALUES(:ID, :category, :movement)"));
        QSqlQuery query;

        if(!query.prepare(statement))
        {
            errDescr = query.lastError().text();
            break;
        }

        query.bindValue("ID", index);
        query.bindValue("category", category);
        query.bindValue("movement", value);

        res = query.exec();
        if(!res)
        {
            errDescr = query.lastError().text();
        }

    }while(0);

    return res;
}

bool DbController::updateRecord(int index, const QString& category, const QVariant& value, QString& errDescr)
{
    bool res = false;

    qDebug()<<QObject::tr("Update query. Index: ")<<index<<QObject::tr(", category: ")<<
               category<<QObject::tr(", value: ")<<value.toString()<<QObject::tr(".");

    do
    {
        QString statement(QObject::tr("INSERT OR REPLACE INTO movements VALUES(:ID, :category, :movement)"));
        QSqlQuery query;

        if(!query.prepare(statement))
        {
            errDescr = query.lastError().text();
            break;
        }

        query.bindValue("ID", index);
        query.bindValue("category", category);
        query.bindValue("movement", value);

        res = query.exec();
        if(!res)
        {
            errDescr = query.lastError().text();
        }

    }while(0);

    return res;
}

bool DbController::deleteRecord(int index, const QString& category, QString& errDescr)
{
    bool res = false;

    do
    {
        QString statement(QObject::tr("DELETE FROM movements WHERE ID=:ID AND category=:category"));
        QSqlQuery query;

        if(!query.prepare(statement))
        {
            errDescr = query.lastError().text();
            break;
        }

        query.bindValue("ID", index);
        query.bindValue("category", category);

        res = query.exec();
        if(!res)
        {
            errDescr = query.lastError().text();
        }

    }while(0);

    return res;
}

void DbController::closeDb()
{
    QSqlDatabase::database().close();
}


