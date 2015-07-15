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
    std::function<void(bool,bool,bool)> m_viewCallback;
    std::function<void(int,int,int)>    m_startAutoSegCallback;
    std::function<void()>               m_stopAutoSegCallback;

public slots:
    void  updatedBasicParams();
    void  updatedViewSelection();
    void  startAutoShiftAndSegment();
    void  stopAutoShiftAndSegment();

public:
    //KViewerParameterWidget& populateFromViewXYZ(int x,int y, int z);
    KViewerParameterWidget& populateFromOptions(std::shared_ptr<KViewerOptions> opts);
    KViewerParameterWidget& setOptionsUpdateCallback(decltype(m_updateCallback) arg){m_updateCallback=arg; return *this;}
    KViewerParameterWidget& setViewDirUpdateCallback(decltype(m_viewCallback) arg){  m_viewCallback  =arg; return *this; }
    KViewerParameterWidget& setStartAutoSegCallback(decltype(m_startAutoSegCallback) arg){  m_startAutoSegCallback = arg; return *this; }
    KViewerParameterWidget& setStopAutoSegCallback(decltype(m_stopAutoSegCallback) arg){  m_stopAutoSegCallback  = arg; return *this; }

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

inline bool isWindows()
{
#ifndef _WIN32
    return false;
#else
    return true;
#endif
}

template <class W> W&  AlwaysOnTop(W&  _window, bool bForceOnTop_win32=true)
{
    auto w_flags =   Qt::WindowStaysOnTopHint ;
    bool bOn     = bForceOnTop_win32 || (!isWindows());
    if(bOn)
        _window.setWindowFlags(_window.windowFlags() | w_flags);
    _window.setAttribute(Qt::WA_DeleteOnClose,true);
    return _window;
}
