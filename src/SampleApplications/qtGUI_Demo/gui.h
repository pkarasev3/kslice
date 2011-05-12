#ifndef GUI_H
#define GUI_H

#include <QtGui/QWidget>
#include "ui_gui.h"

namespace Ui
{
    class gui;
}

class gui : public QWidget, private Ui::gui
{
    Q_OBJECT

public:
    gui(QWidget *parent = 0);

public slots:
    void getPath1();
    void getPath2();

};

#endif // GUI_H
