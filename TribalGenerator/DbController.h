#ifndef DBCONTROLLER_H
#define DBCONTROLLER_H

#include <QtSql>
#include <vector>

class DbController
{
public:
    typedef std::vector< std::vector< QVariant > > DataType;

    DbController();
    ~DbController();

    bool init();
    bool reset();
    QString getErrorString();
    bool isDbOpen() const;
    bool fetchData(DataType& data, QString& errDescr);
    bool insertRecord(int index, const QString& category, const QVariant& value,
                      QString& errDescr);
    bool updateRecord(int index, const QString& category, const QVariant& value,
                      QString& errDescr);
    bool deleteRecord(int index, const QString& category, QString& errDescr);

private:
    void closeDb();
};

#endif // DBCONTROLLER_H
