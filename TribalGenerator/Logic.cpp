#include "Logic.h"

#include <stdlib.h>
#include <time.h>
#include <algorithm>

using namespace std;

MovementPresentation* MovementPresentationStorage::getRandomPresentation()
{
    MovementPresentation* pres = NULL;

    try
    {
        int collSize = static_cast<int>(coll.size());
        int index = Logic::genRandom(collSize);
        shared_ptr<MovementPresentation> presPtr = coll[index];
        pres = presPtr.get();
    }
    catch(...)
    {

    }

    return pres;
}

MovementPresentation* MovementPresentationStorage::getPresentationBySequenceNumber(int num)
{
    MovementPresentation* pres = NULL;

    try
    {
        shared_ptr<MovementPresentation> presPtr = coll[num];
        pres = presPtr.get();
    }
    catch(...)
    {

    }

    return pres;
}

Logic& Logic::getInstance()
{
    static Logic logic;
    return logic;
}

int Logic::genRandom(int maxVal, int minVal)
{
    int res = 0;

    if(maxVal <= minVal)
        return -1;

    srand(time(0));
    int randVal = rand();
    res = randVal % (maxVal-minVal);

    return res;
}

bool Logic::makeDataStorage(Data& ds, QString& errDescr)
{
    bool res = false;

    do
    {
        if(!m_dbController.init())
        {
            errDescr = m_dbController.getErrorString();
            break;
        }

        DbController::DataType rawData;
        if(!m_dbController.fetchData(rawData, errDescr))
        {
            break;
        }

        int rowCount = rawData[0].size();

        for(int rowIndex = 0; rowIndex < rowCount; rowIndex++)
        {
            shared_ptr<MovementPresentation> movement(new (std::nothrow) MovementPresentation);
            if(!movement.get())
                continue;

            movement->value = rawData[2][rowIndex];

            QPixmap pixmap;
            if(pixmap.loadFromData(rawData[2][rowIndex].toByteArray()))
            {
                movement->type = mptBlob;
            }
            else
            {
                movement->type = mptText;
                //qDebug()<<movement->value.toString();
            }

            wstring category(rawData[1][rowIndex].toString().toStdWString());

            //qDebug()<<rawData[1][rowIndex].toString();

            Data::iterator it = find_if(ds.begin(), ds.end(), DataEntryPred(category));
            if(it != ds.end())
            {
                it->second.coll.push_back(movement);
            }
            else
            {
                MovementPresentationStorage stor;
                stor.coll.push_back(movement);
                ds.push_back(make_pair(category, stor));
            }

        }

        res = !ds.empty();

    }while(0);

    return res;
}


bool Logic::isDataEnabled()
{
    return m_dbController.isDbOpen();
}

bool Logic::makeCategorizedDataStorage(CategorizedData& catData, int& maxIndex, QString& errDescr)
{
    bool res = false;

    do
    {
        if(!m_dbController.init())
        {
            errDescr = m_dbController.getErrorString();
            break;
        }

        DbController::DataType rawData;
        if(!m_dbController.fetchData(rawData, errDescr))
        {
            break;
        }

        int rowCount = static_cast<int>(rawData[0].size());
        int tempMaxIndex = 0;

        for(int rowIndex = 0; rowIndex < rowCount; rowIndex++)
        {
            CategorizedSeqEntry entry;

            entry.index = rawData[0][rowIndex].toInt();
            entry.value = rawData[2][rowIndex];

            tempMaxIndex = max(tempMaxIndex, entry.index);

            QPixmap pixmap;
            if(pixmap.loadFromData(rawData[2][rowIndex].toByteArray()))
            {
                entry.type = mptBlob;
            }
            else
            {
                entry.type = mptText;
            }

            bool catFound = false;
            wstring category(rawData[1][rowIndex].toString().toStdWString());

            for(CategorizedData::iterator it = catData.begin(); it != catData.end(); it++)
            {
                if(it->first.compare(category) == 0)
                {
                    it->second.push_back(entry);
                    catFound = true;
                    break;
                }
            }

            if(!catFound)
            {
                CategorizedSeq seq;
                seq.push_back(entry);
                catData.push_back(make_pair(category, seq));
            }
        }

        maxIndex = tempMaxIndex;
        res = !catData.empty();

    }while(0);

    return res;
}

bool Logic::submitDbChanges(const QString& category, CategorizedSeq& toAdd, CategorizedSeq& toDelete, CategorizedSeq& toUpdate)
{
    if(!m_dbController.reset())
    {
        return false;
    }

    QString errDescr;
    for(CategorizedSeq::iterator it = toDelete.begin(); it != toDelete.end(); it++)
    {
        CategorizedSeqEntry& entry = *it;
        m_dbController.deleteRecord(entry.index, category, errDescr);
    }

    for(CategorizedSeq::iterator it = toUpdate.begin(); it != toUpdate.end(); it++)
    {
        CategorizedSeqEntry& entry = *it;
        m_dbController.updateRecord(entry.index, category, entry.value, errDescr);
    }

    for(CategorizedSeq::iterator it = toAdd.begin(); it != toAdd.end(); it++)
    {
        CategorizedSeqEntry& entry = *it;
        m_dbController.insertRecord(entry.index, category, entry.value, errDescr);
    }

    return true;
}



