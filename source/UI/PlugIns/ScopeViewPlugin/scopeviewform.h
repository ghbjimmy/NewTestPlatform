#ifndef SCOPEVIEWFORM_H
#define SCOPEVIEWFORM_H

#include <QScrollArea>
#include <QVector>
#include <QGraphicsView>
class IPlugin;

class QGroupBox;
class ScopeviewForm : public QGraphicsView
{
    Q_OBJECT
public:
    explicit ScopeviewForm(IPlugin* plugIn, QWidget *parent = 0);
    ~ScopeviewForm();

    void loadData();
protected:
    virtual void resizeEvent(QResizeEvent* evt);

private:
    void setupUI();
    void updateSceneRect();
    void updateSceneItemPos();

private slots:
    void onBtnCheckBoxStatedChanged(int state);
    void onClicked();
private:
    IPlugin* _plugIn;

    QVector<QGroupBox*> _groupboxs;
};

#endif // SCOPEVIEWFORM_H
