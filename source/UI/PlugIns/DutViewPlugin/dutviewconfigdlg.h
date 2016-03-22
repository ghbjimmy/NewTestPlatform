#ifndef DUTVIEWCONFIGDLG_H
#define DUTVIEWCONFIGDLG_H

#include <QDialog>

class QTableWidget;
class QDomDocument;

struct TSlotConfig
{
    int index;
    QString reqIp;
    int reqPort;
    QString subIp;
    int subPort;
};

class DutViewConfigDlg : public QDialog
{
    Q_OBJECT

public:
    explicit DutViewConfigDlg(QWidget *parent = 0);
    ~DutViewConfigDlg();

    void setData(const QVector<TSlotConfig>& cfgs);

    const QVector<TSlotConfig>& getData() const {return _slotCfgs;}

    bool loadData(const QString& path);

private:
    void setupUI();

private slots:
    void onOK();
    void onCancel();

private:
    QVector<TSlotConfig> _slotCfgs;
    QTableWidget* _cfgTbl;
    QDomDocument* _dom;
};

#endif // DUTVIEWCONFIGDLG_H
