#ifndef DETAILVIEWFORM1_H
#define DETAILVIEWFORM1_H

#include <QWidget>
#include <QTabWidget>
#include <QVector>

class CVSDataTreeView;
class DetailViewForm : public QTabWidget
{
    Q_OBJECT

public:
    explicit DetailViewForm(QWidget *parent = 0);
    ~DetailViewForm();

    void listCsvData(const QVector<QString>& datas);

private:
    void setupUI();

private:

    CVSDataTreeView* _csvTreeView;
};

#endif // DETAILVIEWFORM_H
