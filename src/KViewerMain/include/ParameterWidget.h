#pragma once

#include  <QObject>
#include  "kv_export.h"
#include  "ui_ParameterWidget.h"
#include <memory>
#include <functional>
#include <vtkWeakPointer.h>

struct KViewerOptions;
class vtkLookupTable;

class KVIEWER_EXPORT  KViewerParameterWidget : public QObject, public Ui_KViewerParameterWidget
{
    Q_OBJECT

    std::function<void(std::shared_ptr<KViewerOptions>)> m_updateCallback;

public slots:
    void  updatedBasicParams();

public:    
    KViewerParameterWidget& populateFromOptions(std::shared_ptr<KViewerOptions> opts);
    KViewerParameterWidget& setOptionsUpdateCallback(decltype(m_updateCallback) arg){m_updateCallback=arg; return *this;}
    
public:
    KViewerParameterWidget();
   ~KViewerParameterWidget();

private:
    std::unique_ptr<QDialog>  m_dialog;
    std::weak_ptr<KViewerOptions> m_kvopts;
    vtkWeakPointer<vtkLookupTable>    m_satLUT;
public:
    KViewerParameterWidget& setSaturationLUT(decltype(m_satLUT) arg);
    
};