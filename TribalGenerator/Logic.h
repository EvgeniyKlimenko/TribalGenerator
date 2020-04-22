#ifndef LOGIC_H
#define LOGIC_H

#include <string>
#include <vector>
#include <list>
#include <memory>

#include "DbController.h"

enum MovementPresentationType
{
    mptText,
    mptBlob
};

struct MovementPresentation
{
    MovementPresentationType type;
    QVariant value;
};

typedef std::vector< std::shared_ptr< MovementPresentation > > MovementPresentationCollection;

struct MovementPresentationStorage
{
    MovementPresentationCollection coll;
    MovementPresentation* getRandomPresentation();
    MovementPresentation* getPresentationBySequenceNumber(int num);
};

template <class T> class GenericPred : public std::unary_function<T, bool>
{
    std::wstring m_category;

public:

    GenericPred(const std::wstring& category) : m_category(category)
    {

    }

    bool operator()(const T& entry) const
    {
        return (m_category.compare(entry.first) == 0);
    }
};

typedef std::pair< std::wstring, MovementPresentationStorage > DataEntry;
typedef std::list<DataEntry> Data;
typedef GenericPred<DataEntry> DataEntryPred;


typedef MovementPresentationType ValueType;

struct CategorizedSeqEntry
{
    int index;
    ValueType type;
    QVariant value;
};

typedef std::vector< CategorizedSeqEntry > CategorizedSeq;

typedef std::pair< std::wstring, CategorizedSeq > CategorizedDataEntry;
typedef std::list<CategorizedDataEntry> CategorizedData;
typedef GenericPred<CategorizedDataEntry> CategorizedDataEntryPred;

class Logic
{
public:
    static Logic& getInstance();
    static int genRandom(int maxVal, int minVal = 0);

    bool makeDataStorage(Data& ds, QString& errDescr);
    bool makeCategorizedDataStorage(CategorizedData& catData, int& maxIndex, QString& errDescr);
    bool isDataEnabled();
    bool submitDbChanges(const QString& category, CategorizedSeq& toAdd,
                         CategorizedSeq& toDelete, CategorizedSeq& toUpdate);

private:
    Logic() {}
    ~Logic() {}

private:
    DbController m_dbController;
};

#endif // LOGIC_H
