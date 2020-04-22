#ifndef DBCONTROLWIDGET_H
#define DBCONTROLWIDGET_H

#include <QtWidgets>
#include "DbEditWidget.h"
#include "DbManagerDefs.h"

class DbControlWidget : public QWidget, public DbManagerObserver, public DbManagerUser
{
    Q_OBJECT

public:
    DbControlWidget(QWidget* parent);
    virtual ~DbControlWidget();

    virtual void notifyDataFetched(const CategorizedData& data);
    virtual void notifyChangesSubmitted();
    virtual void notifyChangesDeclined();
    virtual void notifyCategoryChanged(const CategorizedSeq& curSeq);
    virtual void notifyUndoCompleted(const CategorizedSeq& curSeq);
    virtual void notifyRedoCompleted(const CategorizedSeq& curSeq);
    virtual void notifyRecordMoved(int srcIndex, int dstIndex);
    virtual void notifyRecordAdded(int index, const QVariant& value);
    virtual void notifyRecordModified(int, const QVariant& value);
    virtual void notifyRecordDeleted(int);
    virtual bool notifyCategoryChanging();

    virtual void setDbManagerReference(DbManagerInterface* dbMgr);

private slots:
    void onCategoryChanged(const QString& text);
    void onInsert();
    void onUpdate();
    void onUpdate(int selIndex);
    void onDelete();
    void onUndo();
    void onRedo();
    void onMoveBegin();
    void onMoveEnd();
    void onMoveUp();
    void onMoveDown();
    void onDbEditChangesSaved();
    void onDbEditChangesDiscarded();
    void onDbEditActivated(bool active);

private:
    void displayData(const CategorizedSeq& curSeq);
    int getSelection(int& selIndex);
    QLabel* getSelectedCell();
    int getSelIndex();
    bool getCellsByIndices(int srcIndex, int dstIndex,
                           int& srcTableIndex, int& dstTableIndex,
                           QLabel** srcCell, QLabel** dstCell);

    void beginOperation();
    void endOperation(bool emptinessImportant = false);

    void disableControlButtons();
    void enableControlButtons(bool emptinessImportant);

    void addRecord(int index, const QVariant& value);
    void modifyRecord(const QVariant& value);
    void deleteRecord();
    void moveRecord(int srcIndex, int dstIndex);

private:
    DbManagerInterface* m_dbMgr;
    QComboBox* m_categories;
    QTableWidget* m_movements;
    DbEditWidget* m_dbEdit;
    QPushButton* m_btnInsert;
    QPushButton* m_btnUpdate;
    QPushButton* m_btnDelete;
    QPushButton* m_btnUndo;
    QPushButton* m_btnRedo;
    QPushButton* m_btnMoveBegin;
    QPushButton* m_btnMoveEnd;
    QPushButton* m_btnMoveUp;
    QPushButton* m_btnMoveDown;
};

#endif // DBCONTROLWIDGET_H
