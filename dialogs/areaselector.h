#ifndef AREASELECTOR_H_
#define AREASELECTOR_H_

#include <QDialog>

class QLabel;
class QRubberBand;

class AreaSelector: public QDialog
{
  Q_OBJECT

public:
  AreaSelector(QPixmap desktop, bool magnify = false);
  QRect rect();
  QPixmap pixmap();

private slots:
  void drawBackground();
  void drawRectangleSelector(QPainter &painter);

protected:
  bool event(QEvent *event);

private:
  QRect   mRect;
  QPoint  mOrigin;
  QPoint  mPos;
  QPixmap mCleanDesktop;
  bool mMagnify;

};

#endif /*AREASELECTOR_H_*/
