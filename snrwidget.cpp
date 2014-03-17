#include <QtGui>
#include <QFrame>
#include <QSlider>
#include "snrwidget.h"

SnrWidget::SnrWidget(QWidget *parent) : QFrame(parent)
{
    initUI();
}

void SnrWidget::initUI()
{
  const int MAX_VALUE = 750;
  cur_width = 0;

  slider = new QSlider(Qt::Horizontal , this);
  slider->setMaximum(MAX_VALUE);
  slider->setGeometry(50, 50, 130, 30);

  connect(slider, SIGNAL(valueChanged(int)),
          this, SLOT(valueChanged(int)));

  QVBoxLayout *vbox = new QVBoxLayout(this);
  QHBoxLayout *hbox = new QHBoxLayout();

  vbox->addStretch(1);

  widget = new Widget(this);
  hbox->addWidget(widget, 0);

  vbox->addLayout(hbox);

  setLayout(vbox);
}

void SnrWidget::valueChanged(int val)
{
  cur_width = val;
  widget->repaint();
}

int SnrWidget::getCurrentWidth()
{
  return cur_width;
}
