#include <QtGui>
#include <QRubberBand>

#ifdef Q_WS_WIN
#include "windows.h"
#endif

#include "areaselector.h"

AreaSelector::AreaSelector()
{
  setCursor(Qt::CrossCursor);

  mPixmapLabel = 0;

  QPixmap p = QPixmap::grabWindow(QApplication::desktop()->winId());

  // Save the original desktop pixmap
  mDesktopPixmap = p;

  //Transforming the pixmap to make the user notice the change between desktop and area selector
  QPainter painter(&p);
  painter.setBrush(QBrush(QColor(0, 0, 0, 85), Qt::SolidPattern));
  painter.drawRect(p.rect());

  painter.setRenderHint(QPainter::Antialiasing);

  //Drawing the explanatory text.
  QRect textRect = p.rect();
  QString text = tr("Lightscreen screen area mode:\nUse your mouse to draw a rectangle to screenshot or exit pressing\nany key or using the right or middle mouse buttons.");
  textRect.setHeight(qRound(p.rect().height() / 10)); // We get a decently sized rect where the text should be drawn (centered)

  // We draw the white contrasting background for the text, using the same text and options to get the boundingRect that the text will have.
  painter.setPen(QPen(Qt::white));
  painter.setBrush(QBrush(QColor(255, 255, 255, 180), Qt::SolidPattern));
  QRectF bRect = painter.boundingRect(textRect, Qt::AlignCenter, text);

  // These four calls provide a 3px padding for the rect
  bRect.setHeight(bRect.height() + 6.0f);
  bRect.setWidth(bRect.width() + 6.0f);
  bRect.setX(bRect.x() - 3.0f);
  bRect.setY(bRect.y() - 3.0f);

  painter.drawRoundRect(bRect, 10, 10);

  // We draw the text:
  painter.setPen(QPen(Qt::black));
  painter.drawText(textRect, Qt::AlignCenter, text);

  mOriginalPixmap = p;

#ifdef Q_WS_WIN
  // Windows-specific: Set the window always on top
  ::SetWindowPos(winId(), HWND_TOPMOST, 200, 200, 100, 100, SWP_SHOWWINDOW);
  SetForegroundWindow(winId());
#endif

  QTimer::singleShot(0, this, SLOT(setFullScreen()));
}

QRect AreaSelector::getRect()
{
  return mRect;
}


void AreaSelector::paintEvent(QPaintEvent *event)
{
  //qDebug() << "PaintEvent called: " << mRect.height();

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  // Draw the altered pixmap
  painter.drawPixmap(rect(), mOriginalPixmap);

  // Draw the "light" version, the target, in the moused rect
  painter.setPen(QPen(QBrush(QColor(255, 0, 0, 140)), 2));
  painter.drawPixmap(mRect, mDesktopPixmap, mRect);

  // Border
  painter.drawRect(mRect);

  // Draw the size in the bottom right of the selection rect
  QString text = tr("%1 x %2 px ").arg(mRect.width()).arg(mRect.height());
  painter.drawText(mRect, Qt::AlignBottom | Qt::AlignRight, text);
}

void AreaSelector::setFullScreen()
{
  setWindowState(Qt::WindowFullScreen);
}

void AreaSelector::mousePressEvent(QMouseEvent *event)
{
  if (event->button() != Qt::LeftButton)
  {
    mRect = QRect(QPoint(0, 0), QSize());
    accept();
  }

  mOrigin = event->pos();
  mRect = QRect(mOrigin, QSize());
}

void AreaSelector::mouseMoveEvent(QMouseEvent *event)
{
  mRect = QRect(mOrigin, event->pos()).normalized();
  update();
}

void AreaSelector::mouseReleaseEvent(QMouseEvent *event)
{
  event->ignore();
  accept();
}
