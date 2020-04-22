#include "DbManager.h"
#include <QString>

using namespace std;

DbManagerCmd::DbManagerCmd(CategorizedSeq& seq, Type type, int maxIndex)
    : m_type(type), m_maxIndex(maxIndex)
{
    copy(seq.begin(), seq.end(), back_inserter(m_seq));
}

void DbManagerCmd::execute(CategorizedSeq& seq, int& maxIndex)
{
    seq.clear();
    copy(m_seq.begin(), m_seq.end(), back_inserter(seq));
    maxIndex = m_maxIndex;
}

DbManager::DbManager() : m_curMaxIndex(0), m_dataModified(false), m_observerCount(0)
{

}

DbManager::~DbManager()
{

}

bool DbManager::queryCategories(vector<wstring>& categories)
{
    for(CategorizedData::iterator it = m_data.begin(); it != m_data.end(); it++)
    {
        categories.push_back(it->first);
    }

    return !categories.empty();
}

void DbManager::changeCategory(const std::wstring& category)
{
    // Loop through of our data list.
    for(CategorizedData::iterator it = m_data.begin(); it != m_data.end(); it++)
    {
        // Current entrie's category doesn't match - keep looping.
        if(it->first.compare(category) != 0)
            continue;

        // Current entrie's category is the same as current one - keep looping.
        if(it->first.compare(m_curCategory) == 0)
            continue;

        // Data aren't modified - just put another category and finish.
        if(m_dataModified)
        {
            // Ask the user about submitting changes.
            if(notifyCategoryChanging())
            {
                // In case of modification we have to submit or discard.
                // When discarded than just put another category and finish.
                if(submitChanges())
                {
                    // We're unable to fetch data after they've been changed.
                    // The old data are invalid whereby the new ones aren't available - just return control.
                    if(!fetchData())
                        return;

                    // We have new data thus we can put current category with new sequence.
                }
            }
            else
            {
                m_dataModified = false;
            }
        }

        putCurrentCategory(category);
        return;
    }
}

bool DbManager::addRecord(const QVariant& value)
{
    bool res = setUndoCmd(m_curSeq);
    if(res)
    {
        CategorizedSeq tempSeq(m_curSeq);
        CategorizedSeqEntry newEntry;

        newEntry.index = ++m_curMaxIndex;
        QPixmap pixmap;
        newEntry.type = (pixmap.loadFromData(value.toByteArray())) ? mptBlob :  mptText;
        newEntry.value = value;

        tempSeq.push_back(newEntry);

        swap(tempSeq, m_curSeq);
        m_dataModified = true;

        notifyRecordAdded(newEntry.index, newEntry.value);
    }

    return res;
}

bool DbManager::modifyRecord(int index, const QVariant& value)
{
    bool res = false;

    res = setUndoCmd(m_curSeq);
    if(res)
    {
        CategorizedSeq tempSeq(m_curSeq);
        for(CategorizedSeq::iterator it = tempSeq.begin(); it != tempSeq.end(); it++)
        {
            CategorizedSeqEntry& entry = *it;
            if(entry.index == index)
            {
                QPixmap pixmap;
                entry.type = (pixmap.loadFromData(value.toByteArray())) ? mptBlob :  mptText;
                entry.value = value;

                swap(tempSeq, m_curSeq);
                m_dataModified = true;

                notifyRecordModified(index, value);

                return res;
            }
        }
    }

    return res;
}

bool DbManager::deleteRecord(int index)
{
    bool res = false;

    res = setUndoCmd(m_curSeq);
    if(res)
    {
        CategorizedSeq tempSeq(m_curSeq);
        for(CategorizedSeq::iterator it = tempSeq.begin(); it != tempSeq.end(); it++)
        {
            CategorizedSeqEntry& entry = *it;
            if(entry.index == index)
            {
                tempSeq.erase(it);

                swap(tempSeq, m_curSeq);
                m_dataModified = true;

                notifyRecordDeleted(index);

                return res;
            }
        }
    }

    return res;
}

bool DbManager::moveRecordBegin(int index)
{
    bool found = false;
    CategorizedSeq::iterator itTarget;
    CategorizedSeq tempSeq(m_curSeq);

    for(CategorizedSeq::iterator it = tempSeq.begin(); it != tempSeq.end(); it++)
    {
        if(it->index == index)
        {
            itTarget = it;
            found = true;
            break;
        }
    }

    if(!found)
    {
        return false;
    }

    CategorizedSeq::iterator itStart = tempSeq.begin();
    if(itTarget == itStart)
    {
        return false;
    }

    setUndoCmd(m_curSeq);
    bool res = moveRecordUpwards(index, tempSeq, itTarget, itStart);

    swap(tempSeq, m_curSeq);
    m_dataModified = true;

    return res;
}

bool DbManager::moveRecordEnd(int index)
{
    bool found = false;
    CategorizedSeq::iterator itTarget;
    CategorizedSeq tempSeq(m_curSeq);

    for(CategorizedSeq::iterator it = tempSeq.begin(); it != tempSeq.end(); it++)
    {
        if(it->index == index)
        {
            itTarget = it;
            found = true;
            break;
        }
    }

    if(!found)
    {
        return false;
    }

    CategorizedSeq::iterator itFinish = tempSeq.end() - 1;
    if(itTarget == itFinish)
    {
        return false;
    }

    setUndoCmd(m_curSeq);
    bool res = moveRecordDownwards(index, tempSeq, itTarget, itFinish);

    swap(tempSeq, m_curSeq);
    m_dataModified = true;

    return res;
}

bool DbManager::moveRecordPrev(int index)
{
    bool found = false;
    CategorizedSeq::iterator itTarget;
    CategorizedSeq::iterator itPrev;
    CategorizedSeq tempSeq(m_curSeq);

    for(CategorizedSeq::iterator it = tempSeq.begin(); it != tempSeq.end(); it++)
    {
        if((it != tempSeq.begin()) && (it->index == index))
        {
            itTarget = it;
            itPrev = --it;
            found = true;
            break;
        }
    }

    if(!found)
    {
        return false;
    }

    setUndoCmd(m_curSeq);
    bool res = moveRecordUpwards(index, tempSeq, itTarget, itPrev);

    swap(tempSeq, m_curSeq);
    m_dataModified = true;

    return res;
}

bool DbManager::moveRecordNext(int index)
{
    bool found = false;
    CategorizedSeq::iterator itTarget;
    CategorizedSeq::iterator itNext;
    CategorizedSeq tempSeq(m_curSeq);

    for(CategorizedSeq::iterator it = tempSeq.begin(); it != tempSeq.end(); it++)
    {
        if((it != tempSeq.end() - 1) && (it->index == index))
        {
            itTarget = it;
            itNext = ++it;
            found = true;
            break;
        }
    }

    if(!found)
    {
        return false;
    }

    setUndoCmd(m_curSeq);
    bool res = moveRecordDownwards(index, tempSeq, itTarget, itNext);

    swap(tempSeq, m_curSeq);
    m_dataModified = true;

    return res;
}

void DbManager::undo()
{
    shared_ptr<DbManagerCmd> redoCmd(new (std::nothrow) DbManagerCmd(m_curSeq, DbManagerCmd::typeRedo, m_curMaxIndex));
    if(redoCmd.get())
    {
        m_redoList.push_back(redoCmd);
    }

    shared_ptr<DbManagerCmd> lastUndoCmd(m_undoList.back());
    m_undoList.pop_back();

    lastUndoCmd->execute(m_curSeq, m_curMaxIndex);
    m_dataModified = !isUndoListEmpty();

    notifyUndoCompleted(m_curSeq);
}

void DbManager::redo()
{
    shared_ptr<DbManagerCmd> undoCmd(new (std::nothrow) DbManagerCmd(m_curSeq, DbManagerCmd::typeUndo, m_curMaxIndex));
    if(undoCmd.get())
    {
        m_undoList.push_back(undoCmd);
    }

    shared_ptr<DbManagerCmd> lastRedoCmd(m_redoList.back());
    m_redoList.pop_back();

    lastRedoCmd->execute(m_curSeq, m_curMaxIndex);
    m_dataModified = !isUndoListEmpty();

    notifyRedoCompleted(m_curSeq);
}

bool DbManager::isUndoListEmpty()
{
    return m_undoList.empty();
}

bool DbManager::isRedoListEmpty()
{
    return m_redoList.empty();
}

bool DbManager::addObserver(DbManagerObserver* observer, int& cookie)
{
    if(!observer)
        return false;

    m_observers.insert(make_pair(++m_observerCount, observer));
    cookie = m_observerCount;
    return true;
}

bool DbManager::removeObserver(int cookie)
{
    ObserverCollection::const_iterator it = m_observers.find(cookie);
    if(it == m_observers.end())
        return false;

    m_observers.erase(it);
    return true;
}

bool DbManager::isDbModified()
{
    return m_dataModified;
}

void DbManager::putCurrentCategory(const std::wstring& category)
{
    for(CategorizedData::iterator it = m_data.begin(); it != m_data.end(); it++)
    {
        if(it->first.compare(category) != 0)
            continue;

        m_curCategory = it->first;
        CategorizedSeq& seq = it->second;
        m_curSeq.clear();
        copy(seq.begin(), seq.end(), back_inserter(m_curSeq));

        notifyCategoryChanged(m_curSeq);

        return;
    }
}

bool DbManager::prepareDataForSubmission(CategorizedSeq& toAdd, CategorizedSeq& toDelete,
                                         CategorizedSeq& toUpdate)
{
    bool res = false;

    do
    {
        // Get original sequence correspong to current category and copy data to temporary sequence.
        CategorizedSeq origSeq;
        for(CategorizedData::iterator it = m_data.begin(); it != m_data.end(); it++)
        {
            if(it->first.compare(m_curCategory) == 0)
            {
                CategorizedSeq& seq = it->second;
                copy(seq.begin(), seq.end(), back_inserter(origSeq));
                break;
            }
        }

        if(origSeq.empty())
            break;

        // Copy current sequence into temporary one.
        CategorizedSeq curSeq(m_curSeq);

        // Loop through original and current sequences and save entry indices.

        vector<int> origSeqIndices;
        vector<int> curSeqIndices;

        for(CategorizedSeq::iterator it = origSeq.begin(); it != origSeq.end(); it++)
        {
            origSeqIndices.push_back(it->index);
        }

        for(CategorizedSeq::iterator it = curSeq.begin(); it != curSeq.end(); it++)
        {
            curSeqIndices.push_back(it->index);
        }

        // Find out unchanged indexes.
        sort(origSeqIndices.begin(), origSeqIndices.end());
        sort(curSeqIndices.begin(), curSeqIndices.end());

        int x  = 0;
        for(CategorizedSeq::iterator it = origSeq.begin(); it != origSeq.end(); it++, x++)
        {
            QString str = QString(QObject::tr("Original %1 (%2): %3.")).arg(x+1).arg(it->index).arg(it->value.toString());
            qDebug()<<str;
        }

        x  = 0;
        for(CategorizedSeq::iterator it = curSeq.begin(); it != curSeq.end(); it++, x++)
        {
            QString str = QString(QObject::tr("Current %1 (%2): %3.")).arg(x+1).arg(it->index).arg(it->value.toString());
            qDebug()<<str;
        }

        vector<int> unchangedIndices;

        set_intersection(origSeqIndices.begin(), origSeqIndices.end(), curSeqIndices.begin(),
                         curSeqIndices.end(), back_inserter(unchangedIndices));

        // Having unchanged indices compare entries in both sequences. Save only those have been modified.
        for(vector<int>::iterator it = unchangedIndices.begin(); it != unchangedIndices.end(); it++)
        {
            int index = *it;
            CategorizedSeq::iterator origSeqEntryIt = find_if(origSeq.begin(), origSeq.end(), CatSeqEntryPred(index));
            CategorizedSeqEntry& origSeqEntry = *origSeqEntryIt;
            CategorizedSeq::iterator curSeqEntryIt = find_if(curSeq.begin(), curSeq.end(), CatSeqEntryPred(index));
            CategorizedSeqEntry& curSeqEntry = *curSeqEntryIt;

            if((origSeqEntry.type == curSeqEntry.type) && (origSeqEntry.value == curSeqEntry.value))
            {
                continue;
            }

            toUpdate.push_back(curSeqEntry);
        }

        x  = 0;
        for(CategorizedSeq::iterator it = toUpdate.begin(); it != toUpdate.end(); it++, x++)
        {
            QString str = QString(QObject::tr("To be updated %1 (%2): %3.")).arg(x+1).arg(it->index).arg(it->value.toString());
            qDebug()<<str;
        }

        // Determine entry indices to be deleted.

        vector<int> deletedIndices;

        sort(origSeqIndices.begin(), origSeqIndices.end());
        sort(unchangedIndices.begin(), unchangedIndices.end());
        set_difference(origSeqIndices.begin(), origSeqIndices.end(), unchangedIndices.begin(),
                       unchangedIndices.end(), back_inserter(deletedIndices));

        // Fill sequence of entries to be deleted.
        for(vector<int>::iterator it = deletedIndices.begin(); it != deletedIndices.end(); it++)
        {
            int index = *it;
            CategorizedSeq::iterator origSeqEntryIt = find_if(origSeq.begin(), origSeq.end(), CatSeqEntryPred(index));
            CategorizedSeqEntry& origSeqEntry = *origSeqEntryIt;
            toDelete.push_back(origSeqEntry);
        }

        x  = 0;
        for(CategorizedSeq::iterator it = toDelete.begin(); it != toDelete.end(); it++, x++)
        {
            QString str = QString(QObject::tr("To be deleted %1 (%2): %3.")).arg(x+1).arg(it->index).arg(it->value.toString());
            qDebug()<<str;
        }

        // Determine entry indices to be added.

        vector<int> addedIndices;

        sort(curSeqIndices.begin(), curSeqIndices.end());
        sort(unchangedIndices.begin(), unchangedIndices.end());
        set_difference(curSeqIndices.begin(), curSeqIndices.end(), unchangedIndices.begin(),
                       unchangedIndices.end(), back_inserter(addedIndices));

        // Fill sequence of entries to be deleted.
        for(vector<int>::iterator it = addedIndices.begin(); it != addedIndices.end(); it++)
        {
            int index = *it;
            CategorizedSeq::iterator curSeqEntryIt = find_if(curSeq.begin(), curSeq.end(), CatSeqEntryPred(index));
            CategorizedSeqEntry& curSeqEntry = *curSeqEntryIt;
            toAdd.push_back(curSeqEntry);
        }

        x  = 0;
        for(CategorizedSeq::iterator it = toAdd.begin(); it != toAdd.end(); it++, x++)
        {
            QString str = QString(QObject::tr("To be added %1 (%2): %3.")).arg(x+1).arg(it->index).arg(it->value.toString());
            qDebug()<<str;
        }

        res = true;

    }while(0);

    return res;
}

bool DbManager::submitChanges()
{
    bool res = false;

    do
    {
        CategorizedSeq toAdd;
        CategorizedSeq toDelete;
        CategorizedSeq toUpdate;

        if(!prepareDataForSubmission(toAdd, toDelete, toUpdate))
        {
            break;
        }

        if(!Logic::getInstance().submitDbChanges(QString::fromStdWString(m_curCategory),
                                                 toAdd, toDelete, toUpdate))
        {
            break;
        }

        notifyChangesSubmitted();

        m_data.clear();
        m_curMaxIndex = 0;
        m_curCategory.clear();
        m_curSeq.clear();
        m_dataModified = false;
        m_undoList.clear();
        m_redoList.clear();

        res = true;

    }while(0);

    return res;
}

void DbManager::declineChanges()
{
    notifyChangesDeclined();

    m_data.clear();
    m_curMaxIndex = 0;
    m_curCategory.clear();
    m_curSeq.clear();
    m_dataModified = false;
    m_undoList.clear();
    m_redoList.clear();
}

bool DbManager::fetchData()
{
    QString errDescr;
    bool res = Logic::getInstance().makeCategorizedDataStorage(m_data, m_curMaxIndex, errDescr);
    if(res)
    {
        notifyDataFetched(m_data);
    }

    return res;
}

bool DbManager::setUndoCmd(CategorizedSeq& seq)
{
    bool res = false;

    shared_ptr<DbManagerCmd> undoCmd(new (std::nothrow) DbManagerCmd(seq, DbManagerCmd::typeUndo, m_curMaxIndex));
    if(undoCmd.get())
    {
        m_undoList.push_back(undoCmd);
        res = true;
    }

    return res;
}

bool DbManager::moveRecordUpwards(int index, CategorizedSeq& tempSeq,
                                  CategorizedSeq::iterator& itTarget,
                                  CategorizedSeq::iterator& itDispos)
{
    CategorizedSeqEntry targetEntry(*itTarget);
    CategorizedSeqEntry disposEntry(*itDispos);

    int disposIndex = itDispos->index;
    targetEntry.index = disposIndex;
    disposEntry.index = index;

    CategorizedSeq::iterator itAfterDispos = tempSeq.erase(itDispos);
    CategorizedSeq::iterator itNewTarget = tempSeq.insert(itAfterDispos, targetEntry);

    bool found = false;
    for(CategorizedSeq::iterator it = itNewTarget + 1; it != tempSeq.end(); it++)
    {
        if(it->index == index)
        {
            itTarget = it;
            found = true;
            break;
        }
    }

    if(!found)
    {
        return false;
    }

    CategorizedSeq::iterator itAfterTarget = tempSeq.erase(itTarget);
    tempSeq.insert(itAfterTarget, disposEntry);

    notifyRecordMoved(index, disposIndex);

    return true;
}

bool DbManager::moveRecordDownwards(int index, CategorizedSeq& tempSeq, CategorizedSeq::iterator& itTarget,
                       CategorizedSeq::iterator& itDispos)
{
    CategorizedSeqEntry targetEntry(*itTarget);
    CategorizedSeqEntry disposEntry(*itDispos);

    int disposIndex = itDispos->index;
    targetEntry.index = disposIndex;
    disposEntry.index = index;

    CategorizedSeq::iterator itAfterDispos = tempSeq.erase(itDispos);
    CategorizedSeq::iterator itNewTarget = tempSeq.insert(itAfterDispos, targetEntry);

    bool found = false;
    for(CategorizedSeq::iterator it = tempSeq.begin(); it != itNewTarget; it++)
    {
        if(it->index == index)
        {
            itTarget = it;
            found = true;
            break;
        }
    }

    if(!found)
    {
        return false;
    }

    CategorizedSeq::iterator itAfterTarget = tempSeq.erase(itTarget);
    tempSeq.insert(itAfterTarget, disposEntry);

    notifyRecordMoved(index, disposIndex);

    return true;
}

void DbManager::notifyDataFetched(const CategorizedData& data)
{
    for(ObserverCollection::iterator it = m_observers.begin(); it != m_observers.end(); it++)
    {
        DbManagerObserver* observer = it->second;
        if(observer)
        {
            observer->notifyDataFetched(data);
        }
    }
}

void DbManager::notifyCategoryChanged(const CategorizedSeq& curSeq)
{
    for(ObserverCollection::iterator it = m_observers.begin(); it != m_observers.end(); it++)
    {
        DbManagerObserver* observer = it->second;
        if(observer)
        {
            observer->notifyCategoryChanged(curSeq);
        }
    }
}

void DbManager::notifyChangesSubmitted()
{
    for(ObserverCollection::iterator it = m_observers.begin(); it != m_observers.end(); it++)
    {
        DbManagerObserver* observer = it->second;
        if(observer)
        {
            observer->notifyChangesSubmitted();
        }
    }
}

void DbManager::notifyChangesDeclined()
{
    for(ObserverCollection::iterator it = m_observers.begin(); it != m_observers.end(); it++)
    {
        DbManagerObserver* observer = it->second;
        if(observer)
        {
            observer->notifyChangesDeclined();
        }
    }
}

void DbManager::notifyUndoCompleted(const CategorizedSeq& curSeq)
{
    for(ObserverCollection::iterator it = m_observers.begin(); it != m_observers.end(); it++)
    {
        DbManagerObserver* observer = it->second;
        if(observer)
        {
            observer->notifyUndoCompleted(curSeq);
        }
    }
}

void DbManager::notifyRedoCompleted(const CategorizedSeq& curSeq)
{
    for(ObserverCollection::iterator it = m_observers.begin(); it != m_observers.end(); it++)
    {
        DbManagerObserver* observer = it->second;
        if(observer)
        {
            observer->notifyRedoCompleted(curSeq);
        }
    }
}

void DbManager::notifyRecordMoved(int srcIndex, int dstIndex)
{
    for(ObserverCollection::iterator it = m_observers.begin(); it != m_observers.end(); it++)
    {
        DbManagerObserver* observer = it->second;
        if(observer)
        {
            observer->notifyRecordMoved(srcIndex, dstIndex);
        }
    }
}

void DbManager::notifyRecordAdded(int index, const QVariant& value)
{
    for(ObserverCollection::iterator it = m_observers.begin(); it != m_observers.end(); it++)
    {
        DbManagerObserver* observer = it->second;
        if(observer)
        {
            observer->notifyRecordAdded(index, value);
        }
    }
}

void DbManager::notifyRecordModified(int index, const QVariant& value)
{
    for(ObserverCollection::iterator it = m_observers.begin(); it != m_observers.end(); it++)
    {
        DbManagerObserver* observer = it->second;
        if(observer)
        {
            observer->notifyRecordModified(index, value);
        }
    }
}

void DbManager::notifyRecordDeleted(int index)
{
    for(ObserverCollection::iterator it = m_observers.begin(); it != m_observers.end(); it++)
    {
        DbManagerObserver* observer = it->second;
        if(observer)
        {
            observer->notifyRecordDeleted(index);
        }
    }
}

bool DbManager::notifyCategoryChanging()
{
    bool res = false;

    for(ObserverCollection::iterator it = m_observers.begin(); it != m_observers.end(); it++)
    {
        DbManagerObserver* observer = it->second;
        if(observer)
        {
            res |= observer->notifyCategoryChanging();
        }
    }

    return res;
}


