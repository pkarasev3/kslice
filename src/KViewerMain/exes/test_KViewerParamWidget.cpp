#ifdef KVIEWER_LIB
    #undef KVIEWER_LIB
#endif

#include <QApplication>

#include "KViewerOptions.h"
#include "ParameterWidget.h"


int main(int argc, char** argv)
{
  QApplication app(argc,argv);

  auto opt = std::make_shared<KViewerOptions>();
  opt->setFromArgs(argc,argv);

  KViewerParameterWidget  paramWidget;
  paramWidget.populateFromOptions(opt);

  QCoreApplication::processEvents(QEventLoop::AllEvents,50);

  if (paramWidget.brushSizeSpinbox->value() != opt->paintBrushRad)
      return -1;
  
  return 0;
}

