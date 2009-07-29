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
  QDialog(parent), pixmap(pixmap),  mMagnify(magnify), mSelection(), mMouseDown(false), mNewSelection(false),
  mHandleSize(10), mMouseOverHandle(0), mIdleTimer(),
  mShowHelp(true), mGrabbing(false), mOverlayAlpha(1),
  mTLHandle(0, 0, mHandleSize, mHandleSize), mTRHandle(0, 0, mHandleSize, mHandleSize),
  mBLHandle(0, 0, mHandleSize, mHandleSize), mBRHandle(0, 0, mHandleSize, mHandleSize),
  mLHandle(0, 0, mHandleSize, mHandleSize), mTHandle(0, 0, mHandleSize, mHandleSize),
  mRHandle(0, 0, mHandleSize, mHandleSize), mBHandle(0, 0, mHandleSize, mHandleSize)
{
  mHandles << &mTLHandle << &mTRHandle << &mBLHandle << &mBRHandle
      << &mLHandle << &mTHandle << &mRHandle << &mBHandle;
  setMouseTracking(true);
  setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
  showFullScreen();
  resize(pixmap.size());
  move(0, 0);
  setCursor(Qt::CrossCursor);
  connect(&mIdleTimer, SIGNAL(timeout()), this, SLOT(displayHelp()));
  mIdleTimer.start(3000);
  connect(&mAnimationTimeLine, SIGNAL(frameChanged(int)), this, SLOT(animationTick(int)));
  mAnimationTimeLine.setFrameRange(0, 80);
  mAnimationTimeLine.setDuration(500); // Dont use the overlayAlpha, instead use frames
  mAnimationTimeLine.start();

  // Creating accept widget:
  mAcceptWidget = new QWidget(this);
  mAcceptWidget->resize(110, 60);
  mAcceptWidget->setWindowOpacity(0.4);
  mAcceptWidget->setStyleSheet("QWidget { background: url(:/icons/AreaBackground); } QPushButton { background: transparent; border: none; height: 50px; color: white; padding: 0; } QPushButton:hover { image: url(:/icons/AreaHover) }");

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

  mAcceptWidget->setLayout(awLayout);
}

AreaSelector::~AreaSelector()
{
}

void AreaSelector::displayHelp()
{
  mShowHelp = true;
  update();
}

void AreaSelector::paintEvent(QPaintEvent* e)
{
  Q_UNUSED(e);

  if (mGrabbing) // grabWindow() should just get the background
    return;

  QPainter painter(this);

  QPalette pal = palette();
  QFont font   = QToolTip::font();

  QColor handleColor(255, 0, 0, 180);
  QColor overlayColor(0, 0, 0, mOverlayAlpha);
  QColor textColor = pal.color(QPalette::Active, QPalette::Text);
  QColor textBackgroundColor = pal.color(QPalette::Active, QPalette::Base);
  painter.drawPixmap(0, 0, pixmap);
  painter.setFont(font);

  QRect r = mSelection.normalized().adjusted(0, 0, -1, -1);

  QRegion grey(rect());
  grey = grey.subtracted(r);
  painter.setPen(handleColor);
  painter.setBrush(overlayColor);
  painter.setClipRegion(grey);
  painter.drawRect(-1, -1, rect().width() + 1, rect().height() + 1);
  painter.setClipRect(rect());
  painter.setBrush(Qt::NoBrush);
  painter.drawRect(r);

  if (mShowHelp && mOverlayAlpha == 80)
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

  if (mSelection.isNull())
  {
    return;
  }

  // The grabbed region is everything which is covered by the drawn
  // rectangles (border included). This means that there is no 0px
  // selection, since a 0px wide rectangle will always be drawn as a line.
  QString txt = QString("%1x%2").arg(mSelection.width() == 0 ? 2 : mSelection.width())
      .arg(mSelection.height() == 0 ? 2 : mSelection.height());
  QRect textRect = painter.boundingRect(rect(), Qt::AlignLeft, txt);
  QRect boundingRect = textRect.adjusted(-4, 0, 0, 0);

  if (textRect.width() < r.width() - 2*mHandleSize &&
     textRect.height() < r.height() - 2*mHandleSize &&
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

  if ((r.height() > mHandleSize*2 && r.width() > mHandleSize*2)
    || !mMouseDown)
  {
    updateHandles();
    painter.setPen(handleColor);
    handleColor.setAlpha(80);
    painter.setBrush(handleColor);
    painter.drawRects(handleMask().rects());
  }

  if (!mMouseDown || !mMagnify)
  {
    return;
  }

  // Drawing the magnified version
  QPoint magEnd   = mMousePos;
  magEnd   += QPoint(50, 50);

  QPoint magStart =  mMousePos;
  magStart -= QPoint(50, 50);

  QRect newRect = QRect(magStart, magEnd);

  QPixmap magnified = pixmap.copy(newRect).scaled(QSize(200, 200));

  QPainter magPainter(&magnified);
  magPainter.setPen(QPen(QBrush(QColor(255, 0, 0)), 2)); // Same border pen
  magPainter.drawRect(magnified.rect());
  magPainter.drawText(magnified.rect().center()-QPoint(4, -4), "+"); //Center minus the 4 pixels wide and across of the "+"

  QPoint drawPosition = mSelection.bottomRight();

  if ((drawPosition.x()+200) > pixmap.rect().width())
    drawPosition.setX(drawPosition.x()-200);

  if ((drawPosition.y()+200) > pixmap.rect().height())
    drawPosition.setY(drawPosition.y()-200);

  if (drawPosition.y() == mSelection.bottomRight().y()-200
   && drawPosition.x() == mSelection.bottomRight().x()-200)
    painter.setCompositionMode(QPainter::CompositionMode_Overlay);

  painter.drawPixmap(drawPosition, magnified);
}

void AreaSelector::resizeEvent(QResizeEvent* e)
{
  Q_UNUSED(e);

  if (mSelection.isNull())
    return;

  QRect r = mSelection;
  r.setTopLeft(limitPointToRect(r.topLeft(), rect()));
  r.setBottomRight(limitPointToRect(r.bottomRight(), rect()));

  if (r.width() <= 1 || r.height() <= 1) //this just results in ugly drawing...
    r = QRect();

  mSelection = r;
}

void AreaSelector::mousePressEvent(QMouseEvent* e)
{
  mShowHelp = false;
  mIdleTimer.stop();
  mAcceptWidget->hide();
  if (e->button() == Qt::LeftButton)
  {
    mMouseDown = true;
    mDragStartPoint = e->pos();
    mSelectionBeforeDrag = mSelection;
    if (!mSelection.contains(e->pos()))
    {
      mNewSelection = true;
      mSelection = QRect();
      mShowHelp = true;
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
  if (mMouseDown)
  {
    mMousePos = e->pos();
    if (mNewSelection)
    {
      QRect r = rect();
      mSelection = QRect(mDragStartPoint, limitPointToRect(mMousePos, r)).normalized();
    }
    else if (mMouseOverHandle == 0) // moving the whole selection
    {
      QRect r = rect().normalized(), s = mSelectionBeforeDrag.normalized();
      QPoint p = s.topLeft() + e->pos() - mDragStartPoint;
      r.setBottomRight(r.bottomRight() - QPoint(s.width(), s.height()));

      if (!r.isNull() && r.isValid())
        mSelection.moveTo(limitPointToRect(p, r));
    }
    else // dragging a handle
    {
      QRect r = mSelectionBeforeDrag;
      QPoint offset = e->pos() - mDragStartPoint;

      if (mMouseOverHandle == &mTLHandle || mMouseOverHandle == &mTHandle
       || mMouseOverHandle == &mTRHandle) // dragging one of the top handles
      {
        r.setTop(r.top() + offset.y());
      }

      if (mMouseOverHandle == &mTLHandle || mMouseOverHandle == &mLHandle
       || mMouseOverHandle == &mBLHandle) // dragging one of the left handles
      {
        r.setLeft(r.left() + offset.x());
      }

      if (mMouseOverHandle == &mBLHandle || mMouseOverHandle == &mBHandle
       || mMouseOverHandle == &mBRHandle) // dragging one of the bottom handles
      {
        r.setBottom(r.bottom() + offset.y());
      }

      if (mMouseOverHandle == &mTRHandle || mMouseOverHandle == &mRHandle
       || mMouseOverHandle == &mBRHandle) // dragging one of the right handles
      {
        r.setRight(r.right() + offset.x());
      }

      r = r.normalized();
      r.setTopLeft(limitPointToRect(r.topLeft(), rect()));
      r.setBottomRight(limitPointToRect(r.bottomRight(), rect()));
      mSelection = r;
    }

    QPoint acceptPos = e->pos() + QPoint(5, 5);

    if ((acceptPos.x()+120) > pixmap.rect().width())
      acceptPos.setX(acceptPos.x()-120);

    if ((acceptPos.y()+70) > pixmap.rect().height())
      acceptPos.setY(acceptPos.y()-70);

    mAcceptWidget->move(acceptPos);
    update();
  }
  else
  {
    if (mSelection.isNull())
      return;
    bool found = false;
    foreach(QRect* r, mHandles)
    {
      if (r->contains(e->pos()))
      {
      mMouseOverHandle = r;
      found = true;
        break;
      }
    }

    if (!found)
    {
      mMouseOverHandle = 0;
      if (mSelection.contains(e->pos()))
        setCursor(Qt::OpenHandCursor);
      else if (QRect(mAcceptWidget->mapToParent(mAcceptWidget->pos()), QSize(100, 60)).contains(e->pos()))
        setCursor(Qt::PointingHandCursor);
      else
        setCursor(Qt::CrossCursor);
    }
    else
    {
      if (mMouseOverHandle == &mTLHandle || mMouseOverHandle == &mBRHandle)
        setCursor(Qt::SizeFDiagCursor);
      if (mMouseOverHandle == &mTRHandle || mMouseOverHandle == &mBLHandle)
        setCursor(Qt::SizeBDiagCursor);
      if (mMouseOverHandle == &mLHandle || mMouseOverHandle == &mRHandle)
        setCursor(Qt::SizeHorCursor);
      if (mMouseOverHandle == &mTHandle || mMouseOverHandle == &mBHandle)
        setCursor(Qt::SizeVerCursor);
    }
  }
}

void AreaSelector::mouseReleaseEvent(QMouseEvent* e)
{
  if (!mSelection.isNull())
    mAcceptWidget->show();

  mMouseDown = false;
  mNewSelection = false;
  mIdleTimer.start();
  if (mMouseOverHandle == 0 && mSelection.contains(e->pos()))
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
  QRect r = mSelection.normalized();
  if (!r.isNull() && r.isValid())
  {
    mGrabbing = true;
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
  if (mOverlayAlpha != 80)
    mOverlayAlpha = frame;

  update();
}

void AreaSelector::updateHandles()
{
  QRect r = mSelection.normalized().adjusted(0, 0, -1, -1);
  int s2 = mHandleSize / 2;

  mTLHandle.moveTopLeft(r.topLeft());
  mTRHandle.moveTopRight(r.topRight());
  mBLHandle.moveBottomLeft(r.bottomLeft());
  mBRHandle.moveBottomRight(r.bottomRight());

  mLHandle.moveTopLeft(QPoint(r.x(), r.y() + r.height() / 2 - s2));
  mTHandle.moveTopLeft(QPoint(r.x() + r.width() / 2 - s2, r.y()));
  mRHandle.moveTopRight(QPoint(r.right(), r.y() + r.height() / 2 - s2));
  mBHandle.moveBottomLeft(QPoint(r.x() + r.width() / 2 - s2, r.bottom()));
}

QRegion AreaSelector::handleMask() const
{
  // note: not normalized QRects are bad here, since they will not be drawn
  QRegion mask;
  foreach(QRect* rect, mHandles) mask += QRegion(*rect);
  return mask;
}

QPoint AreaSelector::limitPointToRect(const QPoint &p, const QRect &r) const
{
  QPoint q;
  q.setX(p.x() < r.x() ? r.x() : p.x() < r.right() ? p.x() : r.right());
  q.setY(p.y() < r.y() ? r.y() : p.y() < r.bottom() ? p.y() : r.bottom());
  return q;
}
