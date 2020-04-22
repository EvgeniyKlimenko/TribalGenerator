#ifndef ADJUSTDIALOG_H
#define ADJUSTDIALOG_H

#include <QtWidgets>
#include "DbManager.h"

class AdjustDialog : public QDialog
{
    Q_OBJECT

public:
    AdjustDialog(QSettings* appConfiguration, QWidget* parent);
    virtual ~AdjustDialog();

private slots:
    void onSubmit();
    void onDecline();

private:
    void loadDlgConfiguration();
    void saveDlgConfiguration();

    void closeEvent(QCloseEvent* event);

signals:
    void databaseDataChanged();

private:
    int m_cookie;
    DbManager m_dbMgr;
    QTabWidget* m_tabHolder;
    QPushButton* m_btnSubmit;
    QPushButton* m_btnDecline;
    QSettings* m_appConfiguration;
};

#endif // ADJUSTDIALOG_H
