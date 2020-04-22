#include "DbEditWidget.h"

DbEditWidget::DbEditWidget(QWidget* parent)
    : QWidget(parent), m_origText(false), m_index(-1), m_edit(NULL),
      m_submit(NULL), m_discard(NULL), m_timer(NULL)
{
    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    m_timer->setSingleShot(false);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    m_edit = new QTextEdit(this);
    mainLayout->addWidget(m_edit, 5);

    QGridLayout* btnLayout = new QGridLayout(this);

    m_submit = new QPushButton(tr("OK"), this);
    btnLayout->addWidget(m_submit, 0, 0, Qt::AlignHCenter | Qt::AlignVCenter);

    m_discard = new QPushButton(tr("Cancel"), this);
    btnLayout->addWidget(m_discard, 0, 1, Qt::AlignHCenter | Qt::AlignVCenter);

    mainLayout->addLayout(btnLayout);

    setLayout(mainLayout);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    connect(m_edit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
    connect(m_submit, SIGNAL(clicked()), this, SLOT(onSubmit()));
    connect(m_discard, SIGNAL(clicked()), this, SLOT(onDiscard()));

    m_timer->start(500);
}

DbEditWidget::~DbEditWidget()
{
    m_timer->stop();
}

void DbEditWidget::putIndex(int index)
{
    m_index = index;
}

int DbEditWidget::getIndex()
{
    return m_index;
}

void DbEditWidget::putText(const QString& text)
{
    m_edit->setPlainText(text);
    m_origText = true;
}

QString DbEditWidget::getText()
{
    return m_edit->toPlainText();
}

void DbEditWidget::onTimeout()
{
    bool enabled = (!m_edit->toPlainText().size()) ? false : !m_origText;
    m_submit->setEnabled(enabled);
}

void DbEditWidget::onTextChanged()
{
   m_origText = false;
}

void DbEditWidget::onSubmit()
{
    emit changesSaved();
}

void DbEditWidget::onDiscard()
{
    emit changesDiscarded();
}

void DbEditWidget::showEvent(QShowEvent*)
{
    emit widgetActivated(true);
}

void DbEditWidget::hideEvent(QHideEvent*)
{
    emit widgetActivated(false);
}
