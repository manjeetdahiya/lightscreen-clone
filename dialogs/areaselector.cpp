#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>

#if defined(Q_WS_WIN)
  #include "windows.h"
#endif

#include "areaselector.h"

AreaSelector::AreaSelector(QPixmap desktop, bool magnify) : mCleanDesktop(desktop), mMagnify(magnify)
{
  setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
  setCursor(Qt::CrossCursor);

  resize(desktop.size());
  move(0, 0);

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
  mBackgroundDesktop = mCleanDesktop;

  //Transforming the pixmap to make the user notice the change between desktop and area selector (85% transparent black overlay)
  QPainter painter(&mBackgroundDesktop);
  painter.setBrush(QBrush(QColor(0, 0, 0, 85), Qt::SolidPattern));
  painter.drawRect(mBackgroundDesktop.rect());

  //Drawing the explanatory text.
  QRect textRect = qApp->desktop()->screenGeometry(qApp->desktop()->primaryScreen());
  QString text = tr("Lightscreen screen area mode:\nUse your mouse to draw a rectangle to screenshot or exit pressing\nany key or using the right or middle mouse buttons.");
  textRect.setHeight(qRound(textRect.height() / 10)); // We get a decently sized rect where the text should be drawn (centered)

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

  // Set our pixmap as the background of the widget. This does not work in multiple monitors, it tiles before it should
  //QPalette newPalette = palette();
  //newPalette.setBrush(QPalette::Window, brush);
  //setPalette(newPalette);
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

  if (mPos.isNull() || !mMagnify)
    return;

  // Drawing the magnified version
  QPoint magEnd = mPos;
  magEnd += QPoint(50, 50);

  QPoint magStart = mPos;
  magStart -= QPoint(50, 50);

  QRect newRect = QRect(magStart, magEnd);

  QPixmap magnified = mCleanDesktop.copy(newRect).scaled(QSize(200, 200));
  QPainter magPainter(&magnified);
  magPainter.setPen(QPen(QBrush(QColor(255, 0, 0, 180)), 2)); // Same border pen
  magPainter.drawRect(magnified.rect());
  magPainter.drawText(magnified.rect().center()-QPoint(4, -4), "+"); //Center minus the 4 pixels wide and across of the "+"

  QPoint drawPosition = mRect.bottomRight();

  if ((drawPosition.x()+200) > mCleanDesktop.rect().width() || (drawPosition.y()+200) > mCleanDesktop.rect().height())
    drawPosition -= QPoint(200, 200);

  painter.drawPixmap(drawPosition, magnified);
}

// Protected event

bool AreaSelector::event(QEvent *event)
{
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
    mPos  = mouseEvent->pos();
    update();
  }
  if (event->type() == QEvent::Paint)
  {
    QPainter painter(this);
    painter.drawPixmap(QPoint(0, 0), mBackgroundDesktop);
    drawRectangleSelector(painter);
  }

  return QDialog::event(event);
}
