#ifndef CONFIGFORM_H
#define CONFIGFORM_H

#include <QDialog>

class QBoxLayout;
class QCheckBox;
class QLineEdit;
class QRadioButton;
class QPushButton;
class QComboBox;

class ConfigForm : public QDialog
{
    Q_OBJECT
public:
    ConfigForm(QWidget *parent = 0);
    ~ConfigForm();

private:
    void setupUI();
    QBoxLayout* createApplLyt();
    QBoxLayout* createTrigLyt();
    QWidget* createGeneralTab();

    QBoxLayout* createSNFormatLyt();
    QBoxLayout* createPubBLogLyt();
    QWidget* createAdvanceTab();
private:
    QCheckBox* _scanBarCodeCb;
    QCheckBox* _checkEEEECb;
    QCheckBox* _scanCfgCb;
    QCheckBox* _puddingPdcaCb;
    QCheckBox* _queryResuCb;
    QLineEdit* _resultLdt;

    QRadioButton* _manualRbtn;
    QRadioButton* _extRbtn;
    QRadioButton* _bothRbtn;
    QRadioButton* _scanRbtn;
    QLineEdit* _startLdt;
    QCheckBox* _autoModeCb;

    QLineEdit* _fixtureLdt;
    QLineEdit* _fileEdit;
    QPushButton* _broweBtn;
    QPushButton* _okBtn;
    QPushButton* _cancelBtn;

    QLineEdit* _stopValLdt;
    QLineEdit* _maxResetLdt;

    QLineEdit* _mlbLdt;
    QLineEdit* _cfgLdt;

    QComboBox* _puddingCbx;
    QCheckBox* _logCb;
    QCheckBox* _flowCb;
    QCheckBox* _removeCb;
};

#endif // CONFIGFORM_H
