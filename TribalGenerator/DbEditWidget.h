#ifndef DBEDITWIDGET_H
#define DBEDITWIDGET_H

#include <QtWidgets>

class DbEditWidget : public QWidget
{
    Q_OBJECT

public:
    DbEditWidget(QWidget* parent);
    virtual ~DbEditWidget();

    void putIndex(int index);
    int getIndex();
    void putText(const QString& text);
    QString getText();

signals:
    void changesSaved();
    void changesDiscarded();
    void widgetActivated(bool);

private slots:
    void onTimeout();
    void onTextChanged();
    void onSubmit();
    void onDiscard();

private:
    virtual void showEvent(QShowEvent*);
    virtual void hideEvent(QHideEvent*);

private:
    bool m_origText;
    int m_index;
    QTextEdit* m_edit;
    QPushButton* m_submit;
    QPushButton* m_discard;
    QTimer* m_timer;
};

#endif // DBEDITWIDGET_H
