#ifndef DETAILVIEWFORM1_H
#define DETAILVIEWFORM1_H

#include <QWidget>
#include <QTabWidget>
#include <QVector>

class CVSDataTreeView;
class ProgressForm;
class FailForm;
class TDetailViewItem;

class DetailViewForm : public QTabWidget
{
    Q_OBJECT

public:
    explicit DetailViewForm(QWidget *parent = 0);
    ~DetailViewForm();

    bool listCsvData(const QVector<QString>& datas);
    bool procItemStart(int index, const QString& data);
    bool procItemEnd(int index, const QString& data);

private:
    void setupUI();

private:
    CVSDataTreeView* _csvTreeView;
    ProgressForm* _progressForm;
    FailForm* _failForm;

    QVector<TDetailViewItem*> _vieItems;
};

#endif // DETAILVIEWFORM_H
