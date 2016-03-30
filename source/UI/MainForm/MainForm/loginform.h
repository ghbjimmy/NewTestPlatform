#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QDialog>

class QLineEdit;
class QPushButton;

class UserCtrl;

class LoginForm : public QDialog
{
    Q_OBJECT

public:
    LoginForm(UserCtrl* userctrl, QWidget* parent=0);
    ~LoginForm();

private:
    void setupUI();

signals:
    void userPrivilSignal(const QMap<QString, int>& widgetPrivils);

private slots:
    void onOK();
    void onCancel();

private:
    UserCtrl* _userCtrl;
    QLineEdit* _userLdt;
    QLineEdit* _pwdLdt;
    QPushButton* _okBtn;
    QPushButton* _cancelBtn;
};

#endif // LOGINFORM_H
