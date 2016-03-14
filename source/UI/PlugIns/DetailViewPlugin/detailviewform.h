#ifndef DETAILVIEWFORM1_H
#define DETAILVIEWFORM1_H

#include <QWidget>
#include <QTabWidget>

class CVSDataTreeView;
class DetailViewForm : public QTabWidget
{
    Q_OBJECT

public:
    explicit DetailViewForm(QWidget *parent = 0);
    ~DetailViewForm();

    void loadCsvData();

private:
    void setupUI();

private:

    CVSDataTreeView* _csvTreeView;
};

#endif // DETAILVIEWFORM_H
