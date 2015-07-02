#ifdef KVIEWER_LIB
    #undef KVIEWER_LIB
#endif

#include <QApplication>

#include "KViewer.h"
#include "KViewerOptions.h"

using namespace vrcl;

int main(int argc, char** argv)
{


  KViewerOptions  kview_opt;
  kview_opt.setFromArgs(argc,argv);

  QApplication app(argc, argv);

  KViewer* widget=new KViewer( kview_opt );

  widget->show();
  return app.exec();
}

