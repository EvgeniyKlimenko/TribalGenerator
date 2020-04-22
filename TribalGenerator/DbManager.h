#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "DbManagerDefs.h"
#include <list>
#include <algorithm>
#include <memory>

class DbManagerCmd
{
public:

    enum Type
    {
        typeUndo,
        typeRedo
    };

    DbManagerCmd(CategorizedSeq& seq, Type type, int maxIndex);
    void execute(CategorizedSeq& seq, int& maxIndex);
    inline void putType(Type type)
    {
        m_type = type;
    }

private:
    Type m_type;
    CategorizedSeq m_seq;
    int m_maxIndex;
};

class DbManager : public DbManagerInterface
{
    friend class AdjustDialog;

public:

    virtual bool queryCategories(std::vector<std::wstring>& categories);
    virtual void changeCategory(const std::wstring& category);
    virtual bool addRecord(const QVariant& value);
    virtual bool modifyRecord(int index, const QVariant& value);
    virtual bool deleteRecord(int index);
    virtual bool moveRecordBegin(int index);
    virtual bool moveRecordEnd(int index);
    virtual bool moveRecordPrev(int index);
    virtual bool moveRecordNext(int index);
    virtual void undo();
    virtual void redo();

    virtual bool isUndoListEmpty();
    virtual bool isRedoListEmpty();

    virtual bool addObserver(DbManagerObserver* observer, int& cookie);
    virtual bool removeObserver(int cookie);

    virtual bool isDbModified();

private:

    typedef std::list< std::shared_ptr< DbManagerCmd > > UndoRedoList;
    typedef std::map< int, DbManagerObserver* > ObserverCollection;

    template <class T> class IndexPred : public std::unary_function<T, bool>
    {
        int m_index;

    public:

        IndexPred(int index) : m_index(index)
        {

        }

        bool operator()(const T& entry) const
        {
            return (entry.index == m_index);
        }
    };

    typedef IndexPred<CategorizedSeqEntry> CatSeqEntryPred;

    DbManager();
    virtual ~DbManager();

    void putCurrentCategory(const std::wstring& category);
    bool submitChanges();
    void declineChanges();
    bool fetchData();
    bool setUndoCmd(CategorizedSeq& seq);
    bool moveRecordUpwards(int index, CategorizedSeq& tempSeq, CategorizedSeq::iterator& itTarget,
                           CategorizedSeq::iterator& itDispos);
    bool moveRecordDownwards(int index, CategorizedSeq& tempSeq, CategorizedSeq::iterator& itTarget,
                           CategorizedSeq::iterator& itDispos);
    bool prepareDataForSubmission(CategorizedSeq& toAdd, CategorizedSeq& toDelete, CategorizedSeq& toUpdate);

    void notifyDataFetched(const CategorizedData& data);
    void notifyCategoryChanged(const CategorizedSeq& curSeq);
    void notifyChangesSubmitted();
    void notifyChangesDeclined();
    void notifyUndoCompleted(const CategorizedSeq& curSeq);
    void notifyRedoCompleted(const CategorizedSeq& curSeq);
    void notifyRecordMoved(int srcIndex, int dstIndex);
    void notifyRecordAdded(int index, const QVariant& value);
    void notifyRecordModified(int index, const QVariant& value);
    void notifyRecordDeleted(int index);
    bool notifyCategoryChanging();

private:
    CategorizedData m_data;
    int m_curMaxIndex;
    std::wstring m_curCategory;
    CategorizedSeq m_curSeq;
    bool m_dataModified;
    UndoRedoList m_undoList;
    UndoRedoList m_redoList;
    ObserverCollection m_observers;
    int m_observerCount;
};

#endif // DBMANAGER_H
