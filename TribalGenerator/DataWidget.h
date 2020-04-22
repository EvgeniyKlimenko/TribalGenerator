#ifndef DATAWIDGET_H
#define DATAWIDGET_H

#include <QtWidgets>
#include <string>

class DataWidget : public QWidget
{
    Q_OBJECT

public:
    DataWidget(const std::wstring& category, int index,
               QSignalMapper* mapper, QWidget* parent);
    virtual ~DataWidget();

    void putText(const QString& text);
    const std::wstring& getCategory() const;
    int getIndex() const;
    void putIndex(int index);

private:
    std::wstring m_category;
    int m_index;
    QSignalMapper* m_mapper;
    QLabel* m_label;
    QPushButton* m_button;
};

#endif // DATAWIDGET_H
