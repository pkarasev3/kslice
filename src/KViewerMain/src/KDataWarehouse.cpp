#include "KDataWarehouse.h"
#include "Logger.h"
#include "KViewerOptions.h"
#include "vtkImageData.h"

using namespace vrcl;
//using cv::Ptr;

KDataWarehouse::KDataWarehouse( std::shared_ptr<KViewerOptions> kv_opts_in ) {
  kv_opts        = kv_opts_in;
}

void KDataWarehouse::PushLabelDataNewToCurrent() {
    double range[2];
    labelDataArray->GetScalarRange( range );
    cout << "range before update: " << range[0] << ", " << range[1] << endl;
    labelDataArray->DeepCopy( labelDataArray_new );
    labelDataArray->GetScalarRange( range );
    cout << "range after update: " << range[0] << ", " << range[1] << endl;

  }


