#include "mainwindow.h"

#include <algorithm>
#include <time.h>

#include "DataWidget.h"
#include "AdjustDialog.h"

#pragma warning(disable: 4996)

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    m_table(NULL), m_actionGenerate(NULL), m_actionAdjust(NULL),
    m_actionQuit(NULL), m_actionAbout(NULL), m_toolbar(NULL),
    m_menuCommands(NULL), m_mruMapper(NULL), m_mruSep(NULL),
    m_dataMapper(NULL), m_appConfiguration(NULL)
{
    createPresentation();
    adjustWindowSize();

    createActions();
    createMenu();
    createToolbar();
    createMru();

    loadAppConfiguration();

    QString errDescr;
    if(!Logic::getInstance().makeDataStorage(m_ds, errDescr))
    {
        statusBar()->showMessage(errDescr, s_sbMsgTimeout);
    }
    else
    {
        statusBar()->showMessage(tr("Ready"), s_sbMsgTimeout);
    }

    vector<wstring> categories;
    for(Data::iterator it = m_ds.begin(); it != m_ds.end(); it++)
    {
        wstring& category = it->first;
        categories.push_back(category);
    }

    fillCategoriesInitially(categories);

    updateUI();
}

MainWindow::~MainWindow()
{

}

void MainWindow::adjustWindowSize()
{
    QRect geometry(QApplication::desktop()->screenGeometry());
    QSize minSize(geometry.width()/2, geometry.height()/2);

    setMinimumSize(minSize);
}

void MainWindow::adjustPresentation()
{
    QSize tableSize(m_table->size());
    int colWidth = tableSize.width() / 5;
    m_table->setColumnWidth(0, colWidth);
    colWidth *= 4;
    m_table->setColumnWidth(1, colWidth);

    int rowCount = m_table->rowCount();
    int rowHeight = minimumSize().height() / 8;
    for(int i = 0; i < rowCount; i++)
    {
        m_table->setRowHeight(i, rowHeight);
    }
}

void MainWindow::createPresentation()
{
    m_table = new QTableWidget(s_rowCount, s_colCount, this);

    QStringList headers;
    headers<<tr("Category")<<tr("Movement");
    m_table->setHorizontalHeaderLabels(headers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);

    m_table->show();

    setCentralWidget(m_table);

    QHeaderView* horHeader = m_table->horizontalHeader();
    horHeader->setSectionResizeMode(QHeaderView::Fixed);
    QFont headerFont(QApplication::font());
    headerFont.setPointSize(headerFont.pointSize() + 2);
    headerFont.setBold(true);
    horHeader->setFont(headerFont);

    QHeaderView* vertHeader = m_table->verticalHeader();
    vertHeader->setVisible(false);

    m_dataMapper = new QSignalMapper(this);
    connect(m_dataMapper, SIGNAL(mapped(QWidget*)), this, SIGNAL(dataTriggered(QWidget*)));
    connect(this, SIGNAL(dataTriggered(QWidget*)), this, SLOT(onDataGenRequest(QWidget*)));
}

void MainWindow::createActions()
{
    m_actionGenerate = new QAction(tr("&Generate"), this);
    m_actionGenerate->setIcon(QIcon(tr(":/icons/generate.png")));
    m_actionGenerate->setStatusTip(tr("Generate new movement"));
    connect(m_actionGenerate, SIGNAL(triggered()), this, SLOT(onGenerate()));

    m_actionAdjust = new QAction(tr("A&djust..."), this);
    m_actionAdjust->setIcon(QIcon(tr(":/icons/adjust.png")));
    m_actionAdjust->setStatusTip(tr("Edit program database"));
    connect(m_actionAdjust, SIGNAL(triggered()), this, SLOT(onAdjust()));

    m_actionQuit = new QAction(tr("&Quit"), this);
    m_actionQuit->setIcon(QIcon(tr(":/icons/quit.png")));
    m_actionQuit->setStatusTip(tr("Quit the program"));
    connect(m_actionQuit, SIGNAL(triggered()), this, SLOT(onQuit()));

    m_actionAbout = new QAction(tr("&About"), this);
    m_actionAbout->setIcon(QIcon(tr(":/icons/about.png")));
    m_actionAbout->setStatusTip(tr("About the program"));
    connect(m_actionAbout, SIGNAL(triggered()), this, SLOT(onAbout()));
}

void MainWindow::createMenu()
{
    m_menuCommands = menuBar()->addMenu(tr("&Commands"));
    m_menuCommands->addAction(m_actionGenerate);
    m_menuCommands->addAction(m_actionAdjust);
    m_mruSep = m_menuCommands->addSeparator();
    m_menuCommands->addAction(m_actionQuit);

    QMenu* menuAbout = menuBar()->addMenu(tr("&About"));
    menuAbout->addAction(m_actionAbout);
}

void MainWindow::createToolbar()
{
    m_toolbar = new QToolBar(tr("Main toolbar"), this);
    m_toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_toolbar->setMovable(false);
    m_toolbar->addAction(m_actionGenerate);
    m_toolbar->addAction(m_actionAdjust);
    m_toolbar->addSeparator();
    m_toolbar->addAction(m_actionAbout);

    addToolBar(Qt::TopToolBarArea, m_toolbar);
}

void MainWindow::createMru()
{
    m_mruMapper = new QSignalMapper(this);
    connect(m_mruMapper, SIGNAL(mapped(QObject*)), this, SIGNAL(mruTriggered(QObject*)));
    connect(this, SIGNAL(mruTriggered(QObject*)), this, SLOT(onMru(QObject*)));
}

void MainWindow::onGenerate()
{
    generate();
}

void MainWindow::onAdjust()
{
    AdjustDialog adjustDlg(m_appConfiguration, this);
    connect(&adjustDlg, SIGNAL(databaseDataChanged()), this, SLOT(onDatabaseDataChanged()));
    adjustDlg.exec();
    disconnect(&adjustDlg, SIGNAL(databaseDataChanged()), this, SLOT(onDatabaseDataChanged()));
}

void MainWindow::onAbout()
{
    QString aboutText(tr("A program for modeling movement combinations in the Tribal Fusion.\n"));
    aboutText += tr("Version: ") + QString(APPLICATION_VERSION) + tr(".\n");
    aboutText += tr("Designed by OriSoft Inc 2013.\n");
    aboutText += tr("Main idea and contribution by Olena Bublyk.\n");

    QMessageBox::information(this, QApplication::applicationName(), aboutText);
}

void MainWindow::onQuit()
{
    close();
}

void MainWindow::onMru(QObject* mruObj)
{
    QAction* mruAction = (QAction*)mruObj;
    generate(mruAction);
}

void MainWindow::onDataGenRequest(QWidget* dataWidget)
{
    int index = -1;
    DataWidget* dw = (DataWidget*)dataWidget;
    wstring category(dw->getCategory());

    obtainIndexByCategory(category, index);
    if(index >= 0)
    {
        depictDataByCategory(category, index);
        notifyMru();
        updateUI();
    }
}

void MainWindow::onDatabaseDataChanged()
{
    QString errDescr;
    if(!Logic::getInstance().makeDataStorage(m_ds, errDescr))
    {
        statusBar()->showMessage(errDescr, s_sbMsgTimeout);
    }
    else
    {
        statusBar()->showMessage(tr("Ready"), s_sbMsgTimeout);
    }

    updateUI();
}

void MainWindow::generate(QAction* mruAction)
{
    SequenceType seq;

    obtainSequence(seq, mruAction);
    depictData(seq);
    notifyMru(seq, mruAction);
    updateUI();
}

void MainWindow::updateUI()
{
    m_actionGenerate->setEnabled(!m_ds.empty());
    m_actionAdjust->setEnabled(Logic::getInstance().isDataEnabled());

    QList<QAction*> oldMruActions;
    QList<QAction*> cmdMenuActions = m_menuCommands->actions();
    QListIterator<QAction*> cmdMenuActionsIter(cmdMenuActions);

    if(cmdMenuActionsIter.findPrevious(m_actionQuit))
    {
        while(1)
        {
            if(cmdMenuActionsIter.next() == m_mruSep)
                break;

            oldMruActions << cmdMenuActionsIter.peekPrevious();
        }
    }

    for(QList<QAction*>::Iterator it = oldMruActions.begin(); it != oldMruActions.end(); it++)
    {
        m_menuCommands->removeAction(*it);
    }

    QList<QAction*> newMruActions;
    if(!m_mru.empty())
    {
        for(MruType::reverse_iterator it = m_mru.rbegin(); it != m_mru.rend(); it++)
        {
            QAction* mruAction = *it;
            if(!mruAction)
                continue;

            newMruActions << mruAction;
        }
    }

    if(!newMruActions.isEmpty())
    {
        QAction* quitSep = m_menuCommands->insertSeparator(m_actionQuit);
        m_menuCommands->insertActions(quitSep, newMruActions);
    }
}

void MainWindow::obtainSequence(SequenceType& seq, QAction* mruAction)
{
    if(mruAction)
    {
        MruType::iterator it = std::find(m_mru.begin(), m_mru.end(), mruAction);
        if(it != m_mru.end())
        {
            QVariant varData = mruAction->data();
            if(varData.canConvert<QVariantList>())
            {
                QVariantList dataList(varData.toList());
                foreach(QVariant varItem, dataList)
                {
                    int item = varItem.toInt();
                    seq.push_back(item);
                }
            }
        }
    }
    else
    {
        for(Data::iterator it = m_ds.begin(); it != m_ds.end(); it++)
        {
            MovementPresentationStorage& stor = it->second;
            int collSize = static_cast<int>(stor.coll.size());
            int num = Logic::genRandom(collSize);
            seq.push_back(num);
        }
    }
}

void MainWindow::obtainIndexByCategory(const wstring& category, int& index)
{
    Data::iterator it = find_if(m_ds.begin(), m_ds.end(), DataEntryPred(category));
    if(it != m_ds.end())
    {
        MovementPresentationStorage& stor = it->second;
        int collSize = static_cast<int>(stor.coll.size());
        index = Logic::genRandom(collSize);
    }
}

void MainWindow::depictData(const SequenceType& seq)
{
    m_table->clearContents();

    QFont itemFont(QApplication::font());
    itemFont.setPointSize(itemFont.pointSize() + 2);
    int i = 0;
    SequenceType::const_iterator seqIt = seq.begin();
    for(Data::iterator it = m_ds.begin(); ((it != m_ds.end()) && (i < s_rowCount)); it++, i++)
    {
        if(seqIt == seq.end())
            break;

        QTableWidgetItem* keyItem = new QTableWidgetItem(QString((QChar*)it->first.c_str(), static_cast<int>(it->first.size())));
        keyItem->setFlags(keyItem->flags() & (~Qt::ItemIsEditable));
        keyItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        keyItem->setFont(itemFont);
        m_table->setItem(i, 0, keyItem);

        wstring category(it->first.c_str(), it->first.size());
        DataWidget* dataWidget = new DataWidget(category, *seqIt, m_dataMapper, this);
        m_table->setCellWidget(i, 1, dataWidget);

        MovementPresentationStorage& stor = it->second;
        MovementPresentation* pres = stor.getPresentationBySequenceNumber(*seqIt);
        seqIt++;
        if(!pres)
            continue;

        if(pres->type == mptText)
        {
            dataWidget = (DataWidget*)m_table->cellWidget(i, 1);
            dataWidget->putText(pres->value.toString());
        }
    }

    adjustPresentation();
}

void MainWindow::depictDataByCategory(const wstring& category, int index)
{
    Data::iterator it = find_if(m_ds.begin(), m_ds.end(), DataEntryPred(category));
    if(it == m_ds.end())
        return;

    for(int i = 0; i < s_rowCount; i++)
    {
        QTableWidgetItem* keyItem = m_table->item(i, 0);
        wstring keyItemText(keyItem->text().toStdWString());

        if(keyItemText.compare(category) != 0)
            continue;

        MovementPresentationStorage& stor = it->second;
        MovementPresentation* pres = stor.getPresentationBySequenceNumber(index);
        if(!pres)
            break;

        if(pres->type == mptText)
        {
            DataWidget* dataWidget = (DataWidget*)m_table->cellWidget(i, 1);
            dataWidget->putText(pres->value.toString());
            dataWidget->putIndex(index);
        }

        break;
    }

    m_table->update();

    adjustPresentation();
}

void MainWindow::notifyMru(const SequenceType& seq, QAction* mruAction)
{
    if(mruAction)
    {
        MruType::iterator it = std::find(m_mru.begin(), m_mru.end(), mruAction);
        if(it != m_mru.end())
        {
            m_mru.erase(it);
            m_mru.push_back(mruAction);
        }
    }
    else
    {
        time_t curTime;
        time(&curTime);
        tm* localTime = localtime(&curTime);
        QString actionStr(QString(tr("%1")).arg(asctime(localTime)));

        mruAction = new QAction(this);
        mruAction->setText(actionStr);
        mruAction->setStatusTip(tr("MRU item."));

        QVariantList sequence;
        for(SequenceType::const_iterator itSeq = seq.begin(); itSeq != seq.end(); itSeq++)
        {
            sequence << QVariant(*itSeq);
        }

        QVariant data(sequence);
        mruAction->setData(data);

        connect(mruAction, SIGNAL(triggered()), m_mruMapper, SLOT(map()));
        m_mruMapper->setMapping(mruAction, (QObject*)mruAction);

        m_mru.push_back(mruAction);
    }

    if(m_mru.size() > s_mruMaxCount)
    {
        MruType::iterator itEldest = m_mru.begin();
        if(itEldest != m_mru.end())
        {
            QAction* eldestAction = *itEldest;
            m_mru.erase(itEldest);

            if(eldestAction)
            {
                eldestAction->disconnect();
                eldestAction->deleteLater();
            }
        }
    }
}

void MainWindow::notifyMru()
{
    time_t curTime;
    time(&curTime);
    tm* localTime = localtime(&curTime);
    QString actionStr(QString(tr("%1")).arg(asctime(localTime)));

    QAction* mruAction = new QAction(this);
    mruAction->setText(actionStr);
    mruAction->setStatusTip(tr("MRU item."));

    QVariantList sequence;
    for(int i = 0; i < s_rowCount; i++)
    {
        DataWidget* dataWidget = (DataWidget*)m_table->cellWidget(i, 1);
        int index = dataWidget->getIndex();
        sequence << QVariant(index);
    }

    QVariant data(sequence);
    mruAction->setData(data);

    connect(mruAction, SIGNAL(triggered()), m_mruMapper, SLOT(map()));
    m_mruMapper->setMapping(mruAction, (QObject*)mruAction);

    m_mru.push_back(mruAction);

    if(m_mru.size() > s_mruMaxCount)
    {
        MruType::iterator itEldest = m_mru.begin();
        if(itEldest != m_mru.end())
        {
            QAction* eldestAction = *itEldest;
            m_mru.erase(itEldest);

            if(eldestAction)
            {
                eldestAction->disconnect();
                eldestAction->deleteLater();
            }
        }
    }
}

void MainWindow::loadAppConfiguration()
{
    m_appConfiguration = new QSettings(tr("settings\\TribalGenSettings"),
                                       QSettings::IniFormat, this);

    bool maximized = m_appConfiguration->value(tr("geometry\\Maximized")).toBool();
    if(maximized)
    {
        showMaximized();
    }
    else
    {
        int left = m_appConfiguration->value(tr("geometry\\Left")).toInt();
        int top = m_appConfiguration->value(tr("geometry\\Top")).toInt();
        int width = m_appConfiguration->value(tr("geometry\\Width")).toInt();
        int height = m_appConfiguration->value(tr("geometry\\Height")).toInt();

        if(width && height)
            setGeometry(QRect(left, top, width, height));
    }

    loadMru();
}

void MainWindow::saveAppConfiguration()
{
    saveMru();

    bool maximized = isMaximized();
    m_appConfiguration->setValue(tr("geometry\\Maximized"), QVariant(maximized));

    if(!maximized)
    {
        QRect normalGeom(normalGeometry());

        m_appConfiguration->setValue(tr("geometry\\Left"), QVariant(normalGeom.left()));
        m_appConfiguration->setValue(tr("geometry\\Top"), QVariant(normalGeom.top()));
        m_appConfiguration->setValue(tr("geometry\\Width"), QVariant(normalGeom.width()));
        m_appConfiguration->setValue(tr("geometry\\Height"), QVariant(normalGeom.height()));
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    saveAppConfiguration();
    event->accept();
}

void MainWindow::loadMru()
{
    int mruCount = m_appConfiguration->beginReadArray(tr("MRU"));

    for(int i = 0; i < mruCount; i++)
    {
        QAction* mruAction = new QAction(this);

        m_appConfiguration->setArrayIndex(i);
        QString text = m_appConfiguration->value(tr("Name")).toString();

        mruAction->setText(text);
        mruAction->setStatusTip(tr("MRU item."));

        QVariantList sequence;
        int indexCount = m_appConfiguration->beginReadArray(tr("Indices"));
        for(int j = 0; j < indexCount; j++)
        {
            m_appConfiguration->setArrayIndex(j);
            QVariant index = m_appConfiguration->value(tr("Index"));
            sequence << index;
        }

        QVariant data(sequence);
        mruAction->setData(data);

        connect(mruAction, SIGNAL(triggered()), m_mruMapper, SLOT(map()));
        m_mruMapper->setMapping(mruAction, (QObject*)mruAction);

        m_mru.push_back(mruAction);

        m_appConfiguration->endArray();
    }

    m_appConfiguration->endArray();
}

void MainWindow::saveMru()
{
    m_appConfiguration->remove(tr("MRU"));

    m_appConfiguration->beginWriteArray(tr("MRU"));

    int i = 0;
    for(MruType::iterator it = m_mru.begin(); it != m_mru.end(); it++)
    {
        QAction* mruAction = *it;
        if(mruAction)
        {
            const QString& text = mruAction->text();
            const QVariant& data = mruAction->data();
            const QVariantList& sequence = data.toList();

            m_appConfiguration->setArrayIndex(i);
            m_appConfiguration->setValue(tr("Name"), QVariant(text));

            m_appConfiguration->beginWriteArray(tr("Indices"));

            int j = 0;
            for(QVariantList::const_iterator it = sequence.cbegin();
                it != sequence.cend(); it++)
            {
                const QVariant& index = *it;

                m_appConfiguration->setArrayIndex(j);
                m_appConfiguration->setValue(tr("Index"), index);

                ++j;
            }

            m_appConfiguration->endArray();
        }

        ++i;
    }

    m_appConfiguration->endArray();
}

void MainWindow::fillCategoriesInitially(const std::vector< std::wstring >& categories)
{
    QFont itemFont(QApplication::font());
    itemFont.setPointSize(itemFont.pointSize() + 2);
    int i = 0;
    for(std::vector< std::wstring >::const_iterator it = categories.begin(); ((it != categories.end()) && (i < s_rowCount)); it++, i++)
    {
        QTableWidgetItem* keyItem = new QTableWidgetItem(QString::fromStdWString(*it));
        keyItem->setFlags(keyItem->flags() & (~Qt::ItemIsEditable));
        keyItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        keyItem->setFont(itemFont);
        m_table->setItem(i, 0, keyItem);
    }

    m_table->setEditTriggers(QTableWidget::NoEditTriggers);

    adjustPresentation();
}

void MainWindow::resizeEvent(QResizeEvent*)
{
    adjustPresentation();
}



