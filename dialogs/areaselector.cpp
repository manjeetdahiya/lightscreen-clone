/*
 * Based on KDE's KSnapshot regiongrabber.cpp, revision 796531, Copyright 2007 Luca Gugelmann <lucag@student.ethz.ch>
 * released under the GNU LGPL  <http://www.gnu.org/licenses/old-licenses/library.txt>
 */

#include "areaselector.h"

#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QToolTip>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTimeLine>

AreaSelector::AreaSelector(QWidget* parent, QPixmap pixmap, bool magnify) :
  QDialog(parent), pixmap(pixmap),  magnify(magnify), selection(), mouseDown(false), newSelection(false),
  handleSize(10), mouseOverHandle(0), idleTimer(),
  showHelp(true), grabbing(false), overlayAlpha(1),
  TLHandle(0,0,handleSize,handleSize), TRHandle(0,0,handleSize,handleSize),
  BLHandle(0,0,handleSize,handleSize), BRHandle(0,0,handleSize,handleSize),
  LHandle(0,0,handleSize,handleSize), THandle(0,0,handleSize,handleSize),
  RHandle(0,0,handleSize,handleSize), BHandle(0,0,handleSize,handleSize)
{
  handles << &TLHandle << &TRHandle << &BLHandle << &BRHandle
      << &LHandle << &THandle << &RHandle << &BHandle;
  setMouseTracking(true);
  setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
  showFullScreen();
  resize(pixmap.size());
  move(0, 0);
  setCursor(Qt::CrossCursor);
  connect(&idleTimer, SIGNAL(timeout()), this, SLOT(displayHelp()));
  idleTimer.start(3000);
  connect(&animationTimeLine, SIGNAL(frameChanged(int)), this, SLOT(animationTick(int)));
  animationTimeLine.setFrameRange(0, 80);
  animationTimeLine.setDuration(500);
  animationTimeLine.start();

  // Creating accept widget:
  acceptWidget = new QWidget(this);
  acceptWidget->resize(110, 60);
  acceptWidget->setWindowOpacity(0.4);
  acceptWidget->setStyleSheet("QWidget { background: url(:/icons/AreaBackground); } QPushButton { background: transparent; border: none; height: 50px; color: white; padding: 0; } QPushButton:hover { image: url(:/icons/AreaHover) }");

  QPushButton *awAcceptButton = new QPushButton(QIcon(":/icons/AreaAccept"), "");
  connect(awAcceptButton, SIGNAL(clicked()), this, SLOT(grabRect()));
  awAcceptButton->setCursor(Qt::PointingHandCursor);
  awAcceptButton->setIconSize(QSize(48, 48));

  QPushButton *awRejectButton = new QPushButton(QIcon(":/icons/AreaCancel"), "");
  connect(awRejectButton, SIGNAL(clicked()), this, SLOT(cancel()));
  awRejectButton->setCursor(Qt::PointingHandCursor);
  awRejectButton->setIconSize(QSize(48, 48));

  QHBoxLayout *awLayout = new QHBoxLayout;
  awLayout->addWidget(awAcceptButton);
  awLayout->addWidget(awRejectButton);
  awLayout->setMargin(0);
  awLayout->setSpacing(0);

  acceptWidget->setLayout(awLayout);
}

AreaSelector::~AreaSelector()
{
}

void AreaSelector::displayHelp()
{
  showHelp = true;
  update();
}

void AreaSelector::paintEvent(QPaintEvent* e)
{
  Q_UNUSED(e);
  if (grabbing) // grabWindow() should just get the background
    return;

  QPainter painter(this);

  QPalette pal = palette();
  QFont font   = QToolTip::font();

  QColor handleColor(255, 0, 0, 180);
  QColor overlayColor(0, 0, 0, overlayAlpha);
  QColor textColor = pal.color(QPalette::Active, QPalette::Text);
  QColor textBackgroundColor = pal.color(QPalette::Active, QPalette::Base);
  painter.drawPixmap(0, 0, pixmap);
  painter.setFont(font);

  QRect r = selection.normalized().adjusted(0, 0, -1, -1);

  QRegion grey(rect());
  grey = grey.subtracted(r);
  painter.setPen(handleColor);
  painter.setBrush(overlayColor);
  painter.setClipRegion(grey);
  painter.drawRect(-1, -1, rect().width() + 1, rect().height() + 1);
  painter.setClipRect(rect());
  painter.setBrush(Qt::NoBrush);
  painter.drawRect(r);

  if (showHelp && overlayAlpha == 80)
  {
    //Drawing the explanatory text.
    QRect helpRect = qApp->desktop()->screenGeometry(qApp->desktop()->primaryScreen());
    QString helpTxt = tr("Lightscreen screen area mode:\nUse your mouse to draw a rectangle to screenshot or exit pressing\nany key or using the right or middle mouse buttons.");
    helpRect.setHeight(qRound(helpRect.height() / 10)); // We get a decently sized rect where the text should be drawn (centered)

    // We draw the white contrasting background for the text, using the same text and options to get the boundingRect that the text will have.
    painter.setPen(QPen(Qt::white));
    painter.setBrush(QBrush(QColor(255, 255, 255, 180), Qt::SolidPattern));
    QRectF bRect = painter.boundingRect(helpRect, Qt::AlignCenter, helpTxt);

    // These four calls provide padding for the rect
    bRect.setHeight(bRect.height() + 8.0f);
    bRect.setWidth(bRect.width() + 8.0f);
    bRect.setX(bRect.x() - 4.0f);
    bRect.setY(bRect.y() - 4.0f);

    painter.drawRoundedRect(bRect, 8, 8);

    // Draw the text:
    painter.setPen(QPen(Qt::black));
    painter.drawText(helpRect, Qt::AlignCenter, helpTxt);
  }

  if (selection.isNull())
  {
    return;
  }
  // The grabbed region is everything which is covered by the drawn
  // rectangles (border included). This means that there is no 0px
  // selection, since a 0px wide rectangle will always be drawn as a line.
  QString txt = QString("%1x%2").arg(selection.width() == 0 ? 2 : selection.width())
      .arg(selection.height() == 0 ? 2 : selection.height());
  QRect textRect = painter.boundingRect(rect(), Qt::AlignLeft, txt);
  QRect boundingRect = textRect.adjusted(-4, 0, 0, 0);

  if (textRect.width() < r.width() - 2*handleSize &&
     textRect.height() < r.height() - 2*handleSize &&
     (r.width() > 100 && r.height() > 100)) // center, unsuitable for small selections
  {
    boundingRect.moveCenter(r.center());
    textRect.moveCenter(r.center());
  }
  else if (r.y() - 3 > textRect.height() &&
    r.x() + textRect.width() < rect().right()) // on top, left aligned
  {
    boundingRect.moveBottomLeft(QPoint(r.x(), r.y() - 3));
    textRect.moveBottomLeft(QPoint(r.x() + 2, r.y() - 3));
  }
  else if (r.x() - 3 > textRect.width()) // left, top aligned
  {
    boundingRect.moveTopRight(QPoint(r.x() - 3, r.y()));
    textRect.moveTopRight(QPoint(r.x() - 5, r.y()));
  }
  else if (r.bottom() + 3 + textRect.height() < rect().bottom() &&
    r.right() > textRect.width()) // at bottom, right aligned
  {
    boundingRect.moveTopRight(QPoint(r.right(), r.bottom() + 3));
    textRect.moveTopRight(QPoint(r.right() - 2, r.bottom() + 3));
  }
  else if (r.right() + textRect.width() + 3 < rect().width()) // right, bottom aligned
  {
    boundingRect.moveBottomLeft(QPoint(r.right() + 3, r.bottom()));
    textRect.moveBottomLeft(QPoint(r.right() + 5, r.bottom()));
  }
  // if the above didn't catch it, you are running on a very tiny screen...
  painter.setPen(textColor);
  painter.setBrush(textBackgroundColor);
  painter.drawRect(boundingRect);
  painter.drawText(textRect, txt);

  if ((r.height() > handleSize*2 && r.width() > handleSize*2)
    || !mouseDown)
  {
    updateHandles();
    painter.setPen(handleColor);
    handleColor.setAlpha(80);
    painter.setBrush(handleColor);
    painter.drawRects(handleMask().rects());
  }

  if (!mouseDown || !magnify)
  {
    return;
  }

  // Drawing the magnified version
  QPoint magEnd   = mousePos;
  magEnd   += QPoint(50, 50);

  QPoint magStart =  mousePos;
  magStart -= QPoint(50, 50);

  QRect newRect = QRect(magStart, magEnd);

  QPixmap magnified = pixmap.copy(newRect).scaled(QSize(200, 200));

  QPainter magPainter(&magnified);
  magPainter.setPen(QPen(QBrush(QColor(255, 0, 0, 180)), 2)); // Same border pen
  magPainter.drawRect(magnified.rect());
  magPainter.drawText(magnified.rect().center()-QPoint(4, -4), "+"); //Center minus the 4 pixels wide and across of the "+"

  QPoint drawPosition = selection.bottomRight();

  if ((drawPosition.x()+200) > pixmap.rect().width())
  drawPosition.setX(drawPosition.x()-200);

  if ((drawPosition.y()+200) > pixmap.rect().height())
  {
  drawPosition.setY(drawPosition.y()-200);

  if ((drawPosition.x()+400) > pixmap.rect().width())
    drawPosition.setY(drawPosition.y()-12);
  }

  painter.drawPixmap(drawPosition, magnified);
}

void AreaSelector::resizeEvent(QResizeEvent* e)
{
  Q_UNUSED(e);
  if (selection.isNull())
    return;
  QRect r = selection;
  r.setTopLeft(limitPointToRect(r.topLeft(), rect()));
  r.setBottomRight(limitPointToRect(r.bottomRight(), rect()));

  if (r.width() <= 1 || r.height() <= 1) //this just results in ugly drawing...
    r = QRect();

  selection = r;
}

void AreaSelector::mousePressEvent(QMouseEvent* e)
{
  showHelp = false;
  idleTimer.stop();
  acceptWidget->hide();
  if (e->button() == Qt::LeftButton)
  {
    mouseDown = true;
    dragStartPoint = e->pos();
    selectionBeforeDrag = selection;
    if (!selection.contains(e->pos()))
    {
      newSelection = true;
      selection = QRect();
      showHelp = true;
    }
    else
    {
      setCursor(Qt::ClosedHandCursor);
    }
  }
  else if (e->button() == Qt::RightButton
       || e->button() == Qt::MidButton)
  {
    cancel();
  }

  update();
}

void AreaSelector::mouseMoveEvent(QMouseEvent* e)
{
  if (mouseDown)
  {
    mousePos = e->pos();
    if (newSelection)
    {
      QRect r = rect();
      selection = QRect(dragStartPoint, limitPointToRect(mousePos, r)).normalized();
    }
    else if (mouseOverHandle == 0) // moving the whole selection
    {
      QRect r = rect().normalized(), s = selectionBeforeDrag.normalized();
      QPoint p = s.topLeft() + e->pos() - dragStartPoint;
      r.setBottomRight(r.bottomRight() - QPoint(s.width(), s.height()));

      if (!r.isNull() && r.isValid())
        selection.moveTo(limitPointToRect(p, r));
    }
    else // dragging a handle
    {
      QRect r = selectionBeforeDrag;
      QPoint offset = e->pos() - dragStartPoint;

      if (mouseOverHandle == &TLHandle || mouseOverHandle == &THandle
       || mouseOverHandle == &TRHandle) // dragging one of the top handles
      {
        r.setTop(r.top() + offset.y());
      }

      if (mouseOverHandle == &TLHandle || mouseOverHandle == &LHandle
       || mouseOverHandle == &BLHandle) // dragging one of the left handles
      {
        r.setLeft(r.left() + offset.x());
      }

      if (mouseOverHandle == &BLHandle || mouseOverHandle == &BHandle
       || mouseOverHandle == &BRHandle) // dragging one of the bottom handles
      {
        r.setBottom(r.bottom() + offset.y());
      }

      if (mouseOverHandle == &TRHandle || mouseOverHandle == &RHandle
       || mouseOverHandle == &BRHandle) // dragging one of the right handles
      {
        r.setRight(r.right() + offset.x());
      }
      r = r.normalized();
      r.setTopLeft(limitPointToRect(r.topLeft(), rect()));
      r.setBottomRight(limitPointToRect(r.bottomRight(), rect()));
      selection = r;
    }

    QPoint acceptPos = e->pos() + QPoint(5, 5);

    if ((acceptPos.x()+120) > pixmap.rect().width())
      acceptPos.setX(acceptPos.x()-120);

    if ((acceptPos.y()+70) > pixmap.rect().height())
      acceptPos.setY(acceptPos.y()-70);

    acceptWidget->move(acceptPos);
    update();
  }
  else
  {
    if (selection.isNull())
      return;
    bool found = false;
    foreach(QRect* r, handles)
    {
      if (r->contains(e->pos()))
      {
      mouseOverHandle = r;
      found = true;
        break;
      }
    }

    if (!found)
    {
      mouseOverHandle = 0;
      if (selection.contains(e->pos()))
        setCursor(Qt::OpenHandCursor);
      else if (QRect(acceptWidget->mapToParent(acceptWidget->pos()), QSize(100, 60)).contains(e->pos()))
        setCursor(Qt::PointingHandCursor);
      else
        setCursor(Qt::CrossCursor);
    }
    else
    {
      if (mouseOverHandle == &TLHandle || mouseOverHandle == &BRHandle)
        setCursor(Qt::SizeFDiagCursor);
      if (mouseOverHandle == &TRHandle || mouseOverHandle == &BLHandle)
        setCursor(Qt::SizeBDiagCursor);
      if (mouseOverHandle == &LHandle || mouseOverHandle == &RHandle)
        setCursor(Qt::SizeHorCursor);
      if (mouseOverHandle == &THandle || mouseOverHandle == &BHandle)
        setCursor(Qt::SizeVerCursor);
    }
  }
}

void AreaSelector::mouseReleaseEvent(QMouseEvent* e)
{
  if (!selection.isNull())
    acceptWidget->show();

  mouseDown = false;
  newSelection = false;
  idleTimer.start();
  if (mouseOverHandle == 0 && selection.contains(e->pos()))
    setCursor(Qt::OpenHandCursor);
  update();
}

void AreaSelector::mouseDoubleClickEvent(QMouseEvent*)
{
  grabRect();
}

void AreaSelector::keyPressEvent(QKeyEvent* e)
{
  if (e->key() == Qt::Key_Escape)
  {
    cancel();
  }
  else if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
  {
    grabRect();
  }
  else
  {
    e->ignore();
  }
}

void AreaSelector::grabRect()
{
  QRect r = selection.normalized();
  if (!r.isNull() && r.isValid())
  {
    grabbing = true;
    pixmap = pixmap.copy(r);
    accept();
  }
}

void AreaSelector::cancel()
{
  pixmap = QPixmap();
  reject();
}

void AreaSelector::animationTick(int frame)
{
  if (overlayAlpha != 80)
    overlayAlpha = frame;

  update();
}

void AreaSelector::updateHandles()
{
  QRect r = selection.normalized().adjusted(0, 0, -1, -1);
  int s2 = handleSize / 2;

  TLHandle.moveTopLeft(r.topLeft());
  TRHandle.moveTopRight(r.topRight());
  BLHandle.moveBottomLeft(r.bottomLeft());
  BRHandle.moveBottomRight(r.bottomRight());

  LHandle.moveTopLeft(QPoint(r.x(), r.y() + r.height() / 2 - s2));
  THandle.moveTopLeft(QPoint(r.x() + r.width() / 2 - s2, r.y()));
  RHandle.moveTopRight(QPoint(r.right(), r.y() + r.height() / 2 - s2));
  BHandle.moveBottomLeft(QPoint(r.x() + r.width() / 2 - s2, r.bottom()));
}

QRegion AreaSelector::handleMask() const
{
  // note: not normalized QRects are bad here, since they will not be drawn
  QRegion mask;
  foreach(QRect* rect, handles) mask += QRegion(*rect);
  return mask;
}

QPoint AreaSelector::limitPointToRect(const QPoint &p, const QRect &r) const
{
  QPoint q;
  q.setX(p.x() < r.x() ? r.x() : p.x() < r.right() ? p.x() : r.right());
  q.setY(p.y() < r.y() ? r.y() : p.y() < r.bottom() ? p.y() : r.bottom());
  return q;
}
