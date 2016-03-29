#ifndef SCOPEVIEWFORM_H
#define SCOPEVIEWFORM_H

#include <QScrollArea>
#include <QVector>
#include <QGraphicsView>

#include "plugin_global.h"

class IPlugin;
class QGroupBox;
class ScopeviewForm : public IModuleForm
{
    Q_OBJECT
public:
    explicit ScopeviewForm(IPlugin* plugIn, QWidget *parent = 0);
    ~ScopeviewForm();

    virtual bool init();
    virtual void clear();
    virtual QVector<QAction*> getActions();

    void loadData();

protected:
    virtual void resizeEvent(QResizeEvent* evt);

private:
    void setupUI();
    void updateSceneRect();
    void updateSceneItemPos();

private slots:
    void onClicked();

private:
    QGraphicsView* _graphView;
    QVector<QGroupBox*> _groupboxs;
};

#endif // SCOPEVIEWFORM_H
