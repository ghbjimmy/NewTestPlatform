#include "loadcsvform.h"
#include "util.h"
#include <QProgressBar>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QMessageBox>


LoadCsvForm::LoadCsvForm(QWidget *parent) : QDialog(parent)
{
    setupUI();
    //this->setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowTitle("Loading File");

    this->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);

    UIUtil::moveToScreenCenter(this);
}

LoadCsvForm::~LoadCsvForm()
{

}

void LoadCsvForm::setupUI()
{
    QLabel* titleLbl = new QLabel("Loading default CSV File : xxxxxx.csv");
    UIUtil::setFontSize(titleLbl, -1, true);
    UIUtil::setTextColor(titleLbl, Qt::blue);
    _bar = new QProgressBar();

    _textEdit = new QTextEdit();
    _textEdit->setReadOnly(true);

    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addWidget(titleLbl);
    v1->addWidget(_bar);
    v1->addWidget(_textEdit, 1);
    this->setLayout(v1);

    this->setFixedSize(480, 200);
}

void LoadCsvForm::setProgressMaxSize(int maxSize)
{
    _bar->setRange(0, maxSize);
}

void LoadCsvForm::onAppendText(const QString& text, int state)
{
    QString colorText;
    if (state == 0)
        colorText = QString("<font color=black>%1</font>\n").arg(text);
    else if (state == 1)
        colorText = QString("<font color=blue>%1</font>\n").arg(text);
    else
        colorText = QString("<font color=red>%1</font>\n").arg(text);

    if (state != 0)
    {
        int curVal = _bar->value();
        if (curVal == -1)
            curVal = 0;

        if (curVal <= _bar->maximum())
            _bar->setValue(curVal + 1);
    }

    _textEdit->append(colorText);
}

void LoadCsvForm::onClose()
{
    //QMessageBox::show();
    this->close();
}

