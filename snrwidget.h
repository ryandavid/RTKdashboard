#ifndef SNRWIDGET_H
#define SNRWIDGET_H
#include <QFrame>

class SnrWidget : public QFrame
{
    Q_OBJECT

public:
    SnrWidget(QWidget *parent = 0);

protected:
  void paintEvent(QPaintEvent *e);
  void drawWidget(QPainter &qp);

private:
  QWidget *m_parent;
  SnrWidget *snr;

};
#endif // SNRWIDGET_H
