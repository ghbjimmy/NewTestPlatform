#ifndef DETAILVIEWFORM_H
#define DETAILVIEWFORM_H

#include <QWidget>
#include <QTabWidget>


class DetailViewForm : public QWidget
{
    Q_OBJECT

public:
    explicit DetailViewForm(QWidget *parent = 0);
    ~DetailViewForm();

private:
    void SetUpUi();
private:
    QTabWidget* mTabWidget;
};

#endif // DETAILVIEWFORM_H
