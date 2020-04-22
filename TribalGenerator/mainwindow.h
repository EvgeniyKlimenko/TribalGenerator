#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <deque>
#include <vector>
#include <string>

#include "Logic.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    typedef std::deque<QAction*> MruType;

public:
    typedef std::vector<int> SequenceType;

public:
    MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

signals:
    void mruTriggered(QObject* mruObj);
    void dataTriggered(QWidget* dataWidget);

private slots:
    void onGenerate();
    void onAdjust();
    void onQuit();
    void onAbout();
    void onMru(QObject* mruObj);
    void onDataGenRequest(QWidget* dataWidget);
    void onDatabaseDataChanged();

private:
    void createActions();
    void createMenu();
    void createToolbar();
    void createMru();
    void createPresentation();
    void adjustWindowSize();
    void adjustPresentation();
    void updateUI();
    void obtainSequence(SequenceType& seq, QAction* mruAction);
    void obtainIndexByCategory(const std::wstring& category, int& index);
    void depictData(const SequenceType& seq);
    void depictDataByCategory(const std::wstring& category, int index);
    void notifyMru(const SequenceType& seq, QAction* mruAction);
    void notifyMru();
    void generate(QAction* mruAction = NULL);
    void loadAppConfiguration();
    void saveAppConfiguration();
    void closeEvent(QCloseEvent* event);
    void loadMru();
    void saveMru();
    void fillCategoriesInitially(const std::vector< std::wstring >& categories);

    virtual void resizeEvent(QResizeEvent*);

private:
    static const int s_rowCount = 5;
    static const int s_colCount = 2;
    static const int s_sbMsgTimeout = 3000;
    static const int s_mruMaxCount = 10;
    QTableWidget* m_table;
    QAction* m_actionGenerate;
    QAction* m_actionAdjust;
    QAction* m_actionQuit;
    QAction* m_actionAbout;
    QToolBar* m_toolbar;
    QMenu* m_menuCommands;
    QSignalMapper* m_mruMapper;
    Data m_ds;
    MruType m_mru;
    QAction* m_mruSep;
    QSignalMapper* m_dataMapper;
    QSettings* m_appConfiguration;
};

#endif // MAINWINDOW_H
