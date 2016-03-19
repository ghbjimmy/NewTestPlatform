#ifndef PROGRESSFORM_H
#define PROGRESSFORM_H

#include <QScrollArea>
#include "const.h"

class QProgressBar;
class ProgressForm : public QScrollArea
{
    Q_OBJECT

public:
    explicit ProgressForm(QWidget *parent = 0);
    ~ProgressForm();

    void setBarMaxSize(int size);

    void setBarValue(int index, int value);
    void increaseBarValue(int index);

private:
    void setupUI();

private:
    QProgressBar* _bar[SEQ_NUM];
};

#endif // DETAILVIEWFORM_H
