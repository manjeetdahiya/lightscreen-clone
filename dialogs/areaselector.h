#ifndef AREASELECTOR_H_
#define AREASELECTOR_H_

#include <QDialog>

class AreaSelector: public QDialog
{

public:
  AreaSelector(QPixmap desktop, bool magnify = false);
  QRect rect();
  QPixmap pixmap();

private:
  void drawBackground();
  void drawRectangleSelector(QPainter &painter);

protected:
  bool event(QEvent *event);

private:
  QRect   mRect;
  QPoint  mOrigin;
  QPoint  mPos;
  QPixmap mCleanDesktop;
  QPixmap mBackgroundDesktop;
  bool mMagnify;
  bool mPaletteBackground;

};

#endif /*AREASELECTOR_H_*/
