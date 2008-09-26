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

private slots:
  void drawBackground();
  void drawRectangleSelector(QPainter &painter);

protected:
  bool event(QEvent *event);

private:
  QRect mRect;
  QPoint mOrigin;
  QPixmap mCleanDesktop;

};

#endif /*AREASELECTOR_H_*/
