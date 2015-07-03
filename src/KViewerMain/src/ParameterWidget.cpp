#include "ParameterWidget.h"
#include "KViewer.h"
#include "KViewerOptions.h"
#include <QDoubleSpinBox>
#include <vtkLookupTable.h>


KViewerParameterWidget::KViewerParameterWidget()
{
    m_dialog.reset(new QDialog(nullptr) ); // = std::make_unique<QDialog>(nullptr);
    setupUi(m_dialog.get());
    m_dialog->show();

    setOptionsUpdateCallback([](std::shared_ptr<KViewerOptions>){});

    this->brushSizeSpinbox->setRange(0,1000);
    this->satLUT_MinSpinbox->setRange(-1e5,1e5);
    this->satLUT_MaxSpinbox->setRange(-1e5,1e5);

    auto bOK = QList<bool>()
    <<connect(brushSizeSpinbox,SIGNAL(valueChanged(int)),this,SLOT(updatedBasicParams()),Qt::QueuedConnection)
    <<connect(labelOpacitySpinbox,SIGNAL(valueChanged(double)),this,SLOT(updatedBasicParams()), Qt::QueuedConnection)
    <<connect(satLUT_MinSpinbox,SIGNAL(valueChanged(int)),this,SLOT(updatedBasicParams()), Qt::QueuedConnection)
    <<connect(satLUT_MaxSpinbox,SIGNAL(valueChanged(int)),this,SLOT(updatedBasicParams()), Qt::QueuedConnection)

    /*                              */; Q_ASSERT(!bOK.contains(false));
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

    this->m_dialog->update();
    this->m_dialog->raise();
    this->m_dialog->show( );

    return *this;
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

    m_updateCallback(opts);
}
