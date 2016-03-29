#ifndef SCOPEVIEWFORM_H
#define SCOPEVIEWFORM_H

#include <QScrollArea>
#include <QVector>
#include <QGraphicsView>

#include "plugin_global.h"

class IPlugin;
class QGroupBox;
class QAction;

class ScopeviewForm : public IModuleForm
{
    Q_OBJECT
public:
    explicit ScopeviewForm(IPlugin* plugIn, QWidget *parent = 0);
    ~ScopeviewForm();

    virtual bool init();
    virtual void clear();
    virtual QVector<QAction*> getActions();

protected:
    virtual void resizeEvent(QResizeEvent* evt);

private slots:
    void onShowForm();

private:
    void loadData();

    void setupUI();
    void updateSceneRect();
    void updateSceneItemPos();

private slots:
    void onClicked();

private:
    QAction* _showFormAct;
    QGraphicsView* _graphView;
    QVector<QGroupBox*> _groupboxs;
};

#endif // SCOPEVIEWFORM_H
