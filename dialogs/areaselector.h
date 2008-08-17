#ifndef AREASELECTOR_H_
#define AREASELECTOR_H_

#include <QDialog>

class QLabel;
class QRubberBand;

class AreaSelector: public QDialog
{
  Q_OBJECT

public:
	AreaSelector();
	QRect getRect();

public slots:
  void setFullScreen();

protected:
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  //void keyPressEvent(QKeyEvent *event);
  //void keyReleaseEvent(QKeyEvent *event);
  void paintEvent(QPaintEvent *event);

private:
  QPixmap mOriginalPixmap;
  QPixmap mDesktopPixmap;
  QRect mRect;
  QLabel *mPixmapLabel;
  QPoint mOrigin;

};

#endif /*AREASELECTOR_H_*/
