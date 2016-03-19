#ifndef DETAILVIEWFORM1_H
#define DETAILVIEWFORM1_H

#include <QTabWidget>
#include <QVector>

class CVSDataTreeView;
class ProgressForm;
class FailForm;
class TDetailViewItem;
class IPlugin;

class DetailViewForm : public QTabWidget
{
    Q_OBJECT

public:
    explicit DetailViewForm(IPlugin* plugIn, QWidget *parent = 0);
    ~DetailViewForm();

    bool listCsvData(const QVector<QString>& datas);
    bool procItemStart(int index, const QString& data);
    bool procItemEnd(int index, const QString& data);

private:
    void setupUI();

private:
    IPlugin* _plugIn;
    CVSDataTreeView* _csvTreeView;
    ProgressForm* _progressForm;
    FailForm* _failForm;

    QVector<TDetailViewItem*> _vieItems;
};

#endif // DETAILVIEWFORM_H
