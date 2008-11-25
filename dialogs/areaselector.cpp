#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>

#ifdef Q_WS_WIN
#include "windows.h"
#endif

#include "areaselector.h"

AreaSelector::AreaSelector()
{
  setWindowFlags(Qt::WindowStaysOnTopHint);
  setCursor(Qt::CrossCursor);

  drawBackground();
}

QRect AreaSelector::rect()
{
  return mRect;
}

QPixmap AreaSelector::pixmap()
{
  return mCleanDesktop.copy(rect());
}

// Drawing slots

void AreaSelector::drawBackground()
{
  QPixmap desktop = QPixmap::grabWindow(QApplication::desktop()->winId());
  mCleanDesktop = desktop;

  //Transforming the pixmap to make the user notice the change between desktop and area selector (85% transparent black overlay)
  QPainter painter(&desktop);
  painter.setBrush(QBrush(QColor(0, 0, 0, 85), Qt::SolidPattern));
  painter.drawRect(desktop.rect());

  //Drawing the explanatory text.
  QRect textRect = desktop.rect();
  QString text = tr("Lightscreen screen area mode:\nUse your mouse to draw a rectangle to screenshot or exit pressing\nany key or using the right or middle mouse buttons.");
  textRect.setHeight(qRound(desktop.rect().height() / 10)); // We get a decently sized rect where the text should be drawn (centered)

  // We draw the white contrasting background for the text, using the same text and options to get the boundingRect that the text will have.
  painter.setPen(QPen(Qt::white));
  painter.setBrush(QBrush(QColor(255, 255, 255, 180), Qt::SolidPattern));
  QRectF bRect = painter.boundingRect(textRect, Qt::AlignCenter, text);

  // These four calls provide padding for the rect
  bRect.setHeight(bRect.height() + 8.0f);
  bRect.setWidth(bRect.width() + 8.0f);
  bRect.setX(bRect.x() - 4.0f);
  bRect.setY(bRect.y() - 4.0f);

  painter.drawRoundedRect(bRect, 8, 8);

  // Draw the text:
  painter.setPen(QPen(Qt::black));
  painter.drawText(textRect, Qt::AlignCenter, text);

  // Set our pixmap as the background of the widget.
  QPalette newPalette = palette();
  newPalette.setBrush(backgroundRole(), QBrush(desktop));
  setPalette(newPalette);
}

void AreaSelector::drawRectangleSelector(QPainter &painter)
{
  // Draw the "light" version, the target, in the moused rect
  painter.drawPixmap(mRect, mCleanDesktop, mRect);

  // Border
  painter.setPen(QPen(QBrush(QColor(255, 0, 0, 180)), 2));
  painter.drawRect(mRect);

  // Draw the size in the bottom right of the selection rect
  QString text = tr("%1 x %2 px ").arg(mRect.width()).arg(mRect.height());
  painter.drawText(mRect, Qt::AlignBottom | Qt::AlignRight, text);
}

// Protected event

bool AreaSelector::event(QEvent *event)
{
  if (event->type() == QEvent::Show)
  {
    setWindowState(Qt::WindowFullScreen);
  }
  if (event->type() == QEvent::MouseButtonRelease
   || event->type() == QEvent::KeyPress)
  {
    accept();
  }
  if (event->type() == QEvent::MouseButtonPress)
  {
    QMouseEvent *mouseEvent = static_cast<QMouseEvent*> (event);

    if (mouseEvent->button() != Qt::LeftButton)
      reject();

    mOrigin = mouseEvent->pos();
    mRect = QRect(mOrigin, QSize());
  }
  if (event->type() == QEvent::MouseMove)
  {
    QMouseEvent *mouseEvent = static_cast<QMouseEvent*> (event);
    mRect = QRect(mOrigin, mouseEvent->pos()).normalized();
    update();
  }
  if (event->type() == QEvent::Paint)
  {
    QPainter painter(this);
    drawRectangleSelector(painter);
  }

  return QDialog::event(event);
}
