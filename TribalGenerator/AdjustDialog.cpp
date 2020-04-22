#include "AdjustDialog.h"
#include "DbControlWidget.h"

AdjustDialog::AdjustDialog(QSettings* appConfiguration, QWidget* parent)
    : QDialog(parent, Qt::Dialog | Qt::WindowMaximizeButtonHint), m_cookie(0),
      m_tabHolder(NULL), m_btnSubmit(NULL), m_btnDecline(NULL),
      m_appConfiguration(appConfiguration)
{
    QVBoxLayout* dlgLayout = new QVBoxLayout(this);

    m_tabHolder = new QTabWidget(this);
    dlgLayout->addWidget(m_tabHolder, 5);

    DbControlWidget* dbControl = new DbControlWidget(m_tabHolder);
    m_tabHolder->addTab(dbControl, QIcon(":/icons/database.png"), tr("Database"));

    m_dbMgr.addObserver(dbControl, m_cookie);
    dbControl->setDbManagerReference(&m_dbMgr);
    m_dbMgr.fetchData();

    m_btnSubmit = new QPushButton(tr("Submit"), this);
    m_btnDecline = new QPushButton(tr("Decline"), this);

    QGridLayout* btnLayout = new QGridLayout(this);
    btnLayout->addWidget(m_btnSubmit, 0, 0, Qt::AlignHCenter | Qt::AlignVCenter);
    btnLayout->addWidget(m_btnDecline, 0, 1, Qt::AlignHCenter | Qt::AlignVCenter);

    dlgLayout->addLayout(btnLayout);

    QRect geometry(QApplication::desktop()->screenGeometry());
    QSize minSize(geometry.width()/2, geometry.height()/2);
    resize(minSize);

    setMinimumSize(minSize);

    setLayout(dlgLayout);

    connect(m_btnSubmit, SIGNAL(clicked()), this, SLOT(onSubmit()));
    connect(m_btnDecline, SIGNAL(clicked()), this, SLOT(onDecline()));

    loadDlgConfiguration();
}

AdjustDialog::~AdjustDialog()
{
    m_dbMgr.removeObserver(m_cookie);
}

void AdjustDialog::onSubmit()
{
    bool dbModified = m_dbMgr.isDbModified();
    bool submitted = m_dbMgr.submitChanges();
    if(dbModified && submitted)
    {
        emit databaseDataChanged();
    }

    close();
}

void AdjustDialog::onDecline()
{
    m_dbMgr.declineChanges();
    close();
}

void AdjustDialog::loadDlgConfiguration()
{
    bool maximized = m_appConfiguration->value(tr("AdjustDlg\\geometry\\Maximized")).toBool();
    if(maximized)
    {
        showMaximized();
    }
    else
    {
        int left = m_appConfiguration->value(tr("AdjustDlg\\geometry\\Left")).toInt();
        int top = m_appConfiguration->value(tr("AdjustDlg\\geometry\\Top")).toInt();
        int width = m_appConfiguration->value(tr("AdjustDlg\\geometry\\Width")).toInt();
        int height = m_appConfiguration->value(tr("AdjustDlg\\geometry\\Height")).toInt();

        if(width && height)
            setGeometry(QRect(left, top, width, height));
    }
}

void AdjustDialog::saveDlgConfiguration()
{
    bool maximized = isMaximized();
    m_appConfiguration->setValue(tr("AdjustDlg\\geometry\\Maximized"), QVariant(maximized));

    if(!maximized)
    {
        QRect normalGeom(normalGeometry());

        m_appConfiguration->setValue(tr("AdjustDlg\\geometry\\Left"), QVariant(normalGeom.left()));
        m_appConfiguration->setValue(tr("AdjustDlg\\geometry\\Top"), QVariant(normalGeom.top()));
        m_appConfiguration->setValue(tr("AdjustDlg\\geometry\\Width"), QVariant(normalGeom.width()));
        m_appConfiguration->setValue(tr("AdjustDlg\\geometry\\Height"), QVariant(normalGeom.height()));
    }
}

void AdjustDialog::closeEvent(QCloseEvent* event)
{
    saveDlgConfiguration();
    event->accept();
}


