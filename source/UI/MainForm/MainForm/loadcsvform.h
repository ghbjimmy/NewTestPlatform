#ifndef LOADCSVFORM_H
#define LOADCSVFORM_H

#include <QDialog>

class QProgressBar;
class QTextEdit;

class LoadCsvForm : public QDialog
{
    Q_OBJECT
public:
    LoadCsvForm(QWidget *parent = 0);
    ~LoadCsvForm();

    void setProgressMaxSize(int maxSize);

public slots:
    void onAppendText(const QString& text, int state);
    void onClose();

private:
    void setupUI();

private:
    QProgressBar* _bar;
    QTextEdit* _textEdit;
};

#endif // LOADCSVFORM_H
