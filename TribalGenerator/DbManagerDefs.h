#ifndef DBMANAGERDEFS_H
#define DBMANAGERDEFS_H

#include "Logic.h"
#include <vector>
#include <string>

struct DbManagerObserver
{
    virtual void notifyDataFetched(const CategorizedData& data) = 0;
    virtual void notifyChangesSubmitted() = 0;
    virtual void notifyChangesDeclined() = 0;
    virtual void notifyCategoryChanged(const CategorizedSeq& curSeq) = 0;
    virtual void notifyUndoCompleted(const CategorizedSeq& curSeq) = 0;
    virtual void notifyRedoCompleted(const CategorizedSeq& curSeq) = 0;
    virtual void notifyRecordMoved(int srcIndex, int dstIndex) = 0;
    virtual void notifyRecordAdded(int index, const QVariant& value) = 0;
    virtual void notifyRecordModified(int index, const QVariant& value) = 0;
    virtual void notifyRecordDeleted(int index) = 0;
    virtual bool notifyCategoryChanging() = 0;
};

struct DbManagerInterface
{
    virtual bool queryCategories(std::vector<std::wstring>& categories) = 0;
    virtual void changeCategory(const std::wstring& category)= 0;
    virtual bool addRecord(const QVariant& value) = 0;
    virtual bool modifyRecord(int index, const QVariant& value) = 0;
    virtual bool deleteRecord(int index) = 0;
    virtual bool moveRecordBegin(int index) = 0;
    virtual bool moveRecordEnd(int index) = 0;
    virtual bool moveRecordPrev(int index) = 0;
    virtual bool moveRecordNext(int index) = 0;
    virtual void undo() = 0;
    virtual void redo() = 0;

    virtual bool isUndoListEmpty() = 0;
    virtual bool isRedoListEmpty() = 0;

    virtual bool addObserver(DbManagerObserver* observer, int& cookie) = 0;
    virtual bool removeObserver(int cookie) = 0;

    virtual bool isDbModified() = 0;
};

struct DbManagerUser
{
    virtual void setDbManagerReference(DbManagerInterface* dbMgr) = 0;
};

#endif // DBMANAGERDEFS_H
