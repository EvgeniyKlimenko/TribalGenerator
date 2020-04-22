#include "DbControlWidget.h"
#include <string>

using namespace std;

DbControlWidget::DbControlWidget(QWidget* parent) : QWidget(parent),
    m_dbMgr(NULL), m_categories(NULL), m_movements(NULL), m_dbEdit(NULL),
    m_btnInsert(NULL), m_btnUpdate(NULL), m_btnDelete(NULL),
    m_btnUndo(NULL), m_btnRedo(NULL), m_btnMoveBegin(NULL),
    m_btnMoveEnd(NULL), m_btnMoveUp(NULL), m_btnMoveDown(NULL)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    m_categories = new QComboBox(this);

    mainLayout->addWidget(m_categories);

    m_movements = new QTableWidget(this);
    mainLayout->addWidget(m_movements, 5);

    QGridLayout* btnLayout = new QGridLayout(this);

    m_btnInsert = new QPushButton(QIcon(":/icons/insert.png"), tr("Insert"), this);
    m_btnInsert->setToolTip(tr("Insert new movement."));

    btnLayout->addWidget(m_btnInsert, 0, 0, Qt::AlignHCenter | Qt::AlignVCenter);

    connect(m_btnInsert, SIGNAL(clicked()), this, SLOT(onInsert()));

    m_btnUpdate = new QPushButton(QIcon(":/icons/update.png"), tr("Update"), this);
    m_btnUpdate->setToolTip(tr("Update movement."));

    btnLayout->addWidget(m_btnUpdate, 0, 1, Qt::AlignHCenter | Qt::AlignVCenter);

    connect(m_btnUpdate, SIGNAL(clicked()), this, SLOT(onUpdate()));
    connect(m_movements, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(onUpdate(int)));

    m_btnDelete = new QPushButton(QIcon(":/icons/delete.png"), tr("Delete"), this);
    m_btnDelete->setToolTip(tr("Delete movement."));

    btnLayout->addWidget(m_btnDelete, 0, 2, Qt::AlignHCenter | Qt::AlignVCenter);

    connect(m_btnDelete, SIGNAL(clicked()), this, SLOT(onDelete()));

    m_btnUndo = new QPushButton(QIcon(":/icons/undo.png"), tr("Undo"), this);
    m_btnUndo->setToolTip(tr("Undo last action."));

    btnLayout->addWidget(m_btnUndo, 0, 3, Qt::AlignHCenter | Qt::AlignVCenter);

    connect(m_btnUndo, SIGNAL(clicked()), this, SLOT(onUndo()));

    m_btnRedo = new QPushButton(QIcon(":/icons/redo.png"), tr("Redo"), this);
    m_btnUndo->setToolTip(tr("Redo last action."));

    connect(m_btnRedo, SIGNAL(clicked()), this, SLOT(onRedo()));

    btnLayout->addWidget(m_btnRedo, 0, 4, Qt::AlignHCenter | Qt::AlignVCenter);

    m_btnMoveBegin = new QPushButton(QIcon(":/icons/move_begin.png"), tr("Move begin"), this);
    m_btnMoveBegin->setToolTip(tr("Move current record to the beginning of table."));

    connect(m_btnMoveBegin, SIGNAL(clicked()), this, SLOT(onMoveBegin()));

    btnLayout->addWidget(m_btnMoveBegin, 1, 0, Qt::AlignHCenter | Qt::AlignVCenter);

    m_btnMoveEnd = new QPushButton(QIcon(":/icons/move_end.png"), tr("Move end"), this);
    m_btnMoveEnd->setToolTip(tr("Move current record to the end of table."));

    connect(m_btnMoveEnd, SIGNAL(clicked()), this, SLOT(onMoveEnd()));

    btnLayout->addWidget(m_btnMoveEnd, 1, 1, Qt::AlignHCenter | Qt::AlignVCenter);

    m_btnMoveUp = new QPushButton(QIcon(":/icons/move_up.png"), tr("Move up"), this);
    m_btnMoveUp->setToolTip(tr("Move current record up to single position."));

    connect(m_btnMoveUp, SIGNAL(clicked()), this, SLOT(onMoveUp()));

    btnLayout->addWidget(m_btnMoveUp, 1, 2, Qt::AlignHCenter | Qt::AlignVCenter);

    m_btnMoveDown = new QPushButton(QIcon(":/icons/move_down.png"), tr("Move down"), this);
    m_btnMoveDown->setToolTip(tr("Move current record down to single position."));

    connect(m_btnMoveDown, SIGNAL(clicked()), this, SLOT(onMoveDown()));

    btnLayout->addWidget(m_btnMoveDown, 1, 3, Qt::AlignHCenter | Qt::AlignVCenter);

    mainLayout->addLayout(btnLayout);

    m_dbEdit = new DbEditWidget(this);
    m_dbEdit->setVisible(false);
    mainLayout->addWidget(m_dbEdit);

    connect(m_dbEdit, SIGNAL(widgetActivated(bool)), this, SLOT(onDbEditActivated(bool)));

    setLayout(mainLayout);

    connect(m_categories, SIGNAL(activated(QString)), this, SLOT(onCategoryChanged(QString)));
    connect(m_dbEdit, SIGNAL(changesSaved()), this, SLOT(onDbEditChangesSaved()));
    connect(m_dbEdit, SIGNAL(changesDiscarded()), this, SLOT(onDbEditChangesDiscarded()));
}

DbControlWidget::~DbControlWidget()
{

}

void DbControlWidget::onCategoryChanged(const QString& text)
{
    m_dbMgr->changeCategory(text.toStdWString());
}

void DbControlWidget::onInsert()
{
    m_dbEdit->setVisible(true);
    m_dbEdit->putText(tr(""));
}

void DbControlWidget::onUpdate()
{
    int selIndex = -1;
    int index = getSelection(selIndex);
    if(index == -1)
        return;

    QLabel* label = (QLabel*)m_movements->cellWidget(selIndex, 1);

    m_dbEdit->putIndex(index);
    m_dbEdit->putText(label->text());
    m_dbEdit->setVisible(true);
}

void DbControlWidget::onUpdate(int selIndex)
{
    int index = getSelection(selIndex);
    if(index == -1)
        return;

    QLabel* label = (QLabel*)m_movements->cellWidget(selIndex, 1);

    m_dbEdit->putIndex(index);
    m_dbEdit->putText(label->text());
    m_dbEdit->setVisible(true);
}

void DbControlWidget::onDelete()
{
    int selIndex = -1;
    int index = getSelection(selIndex);
    if(index == -1)
        return;

    m_dbMgr->deleteRecord(index);
}

void DbControlWidget::onUndo()
{
    m_dbMgr->undo();
}

void DbControlWidget::onRedo()
{
    m_dbMgr->redo();
}

void DbControlWidget::onMoveBegin()
{
    int selIndex = -1;
    int index = getSelection(selIndex);
    if(index == -1)
        return;

    m_dbMgr->moveRecordBegin(index);
}

void DbControlWidget::onMoveEnd()
{
    int selIndex = -1;
    int index = getSelection(selIndex);
    if(index == -1)
        return;

    m_dbMgr->moveRecordEnd(index);
}

void DbControlWidget::onMoveUp()
{
    int selIndex = -1;
    int index = getSelection(selIndex);
    if(index == -1)
        return;

    m_dbMgr->moveRecordPrev(index);
}

void DbControlWidget::onMoveDown()
{
    int selIndex = -1;
    int index = getSelection(selIndex);
    if(index == -1)
        return;

    m_dbMgr->moveRecordNext(index);
}

void DbControlWidget::onDbEditChangesSaved()
{
    int index = m_dbEdit->getIndex();
    QVariant value(m_dbEdit->getText());
    if(index == -1)
    {
        m_dbMgr->addRecord(value);
    }
    else
    {
        m_dbMgr->modifyRecord(index, value);
    }
    m_dbEdit->setVisible(false);
}

void DbControlWidget::onDbEditChangesDiscarded()
{
    m_dbEdit->setVisible(false);
}

void DbControlWidget::onDbEditActivated(bool active)
{
    if(active)
    {
        disableControlButtons();
    }
    else
    {
        enableControlButtons(true);
    }

    m_categories->setEnabled(!active);
    m_movements->setEnabled(!active);
}

void DbControlWidget::notifyDataFetched(const CategorizedData& data)
{
    m_categories->clear();
    for(CategorizedData::const_iterator it = data.begin(); it != data.end(); it++)
    {
        QString category = QString::fromStdWString(it->first);
        m_categories->addItem(category);
    }

    if(m_categories->count())
    {
        m_categories->setCurrentIndex(0);
        m_dbMgr->changeCategory(m_categories->currentText().toStdWString());
    }
}

void DbControlWidget::notifyChangesSubmitted()
{
    //close();
}

void DbControlWidget::notifyChangesDeclined()
{
    //close();
}

void DbControlWidget::notifyCategoryChanged(const CategorizedSeq& curSeq)
{
    beginOperation();
    displayData(curSeq);
    endOperation(true);
}

void DbControlWidget::notifyUndoCompleted(const CategorizedSeq& curSeq)
{
    beginOperation();
    displayData(curSeq);
    endOperation(true);
}

void DbControlWidget::notifyRedoCompleted(const CategorizedSeq& curSeq)
{
    beginOperation();
    displayData(curSeq);
    endOperation(true);
}

void DbControlWidget::notifyRecordMoved(int srcIndex, int dstIndex)
{
    beginOperation();
    moveRecord(srcIndex, dstIndex);
    endOperation();
}

void DbControlWidget::notifyRecordAdded(int index, const QVariant& value)
{
    beginOperation();
    addRecord(index, value);
    endOperation();
}

void DbControlWidget::notifyRecordModified(int, const QVariant& value)
{
    beginOperation();
    modifyRecord(value);
    endOperation();
}

void DbControlWidget::notifyRecordDeleted(int)
{
    beginOperation();
    deleteRecord();
    endOperation();
}

bool DbControlWidget::notifyCategoryChanging()
{
    bool res = (QMessageBox::information(this, QApplication::applicationName(),
                                         tr("Category data have been changed. \n Would you like to submit?"),
                                         QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) ? true : false;

    return res;
}

void DbControlWidget::setDbManagerReference(DbManagerInterface* dbMgr)
{
    m_dbMgr = dbMgr;
}

void DbControlWidget::displayData(const CategorizedSeq& curSeq)
{
    m_movements->clearContents();

    int count = curSeq.size();
    m_movements->setRowCount(count);
    m_movements->setColumnCount(2);
    m_movements->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_movements->setSelectionMode(QAbstractItemView::SingleSelection);

    m_movements->horizontalHeader()->setVisible(false);
    m_movements->verticalHeader()->setVisible(false);

    count = 0;
    for(CategorizedSeq::const_iterator itSeq = curSeq.begin(); itSeq != curSeq.end(); itSeq++, count++)
    {
        const CategorizedSeqEntry& seqEntry = *itSeq;
        QLabel* label = new QLabel(m_movements);
        label->setProperty("index", QVariant(seqEntry.index));
        if(seqEntry.type == mptBlob)
        {
            QPixmap pixmap;
            if(pixmap.loadFromData(seqEntry.value.toByteArray()))
            {
                label->setPixmap(pixmap);
            }
        }
        else
        {
            label->setText(seqEntry.value.toString());
        }

        QString ordinalText = QString(tr("%1.")).arg(count+1);
        QLabel* ordinalLabel = new QLabel(m_movements);
        ordinalLabel->setText(ordinalText);
        ordinalLabel->setProperty("selIndex", QVariant(count));
        m_movements->setCellWidget(count, 0, ordinalLabel);
        m_movements->setCellWidget(count, 1, label);

        //qDebug()<<tr("Number: ")<<((QLabel*)m_movements->cellWidget(count, 0))->property("selIndex").toInt()<<tr(", text: ")<<
        //           ((QLabel*)m_movements->cellWidget(count, 0))->text();
    }

    m_movements->resizeRowsToContents();
    m_movements->resizeColumnsToContents();
}

int DbControlWidget::getSelection(int& selIndex)
{
    QList<QTableWidgetSelectionRange> selRangeList = m_movements->selectedRanges();
    QList<QTableWidgetSelectionRange>::iterator itSelRangeList = selRangeList.begin();
    if(itSelRangeList == selRangeList.end())
        return -1;

    QTableWidgetSelectionRange selRange = *itSelRangeList;
    selIndex = selRange.topRow();
    QLabel* label = (QLabel*)m_movements->cellWidget(selIndex, 1);
    if(!label)
        return -1;

    QVariant val(label->property("index"));
    if(!val.isValid())
        return -1;

    int index = val.toInt();
    return index;
}

QLabel* DbControlWidget::getSelectedCell()
{
    QList<QTableWidgetSelectionRange> selRangeList = m_movements->selectedRanges();
    QList<QTableWidgetSelectionRange>::iterator itSelRangeList = selRangeList.begin();
    if(itSelRangeList == selRangeList.end())
        return NULL;

    QTableWidgetSelectionRange selRange = *itSelRangeList;
    QLabel* label = (QLabel*)m_movements->cellWidget(selRange.topRow(), 1);

    return label;
}

int DbControlWidget::getSelIndex()
{
    QList<QTableWidgetSelectionRange> selRangeList = m_movements->selectedRanges();
    QList<QTableWidgetSelectionRange>::iterator itSelRangeList = selRangeList.begin();
    if(itSelRangeList == selRangeList.end())
        return -1;

    QTableWidgetSelectionRange selRange = *itSelRangeList;

    return selRange.topRow();
}

bool DbControlWidget::getCellsByIndices(int srcIndex, int dstIndex,
                                        int& srcTableIndex, int& dstTableIndex,
                                        QLabel** srcCell, QLabel** dstCell)
{
    bool res = false;
    int rowCount = m_movements->rowCount();
    QLabel* src = NULL;
    QLabel* dst = NULL;

    for(int i = 0; i < rowCount; i++)
    {
        QLabel* label = (QLabel*)m_movements->cellWidget(i, 1);
        if(!label)
            continue;

        QVariant val(label->property("index"));
        if(!val.isValid())
            continue;

        int index = val.toInt();
        if(index == srcIndex)
        {
            src = label;
            srcTableIndex = i;
        }
        else if(index == dstIndex)
        {
            dst = label;
            dstTableIndex = i;
        }

        if(src && dst)
            break;
    }

    res = src && dst && (srcTableIndex > -1) && (dstTableIndex > -1);

    *srcCell  = src;
    *dstCell = dst;

    return res;
}

void DbControlWidget::beginOperation()
{
    disableControlButtons();
}

void DbControlWidget::endOperation(bool emptinessImportant)
{
    enableControlButtons(emptinessImportant);
}

void DbControlWidget::disableControlButtons()
{
    m_btnInsert->setEnabled(false);
    m_btnUpdate->setEnabled(false);
    m_btnDelete->setEnabled(false);
    m_btnUndo->setEnabled(false);
    m_btnRedo->setEnabled(false);
    m_btnMoveBegin->setEnabled(false);
    m_btnMoveEnd->setEnabled(false);
    m_btnMoveUp->setEnabled(false);
    m_btnMoveDown->setEnabled(false);
}

void DbControlWidget::enableControlButtons(bool emptinessImportant)
{
    bool enabled =  true;

    if(emptinessImportant)
    {
        enabled = (m_movements->rowCount() == 0) ? false : true;
    }

    m_btnInsert->setEnabled(true);
    m_btnUpdate->setEnabled(enabled);
    m_btnDelete->setEnabled(enabled);
    bool undo = !m_dbMgr->isUndoListEmpty();
    bool redo = !m_dbMgr->isRedoListEmpty();
    m_btnUndo->setEnabled(undo);
    m_btnRedo->setEnabled(redo);
    m_btnMoveBegin->setEnabled(enabled);
    m_btnMoveEnd->setEnabled(enabled);
    m_btnMoveUp->setEnabled(enabled);
    m_btnMoveDown->setEnabled(enabled);
}

void DbControlWidget::addRecord(int index, const QVariant& value)
{
    QLabel* label = new QLabel(m_movements);
    label->setProperty("index", QVariant(index));
    if(value.type() == QMetaType::QByteArray)
    {
        QPixmap pixmap;
        if(pixmap.loadFromData(value.toByteArray()))
        {
            label->setPixmap(pixmap);
        }
    }
    else
    {
        label->setText(value.toString());
    }

    int count = m_movements->rowCount();
    m_movements->setRowCount(count + 1);

    QString ordinalText = QString(tr("%1.")).arg(count+1);
    QLabel* ordinalLabel = new QLabel(m_movements);
    ordinalLabel->setText(ordinalText);
    ordinalLabel->setProperty("selIndex", QVariant(count));
    m_movements->setCellWidget(count, 0, ordinalLabel);
    m_movements->setCellWidget(count, 1, label);

    m_movements->resizeRowsToContents();
    m_movements->resizeColumnsToContents();
}

void DbControlWidget::modifyRecord(const QVariant& value)
{
    QLabel* label = getSelectedCell();
    if(value.type() == QMetaType::QByteArray)
    {
        QPixmap pixmap;
        if(pixmap.loadFromData(value.toByteArray()))
        {
            label->setPixmap(pixmap);
        }
    }
    else
    {
        label->setText(value.toString());
    }

    m_movements->resizeRowsToContents();
    m_movements->resizeColumnsToContents();
}

void DbControlWidget::deleteRecord()
{
    int selIndex = getSelIndex();
    int rowCount = m_movements->rowCount();

    m_movements->removeCellWidget(selIndex, 1);
    m_movements->removeRow(selIndex);

    rowCount--;
    for(int i = 0; i < rowCount; i++)
    {
        QLabel* ordinalLabel = (QLabel*)m_movements->cellWidget(i, 0);
        if(!ordinalLabel)
            continue;

        //qDebug()<<tr("Number: ")<<((QLabel*)m_movements->cellWidget(i, 0))->property("selIndex").toInt()<<tr(", text: ")<<
        //           ((QLabel*)m_movements->cellWidget(i, 0))->text();

        if(((QLabel*)m_movements->cellWidget(i, 0))->property("selIndex").toInt() > selIndex)
        {
            QString ordinalText = QString(tr("%1.")).arg(i+1);
            ((QLabel*)m_movements->cellWidget(i, 0))->setText(ordinalText);
            ((QLabel*)m_movements->cellWidget(i, 0))->setProperty("selIndex", QVariant(i));
        }

        //qDebug()<<tr("Number: ")<<((QLabel*)m_movements->cellWidget(i, 0))->property("selIndex").toInt()<<tr(", text: ")<<
        //           ((QLabel*)m_movements->cellWidget(i, 0))->text();
    }

    m_movements->resizeRowsToContents();
    m_movements->resizeColumnsToContents();
}

void DbControlWidget::moveRecord(int srcIndex, int dstIndex)
{
    QLabel* srcCell = NULL;
    QLabel* dstCell = NULL;
    int srcTableIndex = 0;
    int dstTableIndex = 0;

    if(getCellsByIndices(srcIndex, dstIndex, srcTableIndex, dstTableIndex,
                         &srcCell, &dstCell))
    {
        QLabel* newSrcCell = new QLabel(m_movements);
        newSrcCell->setProperty("index", QVariant(dstIndex));
        if(srcCell->text().length())
        {
            newSrcCell->setText(srcCell->text());
        }
        else
        {
             newSrcCell->setPixmap(*srcCell->pixmap());
        }

        QLabel* newDstCell = new QLabel(m_movements);
        newDstCell->setProperty("index", QVariant(srcIndex));
        if(dstCell->text().length())
        {
            newDstCell->setText(dstCell->text());
        }
        else
        {
             newDstCell->setPixmap(*dstCell->pixmap());
        }

        m_movements->setCellWidget(srcTableIndex, 1, newDstCell);
        m_movements->setCellWidget(dstTableIndex, 1, newSrcCell);
        m_movements->setCurrentCell(dstTableIndex, 1, QItemSelectionModel::SelectCurrent);
    }
}
