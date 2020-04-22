#include "DataWidget.h"

using namespace std;

DataWidget::DataWidget(const wstring& category, int index,
                       QSignalMapper* mapper, QWidget* parent)
    : QWidget(parent, Qt::SubWindow), m_category(category), m_index(index),
      m_mapper(mapper), m_label(NULL), m_button(NULL)
{
    QHBoxLayout* layout = new QHBoxLayout(this);

    QFont textFont(QApplication::font());
    textFont.setPointSize(textFont.pointSize() + 2);

    m_button = new QPushButton(tr("More"), this);

    connect(m_button, SIGNAL(clicked()), m_mapper, SLOT(map()));
    m_mapper->setMapping(m_button, this);

    layout->addWidget(m_button);

    m_label = new QLabel(this);
    m_label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_label->setFont(textFont);

    layout->addWidget(m_label, 5);

    setLayout(layout);
    show();
}

DataWidget::~DataWidget()
{

}

void DataWidget::putText(const QString& text)
{
    m_label->setText(text);
}

const wstring& DataWidget::getCategory() const
{
    return m_category;
}

int DataWidget::getIndex() const
{
    return m_index;
}

void DataWidget::putIndex(int index)
{
    m_index = index;
}
