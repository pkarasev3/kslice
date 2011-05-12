/********************************************************************************
** Form generated from reading UI file 'gui.ui'
**
** Created: Thu Feb 24 14:40:45 2011
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GUI_H
#define UI_GUI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFormLayout>
#include <QtGui/QGraphicsView>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_gui
{
public:
    QFormLayout *formLayout;
    QHBoxLayout *horizontalLayout_3;
    QGraphicsView *graphicsView;
    QGraphicsView *graphicsView_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *lineEdit_2;
    QPushButton *pushButton_browse2;
    QPushButton *pushButton_load2;
    QHBoxLayout *horizontalLayout;
    QLabel *label_1;
    QLineEdit *lineEdit_1;
    QPushButton *pushButton_browse1;
    QPushButton *pushButton_load1;
    QHBoxLayout *horizontalLayout_4;

    void setupUi(QWidget *gui)
    {
        if (gui->objectName().isEmpty())
            gui->setObjectName(QString::fromUtf8("gui"));
        gui->resize(527, 285);
        formLayout = new QFormLayout(gui);
        formLayout->setSpacing(6);
        formLayout->setContentsMargins(11, 11, 11, 11);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        graphicsView = new QGraphicsView(gui);
        graphicsView->setObjectName(QString::fromUtf8("graphicsView"));

        horizontalLayout_3->addWidget(graphicsView);

        graphicsView_2 = new QGraphicsView(gui);
        graphicsView_2->setObjectName(QString::fromUtf8("graphicsView_2"));

        horizontalLayout_3->addWidget(graphicsView_2);


        formLayout->setLayout(2, QFormLayout::SpanningRole, horizontalLayout_3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_2 = new QLabel(gui);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_2->addWidget(label_2);

        lineEdit_2 = new QLineEdit(gui);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));

        horizontalLayout_2->addWidget(lineEdit_2);

        pushButton_browse2 = new QPushButton(gui);
        pushButton_browse2->setObjectName(QString::fromUtf8("pushButton_browse2"));

        horizontalLayout_2->addWidget(pushButton_browse2);

        pushButton_load2 = new QPushButton(gui);
        pushButton_load2->setObjectName(QString::fromUtf8("pushButton_load2"));

        horizontalLayout_2->addWidget(pushButton_load2);


        formLayout->setLayout(1, QFormLayout::SpanningRole, horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_1 = new QLabel(gui);
        label_1->setObjectName(QString::fromUtf8("label_1"));

        horizontalLayout->addWidget(label_1);

        lineEdit_1 = new QLineEdit(gui);
        lineEdit_1->setObjectName(QString::fromUtf8("lineEdit_1"));

        horizontalLayout->addWidget(lineEdit_1);

        pushButton_browse1 = new QPushButton(gui);
        pushButton_browse1->setObjectName(QString::fromUtf8("pushButton_browse1"));

        horizontalLayout->addWidget(pushButton_browse1);

        pushButton_load1 = new QPushButton(gui);
        pushButton_load1->setObjectName(QString::fromUtf8("pushButton_load1"));

        horizontalLayout->addWidget(pushButton_load1);


        formLayout->setLayout(0, QFormLayout::SpanningRole, horizontalLayout);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));

        formLayout->setLayout(3, QFormLayout::LabelRole, horizontalLayout_4);

        QWidget::setTabOrder(lineEdit_1, pushButton_browse1);
        QWidget::setTabOrder(pushButton_browse1, pushButton_load1);
        QWidget::setTabOrder(pushButton_load1, lineEdit_2);
        QWidget::setTabOrder(lineEdit_2, pushButton_browse2);
        QWidget::setTabOrder(pushButton_browse2, pushButton_load2);

        retranslateUi(gui);

        QMetaObject::connectSlotsByName(gui);
    } // setupUi

    void retranslateUi(QWidget *gui)
    {
        gui->setWindowTitle(QApplication::translate("gui", "Gui Rev 0", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("gui", "Image 2:", 0, QApplication::UnicodeUTF8));
        pushButton_browse2->setText(QApplication::translate("gui", "Browse", 0, QApplication::UnicodeUTF8));
        pushButton_load2->setText(QApplication::translate("gui", "Load", 0, QApplication::UnicodeUTF8));
        label_1->setText(QApplication::translate("gui", "Image 1:", 0, QApplication::UnicodeUTF8));
        pushButton_browse1->setText(QApplication::translate("gui", "Browse", 0, QApplication::UnicodeUTF8));
        pushButton_load1->setText(QApplication::translate("gui", "Load", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class gui: public Ui_gui {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUI_H
