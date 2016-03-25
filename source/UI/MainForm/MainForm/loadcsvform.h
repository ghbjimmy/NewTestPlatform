#ifndef LOADCSVFORM_H
#define LOADCSVFORM_H

#include <QDialog>

class QProgressBar;
class QListWidget;

class LoadCsvForm : public QDialog
{
    Q_OBJECT
public:
    LoadCsvForm(QWidget *parent = 0);
    ~LoadCsvForm();

private:
    void setupUI();

private:
    QProgressBar* _bar;
    QListWidget* _listWgt;
};

#endif // LOADCSVFORM_H
