#ifndef PROGRESSFORM_H
#define PROGRESSFORM_H

#include <QScrollArea>
#include <QProgressBar>


class ProgressForm : public QScrollArea
{
    Q_OBJECT

public:
    explicit ProgressForm(QWidget *parent = 0);
    ~ProgressForm();

private:
    void setupUI();

private:
    QProgressBar* _bar1;
    QProgressBar* _bar2;
    QProgressBar* _bar3;
    QProgressBar* _bar4;
    QProgressBar* _bar5;
    QProgressBar* _bar6;
};

#endif // DETAILVIEWFORM_H
