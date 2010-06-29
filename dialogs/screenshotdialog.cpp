#include "screenshotdialog.h"
#include "../tools/screenshot.h"


#include <QApplication>
#include <QDesktopWidget>
#include <QScrollArea>
#include <QScrollBar>
#include <QLabel>
#include <QHBoxLayout>
#include <QMouseEvent>

#include <QDebug>

ScreenshotDialog::ScreenshotDialog(Screenshot *screenshot, QWidget *parent) :
    QDialog(parent)
{
  setWindowTitle(tr("Lightscreen Screenshot Viewer"));

  mScrollArea = new QScrollArea;
  mScrollArea->verticalScrollBar()->installEventFilter(this);

  mLabel = new QLabel;

  QHBoxLayout *layout = new QHBoxLayout;
  mLabel->setPixmap(screenshot->pixmap());
  mLabel->setScaledContents(true);

  mScrollArea->setWidget(mLabel);

  layout->addWidget(mScrollArea);
  layout->setMargin(0);

  setLayout(layout);
  setCursor(Qt::OpenHandCursor);

  QSize size = screenshot->pixmap().size();

  if (size.width() >= qApp->desktop()->size().width()) {
    size.setWidth(qApp->desktop()->availableGeometry().size().width() - 300);
  }

  if (size.height() >= qApp->desktop()->size().height()) {
    size.setHeight(qApp->desktop()->availableGeometry().size().height() - 300);
  }

  resize(size);

  show();
}

void ScreenshotDialog::mousePressEvent(QMouseEvent *event)
{
  mMousePos = event->pos();
  setCursor(Qt::ClosedHandCursor);
}

void ScreenshotDialog::mouseReleaseEvent(QMouseEvent *event)
{
  setCursor(Qt::OpenHandCursor);
}

void ScreenshotDialog::mouseMoveEvent(QMouseEvent *event)
{
  QPoint diff = event->pos() - mMousePos;
  mMousePos = event->pos();

  mScrollArea->verticalScrollBar()->setValue(mScrollArea->verticalScrollBar()->value() - diff.y());
  mScrollArea->horizontalScrollBar()->setValue(mScrollArea->horizontalScrollBar()->value() - diff.x());
}

void ScreenshotDialog::closeEvent(QCloseEvent *event)
{
  Q_UNUSED(event)
  deleteLater();
}

bool ScreenshotDialog::eventFilter(QObject *obj, QEvent *event)
{
  if (event->type() == QEvent::Wheel
   && qApp->keyboardModifiers() & Qt::ControlModifier) {
    QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);

    if (wheelEvent->delta() > 0) {
      mLabel->resize(mLabel->size()+QSize(100, 100));
    }
    else {
      mLabel->resize(mLabel->size()-QSize(100, 100));
    }

    return true;
  }

  return QObject::eventFilter(obj, event);
}

void ScreenshotDialog::wheelEvent(QWheelEvent *event)
{
  qDebug() << "ScreenshotDialog::wheelEvent(delta: " << event->delta() << ")";



  event->accept();
}
