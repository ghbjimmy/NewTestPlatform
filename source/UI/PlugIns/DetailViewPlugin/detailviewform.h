#ifndef DETAILVIEWFORM1_H
#define DETAILVIEWFORM1_H

#include <QWidget>
#include <QTabWidget>


class DetailViewForm : public QWidget
{
    Q_OBJECT

public:
    explicit DetailViewForm(QWidget *parent = 0);
    ~DetailViewForm();

private:
    void SetupUI();
private:
    QTabWidget* mTabWidget;
};

#endif // DETAILVIEWFORM_H
