#pragma once
#include <QtWidgets/qwidget.h>
#include <qapplication.h>

class ControlPanel : public QWidget
{
public:
    ControlPanel();
    ~ControlPanel();

};

extern ControlPanel* gPanel;