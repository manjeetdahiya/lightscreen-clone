
#include <QApplication>
#include <QWidget>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QRubberBand>

#include "windowpicker.h"
#include "os.h"
#include <windows.h>

WindowPicker::WindowPicker() : QWidget(0), mWindowLabel(0), mIndicator(0)
{
  setWindowFlags(Qt::SplashScreen | Qt::WindowStaysOnTopHint);
  setFocusPolicy(Qt::StrongFocus);
  setFocus(Qt::MouseFocusReason);

  setWindowTitle(tr("Lightscreen Window Picker"));

  QLabel *helpLabel = new QLabel(tr("Pick a window and click on it to capture it.\nPress the right mouse button to cancel."), this);

  mWindowLabel = new QLabel(tr(" - No window selected"));
  mWindowLabel->setStyleSheet("font-weight: bold");

  QVBoxLayout *fl = new QVBoxLayout(this);
  fl->addWidget(helpLabel);
  fl->addWidget(mWindowLabel);
  fl->setMargin(0);

  QFrame *frame = new QFrame(this);
  frame->setObjectName("frame");
  frame->setLayout(fl);

  QVBoxLayout *l = new QVBoxLayout(this);
  l->setMargin(0);
  l->addWidget(frame);

  setLayout(l);

  setStyleSheet("#frame { padding: 5px; border: 1px solid #898c95; background-color: qlineargradient(spread:pad, x1:1, y1:1, x2:0.988636, y2:0.608, stop:0 rgba(235, 235, 235, 255), stop:1 rgba(255, 255, 255, 255)); }");

  resize(sizeHint());

  move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(QCursor::pos())).center()-QPoint(width()/2, height()/2));
  show();

  setMouseTracking(true);
  grabMouse(Qt::CrossCursor);
}

void WindowPicker::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
  {
     hide();

     POINT mousePos;
     mousePos.x = event->globalX();
     mousePos.y = event->globalY();
     HWND window = GetAncestor(WindowFromPoint(mousePos), GA_ROOT);

     qDebug() << window;
     emit pixmap(os::grabWindow(window));
  }

  close();
}

void WindowPicker::mouseMoveEvent(QMouseEvent *event)
{
  POINT mousePos;
  mousePos.x = event->globalX();
  mousePos.y = event->globalY();
  static HWND window;

  HWND cWindow = GetAncestor(WindowFromPoint(mousePos), GA_ROOT);

  if (window == cWindow)
    return;

  window = cWindow;

  // Text (TODO: Icon?)
  WCHAR str[40];
  GetWindowText(window, str, 40);

  QString windowText = QString(" - %1").arg(QString::fromWCharArray(str));
  mWindowLabel->setText(windowText);

  if (windowText.length() == 42)
    mWindowLabel->setText(windowText + "...");
}

void WindowPicker::closeEvent(QCloseEvent*)
{
  emit pixmap(QPixmap());
  releaseMouse();
  deleteLater();
}

void WindowPicker::focusOutEvent(QFocusEvent*)
{
  qDebug() << "WindowPicker::focusOutEvent";
  close();
}
