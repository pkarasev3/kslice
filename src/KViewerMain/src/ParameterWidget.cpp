#include "ParameterWidget.h"
#include "KViewer.h"
#include "KViewerOptions.h"
#include <QDoubleSpinBox>
#include <vtkLookupTable.h>


KViewerParameterWidget::KViewerParameterWidget()
{
    m_dialog.reset(new QDialog(nullptr) ); // = std::make_unique<QDialog>(nullptr);
    setupUi(m_dialog.get());
    QPoint current_loc = QCursor::pos();
    m_dialog->setGeometry(30,400,m_dialog->width(),m_dialog->height());
    m_dialog->show();
    AlwaysOnTop(*m_dialog);

    setOptionsUpdateCallback([](std::shared_ptr<KViewerOptions>){});
    setViewDirUpdateCallback([](bool,bool,bool){ });
    setStartAutoSegCallback( [](int,int,int){ } );
    setStopAutoSegCallback(  [](){ } );

    //! @note{ valid ranges + stepsizes are defined via .ui file }
    auto bOK = QList<bool>()
    <<connect(this->brushSizeSpinbox,SIGNAL(valueChanged(int)),this,SLOT(updatedBasicParams()),Qt::QueuedConnection)
    <<connect(this->labelOpacitySpinbox,SIGNAL(valueChanged(double)),this,SLOT(updatedBasicParams()), Qt::QueuedConnection)
    <<connect(this->satLUT_MinSpinbox,SIGNAL(valueChanged(int)),this,SLOT(updatedBasicParams()), Qt::QueuedConnection)
    <<connect(this->satLUT_MaxSpinbox,SIGNAL(valueChanged(int)),this,SLOT(updatedBasicParams()), Qt::QueuedConnection)
    <<connect(this->lambdaSmoothnessSpinbox,SIGNAL(valueChanged(double)),this,SLOT(updatedBasicParams()), Qt::QueuedConnection)
    <<connect(this->autoTriggerContourSpinbox,SIGNAL(valueChanged(double)),this,SLOT(updatedBasicParams()), Qt::QueuedConnection)
    <<connect(this->regionSizeRBACSpinBox,SIGNAL(valueChanged(int)),this,SLOT(updatedBasicParams()), Qt::QueuedConnection)
    <<connect(this->contourIterationsSpinBox,SIGNAL(valueChanged(int)),this,SLOT(updatedBasicParams()), Qt::QueuedConnection)
    <<connect(this->checkBoxCopyPasteAllLabels,SIGNAL(stateChanged(int)),this,SLOT(updatedBasicParams()), Qt::QueuedConnection)
    <<connect(this->checkBoxEvolveAllLabels,SIGNAL(stateChanged(int)),this,SLOT(updatedBasicParams()), Qt::QueuedConnection)
    <<connect(this->checkBoxEnableAutoTriggerSegmentor,SIGNAL(stateChanged(int)),this,SLOT(updatedBasicParams()), Qt::QueuedConnection)
    <<connect(this->checkBoxPasteAsMax, SIGNAL(stateChanged(int)), this, SLOT(updatedBasicParams()), Qt::QueuedConnection)

    <<connect(this->radioButton_View0, SIGNAL(toggled(bool)), this, SLOT(updatedViewSelection()))
    <<connect(this->radioButton_View1, SIGNAL(toggled(bool)), this, SLOT(updatedViewSelection()))
    <<connect(this->radioButton_View2, SIGNAL(toggled(bool)), this, SLOT(updatedViewSelection()))

    <<connect(this->pushButton_AutoSegGoLeft,  SIGNAL(pressed()), this, SLOT(startAutoShiftAndSegment()))
    <<connect(this->pushButton_AutoSegGoRight, SIGNAL(pressed()), this, SLOT(startAutoShiftAndSegment()))
    <<connect(this->pushButton_AutoSegStop,    SIGNAL(pressed()), this, SLOT(stopAutoShiftAndSegment()))

    /**/; Q_ASSERT(!bOK.contains(false));
}

KViewerParameterWidget::~KViewerParameterWidget()
{
}

KViewerParameterWidget& KViewerParameterWidget::setSaturationLUT(vtkWeakPointer<vtkLookupTable> lut)
{
    m_satLUT = lut;
    if (!m_satLUT)
        return *this;

    double* lut_range;
    lut_range = m_satLUT->GetRange();
    satLUT_MinSpinbox->setValue(lut_range[0]);
    satLUT_MaxSpinbox->setValue(lut_range[1]);

    this->m_dialog->update();
    return *this;
}

KViewerParameterWidget& KViewerParameterWidget::populateFromOptions(std::shared_ptr<KViewerOptions> opts_in)
{
    auto current_opts = m_kvopts.lock();
    if (current_opts.get( ) != opts_in.get( ))
        this->m_kvopts = opts_in;

    current_opts = m_kvopts.lock();
    if (!current_opts)
        return *this;

    auto& opts = *opts_in;

    this->brushSizeSpinbox->setValue( opts.paintBrushRad );
    if (!m_satLUT) {
        this->satLUT_MinSpinbox->setValue(0);
        this->satLUT_MaxSpinbox->setValue(1e4);
    } else
        setSaturationLUT(m_satLUT);

    this->lambdaSmoothnessSpinbox->setValue(opts.lambda);
    this->autoTriggerContourSpinbox->setValue(opts.seg_time_interval);
    this->labelOpacitySpinbox->setValue(opts.labelOpacity2D);
    this->autoTriggerContourSpinbox->setValue(opts.seg_time_interval);

    this->regionSizeRBACSpinBox->setValue(opts.rad);
    this->contourIterationsSpinBox->setValue(opts.segmentor_iters);

    this->checkBoxCopyPasteAllLabels->setChecked(opts.multilabel_paste_mode);
    this->checkBoxEvolveAllLabels->setChecked(opts.multilabel_sgmnt_mode);
    this->checkBoxEnableAutoTriggerSegmentor->setChecked(opts.time_triggered_seg_update);
    this->checkBoxPasteAsMax->setChecked(opts.m_bPasteAsMax);

    this->m_dialog->update();
    this->m_dialog->raise();
    this->m_dialog->show( );

    return *this;
}

void KViewerParameterWidget::updatedViewSelection( )
{
  auto who_sent_it = sender();
  PRINT_AND_EVAL( who_sent_it );
  auto button_sender = dynamic_cast<QRadioButton*>(who_sent_it);
  if( button_sender && !button_sender->isChecked() )
    return;

  m_viewCallback(radioButton_View0->isChecked(),radioButton_View1->isChecked(),radioButton_View2->isChecked());

}

void KViewerParameterWidget::updatedBasicParams( )
{
    auto opts = m_kvopts.lock();
    if (!opts){ return; }

    if (m_satLUT)
    {
        double s0 = this->satLUT_MinSpinbox->value();
        double s1 = this->satLUT_MaxSpinbox->value();
        m_satLUT->SetRange( s0, s1 );
    }

    opts->paintBrushRad   = this->brushSizeSpinbox->value();
    opts->labelOpacity2D  = this->labelOpacitySpinbox->value();

    opts->lambda = this->lambdaSmoothnessSpinbox->value();
    opts->seg_time_interval = this->autoTriggerContourSpinbox->value();
    opts->labelOpacity2D = this->labelOpacitySpinbox->value();
    opts->seg_time_interval = this->autoTriggerContourSpinbox->value();
    opts->rad = this->regionSizeRBACSpinBox->value();
    opts->segmentor_iters = this->contourIterationsSpinBox->value();

    opts->multilabel_paste_mode     = this->checkBoxCopyPasteAllLabels->isChecked();
    opts->multilabel_sgmnt_mode     = this->checkBoxEvolveAllLabels->isChecked();
    opts->time_triggered_seg_update = this->checkBoxEnableAutoTriggerSegmentor->isChecked();
    opts->m_bPasteAsMax             = this->checkBoxPasteAsMax->isChecked();

    m_updateCallback(opts);
}

void KViewerParameterWidget::startAutoShiftAndSegment()
{
    this->pushButton_AutoSegGoLeft->setEnabled(false);
    this->pushButton_AutoSegGoRight->setEnabled(false);
    this->pushButton_AutoSegStop->setEnabled(true);

    int nSliceSteps   = this->spinBox_AutoShiftSteps->value();
    int nSegmentSteps = this->spinBox_AutoSegmentSteps->value();
    int iDirection    = 0;
    auto _sender      = sender();
    if(_sender == pushButton_AutoSegGoLeft )
        iDirection = -1;
    if(_sender == pushButton_AutoSegGoRight )
        iDirection = +1;

    auto bAutoSegCP = checkBox_CopyPasteDuringAutoStepSegment->isChecked();
    if(auto opts = m_kvopts.lock())
      opts->m_bAutoShiftSegDoesCopyAndPaste = bAutoSegCP;

    m_startAutoSegCallback( nSliceSteps, nSegmentSteps, iDirection );

    stopAutoShiftAndSegment();
}

void KViewerParameterWidget::stopAutoShiftAndSegment()
{
    this->pushButton_AutoSegGoLeft->setEnabled(true);
    this->pushButton_AutoSegGoRight->setEnabled(true);
    this->pushButton_AutoSegStop->setEnabled(false);
    m_stopAutoSegCallback( );
}
