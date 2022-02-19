#pragma once
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qlabel.h>
#include <qpixmap.h>
#include <qapplication.h>

#include "shimeji.h"

class ControlPanel : public QWidget
{
public:
    ControlPanel();
    ~ControlPanel();

};

extern ControlPanel* gPanel;

