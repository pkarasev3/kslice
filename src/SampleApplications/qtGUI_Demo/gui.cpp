#include <QtGui>
#include "gui.h"

gui::gui(QWidget *)
{
    setupUi(this);

    connect(pushButton_browse1, SIGNAL(clicked()), this, SLOT(getPath1()));
    connect(pushButton_browse2, SIGNAL(clicked()), this, SLOT(getPath2()));
}

void gui::getPath1()
{
    QString path;
    path = QFileDialog::getOpenFileName(
            this,
            "Choose a file to open",
            QString::null,
            QString::null);
    lineEdit_1->setText(path);
}

void gui::getPath2()
{
    QString path;
    path = QFileDialog::getOpenFileName(
            this,
            "Choose a file to open",
            QString::null,
            QString::null);
    lineEdit_2->setText(path);
}

