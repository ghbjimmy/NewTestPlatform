#ifndef UUTBUTTON_H
#define UUTBUTTON_H

#include <QPushButton>

class QCheckBox;
class QLabel;

class UutButton : public QPushButton
{
    Q_OBJECT

public:
    explicit UutButton(int style, QWidget *parent = 0);
    virtual ~UutButton();

    void setIndex(int index);

    bool isChecked();
    void setChecked(bool flag);

    void showColor(const QColor& color);

signals:
    void signal_check(int);

protected:
    virtual void resizeEvent(QResizeEvent* evt);

private:
    void setupUI();

private slots:
    //void onStateChanged(int);

private:
    QCheckBox* _box;
    QLabel* _txtLbl;
    int _style;
    int _index;
};

#endif // SCOPEVIEWFORM_H
