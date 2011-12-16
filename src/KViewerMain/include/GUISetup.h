#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QRadioButton>
#include <QtGui/QStatusBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "QVTKWidget.h"
#include "qslider.h"
#include "qpushbutton.h"


QT_BEGIN_NAMESPACE

class GUITemplate
{
public:
  QMenuBar *menubar;
  QMenu *menuFile, *menuHelp;
  QAction *actionExit, *actionLoadImage, *actionmousePaintEventMap,*actionAbout;
  QWidget *centralwidget;
  QVBoxLayout *infoVBoxLayout;
  QVBoxLayout *keyboardTipsVBoxLayout;
  QVBoxLayout *statusVBoxLayout;
  QHBoxLayout *hToolbar, *hGraphicsLayout, *hSaveBar;

  /** 3D Widgets, do the heavy graphics work */
  QVTKWidget *qVTK1, *qVTK2;
  /** labels to display status on screen */
  QLabel *coord, *paintBrushStatus, *volumeOfLabelmap,*segmentationInterval,*distweight;

  /** static labels, keyboard tips */
  QLabel *copyPasteKeyTip, *contrastKeyTip, *segmentTip, *drawEraseTip;

  /** place to enter file name to save as */
  QLineEdit *saveAsLineEdit;

  /** slice slider and min/max threshold sliders */
  QSlider *Slider, *maxThreshSlider, *minThreshSlider;

  QPushButton *FillEraseButton, *ComputeVolumeButton, *RunSegmentButton, 
              *SaveSegmentButton, *SaveAsButton,*CopyButton, *PasteButton,
              *NewLabelMapButton;

  std::string GUI_MAIN_WINDOW_NAME;
  size_t      GUI_WINDOW_WIDTH;
  size_t      GUI_WINDOW_HEIGHT;

  void setupGUI(QMainWindow *GUI, double sliderMin,double  sliderMax, double sliceZSpace,
                                  double threshMin = 0.0, double threshMax = 1.0e5 )
  {
     GUI_MAIN_WINDOW_NAME = "KViewer-0.6";
     GUI_WINDOW_HEIGHT = 800;
     GUI_WINDOW_WIDTH  = 1200;

    if (GUI->objectName().isEmpty()) {
        GUI->setObjectName(QString::fromUtf8( GUI_MAIN_WINDOW_NAME.c_str() ));
    }

    GUI->resize(GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT);

    /* Main Window Creation */
    centralwidget = new QWidget(GUI);
    centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
    centralwidget->setFocusPolicy( Qt::NoFocus );

    /* Menu Creation */
    actionExit = new QAction(GUI);
    actionLoadImage = new QAction(GUI);
    actionmousePaintEventMap = new QAction(GUI);
    actionAbout = new QAction(GUI);

    menubar = new QMenuBar(GUI);
    menubar->setGeometry(QRect(0, 0, 4, 100));

    menuFile = new QMenu(menubar);
    menuFile->addAction(actionLoadImage);
    menuFile->addAction(actionmousePaintEventMap);
    menuFile->addAction(actionExit);

    menuHelp = new QMenu(menubar);
    menuHelp->addAction(actionAbout);

    menubar->addAction(menuFile->menuAction());
    menubar->addAction(menuHelp->menuAction());


    /* Buttons, TODO: Move Save to File Menu */
    RunSegmentButton= new QPushButton( GUI );
    RunSegmentButton->setText("KSegmentor");
    FillEraseButton= new QPushButton( GUI );
    FillEraseButton->setText("Toggle Draw/Erase");
    ComputeVolumeButton= new QPushButton( GUI );
    ComputeVolumeButton->setText("Update 3D and Volume");
    SaveSegmentButton= new QPushButton( GUI );
    SaveSegmentButton->setText("Save Label Map Timestamp");
    
    SaveAsButton = new QPushButton( GUI );
    SaveAsButton->setText("Save Label Map As");
    CopyButton   = new QPushButton( GUI );
    CopyButton->setText("Copy Label Slice");
    PasteButton   = new QPushButton( GUI );
    PasteButton->setText("Paste Label Slice");
    NewLabelMapButton = new QPushButton( GUI );
    NewLabelMapButton->setText("Add New LabelMap");

    /* QVTK Widget Creation */
    cout<<"Creating first qVTK widget from parent centralWidget...";
    qVTK1 = new QVTKWidget(centralwidget);
    cout<<"Done! \n";
    qVTK1->setObjectName(QString::fromUtf8("qVTK1"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(3));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(qVTK1->sizePolicy().hasHeightForWidth());
    qVTK1->setSizePolicy(sizePolicy);

    qVTK2 = new QVTKWidget(centralwidget);
    qVTK2->setObjectName(QString::fromUtf8("qVTK2"));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(7));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(qVTK2->sizePolicy().hasHeightForWidth());
    qVTK2->setSizePolicy(sizePolicy1);


    /* Slider */
    Slider = new QSlider(Qt::Horizontal);
    Slider->setFocusPolicy(Qt::StrongFocus);
    Slider->setTickPosition(QSlider::TicksBothSides);
    Slider->setMinimum(sliderMin);
    Slider->setMaximum(sliderMax);
    Slider->setTickInterval(10);
    Slider->setSingleStep(sliceZSpace); //if step size <1, it does nothing!

    int SliceSliderWidth  = 1;
    int SliceSliderHeight = 3;
    Slider->setSizeIncrement( SliceSliderWidth, SliceSliderHeight );

		maxThreshSlider = new QSlider(Qt::Horizontal);
		maxThreshSlider->setFocusPolicy(Qt::StrongFocus);
		maxThreshSlider->setTickPosition(QSlider::TicksBothSides);
		maxThreshSlider->setMinimum(threshMin);
		maxThreshSlider->setMaximum(threshMax );
		maxThreshSlider->setTickInterval((threshMax-threshMin)/50);
		maxThreshSlider->setSingleStep(1);

		minThreshSlider = new QSlider(Qt::Horizontal);
		minThreshSlider->setFocusPolicy(Qt::StrongFocus);
		minThreshSlider->setTickPosition(QSlider::TicksBothSides);
		minThreshSlider->setMinimum(threshMin);
		minThreshSlider->setMaximum(threshMax);
		minThreshSlider->setTickInterval((threshMax-threshMin)/50);
		minThreshSlider->setSingleStep(1);


    /* Status Labels */
    coord = new QLabel(centralwidget);
    coord->setObjectName(QString::fromUtf8("coord"));
    coord->setAlignment(Qt::AlignCenter);

    paintBrushStatus = new QLabel(centralwidget);
    paintBrushStatus->setObjectName(QString::fromUtf8("paintBrushStatus"));
    paintBrushStatus->setAlignment(Qt::AlignCenter);

    volumeOfLabelmap = new QLabel(centralwidget);
    volumeOfLabelmap->setObjectName(QString::fromUtf8("volumeOfLabelmap"));
    volumeOfLabelmap->setAlignment(Qt::AlignCenter);

    //creating qlabel to display segmentation interval
    segmentationInterval = new QLabel(centralwidget);
    segmentationInterval->setObjectName(QString::fromUtf8("timeIntervalForSeg"));
    segmentationInterval->setAlignment(Qt::AlignCenter);

    distweight = new QLabel(centralwidget);
    distweight->setObjectName(QString::fromUtf8("distanceWeight"));
    distweight->setAlignment(Qt::AlignCenter);


    /* Line Edit, TODO: Move Saving functionality to menu */
    saveAsLineEdit = new QLineEdit( GUI );
    saveAsLineEdit->setObjectName(QString::fromUtf8("saveAsLineEdit"));
    saveAsLineEdit->setText(QString::fromUtf8("Insert filename here..."));


    /* XML Layout Connections*/
    statusVBoxLayout = new QVBoxLayout();
    statusVBoxLayout->addWidget( coord );
    statusVBoxLayout->addWidget( paintBrushStatus );
    statusVBoxLayout->addWidget( volumeOfLabelmap );
    statusVBoxLayout->addWidget( segmentationInterval );
    statusVBoxLayout->addWidget( distweight );

    hToolbar = new QHBoxLayout();
    hToolbar->addWidget(FillEraseButton);
    hToolbar->addWidget(ComputeVolumeButton);
    hToolbar->addWidget(RunSegmentButton);
    hToolbar->addStrut(2);
    hToolbar->addWidget(CopyButton);
    hToolbar->addWidget(PasteButton);
    hToolbar->addWidget(NewLabelMapButton);

    hGraphicsLayout = new QHBoxLayout();
    hGraphicsLayout->addWidget(qVTK1);
    hGraphicsLayout->addWidget(qVTK2);

    keyboardTipsVBoxLayout = new QVBoxLayout();
    contrastKeyTip = new QLabel();
    contrastKeyTip->setObjectName(QString::fromUtf8("contrastkeytip"));
    contrastKeyTip->setAlignment(Qt::AlignCenter);
    copyPasteKeyTip = new QLabel();
    copyPasteKeyTip->setObjectName(QString::fromUtf8("copypastekeytip"));
    copyPasteKeyTip->setAlignment(Qt::AlignCenter);
    segmentTip = new QLabel();
    segmentTip->setObjectName(QString::fromUtf8("segmentkeytip"));
    segmentTip->setAlignment(Qt::AlignCenter);
    drawEraseTip = new QLabel();
    drawEraseTip->setObjectName(QString::fromUtf8("drawerasekeytip"));
    drawEraseTip->setAlignment(Qt::AlignCenter);
    keyboardTipsVBoxLayout->addWidget( segmentTip );
    keyboardTipsVBoxLayout->addWidget( drawEraseTip );
    keyboardTipsVBoxLayout->addWidget( contrastKeyTip );
    keyboardTipsVBoxLayout->addWidget( copyPasteKeyTip );

    hSaveBar = new QHBoxLayout();
    hSaveBar->addLayout(keyboardTipsVBoxLayout);
    hSaveBar->addLayout(statusVBoxLayout);
    hSaveBar->addWidget(saveAsLineEdit);
    hSaveBar->addWidget(SaveAsButton);
    hSaveBar->addWidget(SaveSegmentButton);

    infoVBoxLayout   = new QVBoxLayout(centralwidget);
    infoVBoxLayout->addLayout(hToolbar);
    infoVBoxLayout->addLayout(hGraphicsLayout);
    infoVBoxLayout->addWidget(Slider);
    infoVBoxLayout->addLayout(hSaveBar);

    GUI->setCentralWidget(centralwidget);
    GUI->setMenuBar(menubar);

    cout<<"Harvesting Tiberium..." << endl;
    retranslateGUI(GUI);

    QObject::connect(actionExit, SIGNAL(triggered()), GUI, SLOT(close()));
    QMetaObject::connectSlotsByName(GUI);

  }

  void retranslateGUI(QMainWindow *GUI)
  {
    GUI->setWindowTitle(QApplication::translate("GUI", "KViewer", 0, QApplication::UnicodeUTF8));
    actionExit->setText(QApplication::translate("GUI", "E&xit", 0, QApplication::UnicodeUTF8));
    actionLoadImage->setText(QApplication::translate("GUI", "Change I&mage Volume", 0, QApplication::UnicodeUTF8));
    actionmousePaintEventMap->setText(QApplication::translate("GUI", "Change L&abel Map", 0, QApplication::UnicodeUTF8));
    actionAbout->setText(QApplication::translate("GUI", "&About KSlice", 0, QApplication::UnicodeUTF8));

    coord->setText(QApplication::translate("GUI", "Awaiting Mouse Input on Label Map", 0, QApplication::UnicodeUTF8));
    volumeOfLabelmap->setText(QApplication::translate("GUI", "Volume: not yet computed", 0, QApplication::UnicodeUTF8));
    distweight->setText(QApplication::translate("GUI", "dist. weight:", 0, QApplication::UnicodeUTF8));
    segmentationInterval->setText(QApplication::translate("GUI", "seg. int.:", 0, QApplication::UnicodeUTF8));
    paintBrushStatus->setText(QApplication::translate("GUI", "Toggled: Draw", 0, QApplication::UnicodeUTF8));

    contrastKeyTip->setText(QApplication::translate("GUI", "Contrast up/down keys: [l,k]; Label Opacity up/down keys: [o,p]", 0, QApplication::UnicodeUTF8));
    copyPasteKeyTip->setText(QApplication::translate("GUI", "Copy/Paste Slice keys: [c,v]; Scroll thru labels keys: [1,2]", 0, QApplication::UnicodeUTF8));
    segmentTip->setText(QApplication::translate("GUI", "Run KSegmentor key:  s ", 0, QApplication::UnicodeUTF8));
    drawEraseTip->setText(QApplication::translate("GUI", "Toggle Draw/Erase key:  d ", 0, QApplication::UnicodeUTF8));

    menuFile->setTitle(QApplication::translate("GUI", "&File", 0, QApplication::UnicodeUTF8));
    menuHelp->setTitle(QApplication::translate("GUI", "&Help", 0, QApplication::UnicodeUTF8));
  }

};

namespace Ui {
  class GUI: public GUITemplate {};
}

QT_END_NAMESPACE

# if 0

// PURGATORY

#endif
