#include "previewdialog.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>
#include <QTimeLine>

PreviewDialog::PreviewDialog(QPixmap &screenshot) : QDialog(0)
{
  setWindowFlags( Qt::WindowStaysOnTopHint | Qt::Popup);
  setMouseTracking(true);

  QSize size = screenshot.size();

  if (!(size.width() < 200 && size.height() < 200))
  {
    size.scale(200, 200, Qt::KeepAspectRatio);
  }

  // Set our pixmap as the background of the widget.
  QPalette newPalette = palette();
  newPalette.setBrush(backgroundRole(), QBrush(screenshot.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
  setPalette(newPalette);

  resize(size);

  QPoint position = QApplication::desktop()->availableGeometry().bottomRight();
  position.setX(position.x() - size.width());
  //position.setY(position.y() - size.height());

  move(position);

  QTimeLine *effectTimeLine = new QTimeLine(400, this);
  effectTimeLine->setFrameRange(position.y(), position.y() - size.height());

  connect(effectTimeLine, SIGNAL(frameChanged(int)), this, SLOT(effect(int)));
  connect(&deathTimer, SIGNAL(timeout()), this, SLOT(kill()));

  deathTimer.setInterval(3000);

  show();
  deathTimer.start();
  effectTimeLine->start();
}

void PreviewDialog::kill()
{
  deleteLater();
  close();
}

bool PreviewDialog::event(QEvent *event)
{
  if (event->type() == QEvent::MouseButtonPress)
  {
    //QMouseEvent *mouseEvent = static_cast<QMouseEvent*> (event);
    //if (mouseEvent->button() != Qt::RightButton)
    //TODO: Open the screenshot file?

    kill();
  }
  if (event->type() == QEvent::MouseMove)
  {
    setFocus();
  }
  if (event->type() == QEvent::FocusIn)
  {
    deathTimer.stop();
  }
  if (event->type() == QEvent::FocusOut)
  {
    deathTimer.start();
  }

  return QDialog::event(event);
}

void PreviewDialog::effect(int frame)
{
  move(pos().x(), frame);
}
