#include "ParameterWidget.h"
#include "KViewer.h"
#include "KViewerOptions.h"
#include <QDoubleSpinBox>

KViewerParameterWidget::KViewerParameterWidget()
{    
    m_dialog = std::make_unique<QDialog>(nullptr);    
    setupUi(m_dialog.get());
    m_dialog->show();

    setOptionsUpdateCallback([](std::shared_ptr<KViewerOptions>){});

    this->brushSizeSpinbox->setRange(0,1000);

    auto bOK = QList<bool>()
    <<connect(brushSizeSpinbox,SIGNAL(valueChanged(int)),    this,SLOT(updatedBasicParams()),Qt::QueuedConnection)
    <<connect(labelOpacitySpinbox,SIGNAL(valueChanged(double)), this,SLOT(updatedBasicParams()), Qt::QueuedConnection)
    
    /*                              */; Q_ASSERT(!bOK.contains(false));
}

KViewerParameterWidget::~KViewerParameterWidget()
{
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

    this->blockSignals(true);
    this->brushSizeSpinbox->setValue( opts.paintBrushRad );
    this->satLUT_MinSpinbox->setValue(0);
    this->satLUT_MaxSpinbox->setValue(1e4);
    this->lambdaSmoothnessSpinbox->setValue(opts.lambda);
    this->autoTriggerContourSpinbox->setValue(opts.seg_time_interval);
    this->labelOpacitySpinbox->setValue(opts.labelOpacity2D);

    this->m_dialog->update();
    this->m_dialog->raise();
    this->m_dialog->show( );    
    
    this->blockSignals(false);
    return *this;
}

void KViewerParameterWidget::updatedBasicParams( )
{
    auto opts = m_kvopts.lock();
    if (!opts){ return; }

    opts->paintBrushRad   = this->brushSizeSpinbox->value();
    opts->labelOpacity2D  = this->labelOpacitySpinbox->value();

    m_updateCallback(opts);
}