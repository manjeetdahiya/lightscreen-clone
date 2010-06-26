
#include <QApplication>
#include <QWidget>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QRubberBand>
#include <QPushButton>

#include "windowpicker.h"
#include "os.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif


WindowPicker::WindowPicker() : QWidget(0), mCrosshair(":/icons/picker"), mWindowLabel(0), mTaken(false)
{
  setWindowFlags(Qt::SplashScreen | Qt::WindowStaysOnTopHint);
  setFocusPolicy(Qt::StrongFocus);
  setFocus(Qt::MouseFocusReason);

  setWindowTitle(tr("Lightscreen Window Picker"));

  QLabel *helpLabel = new QLabel(tr("Grab the picker by clicking and holding down the mouse button,\nthen drag it to the window of your choice and release it to capture."), this);

  mWindowIcon = new QLabel(this);
  mWindowIcon->setMinimumSize(22, 22);
  mWindowIcon->setMaximumSize(22, 22);
  mWindowIcon->setScaledContents(true);

  mWindowLabel = new QLabel(tr(" - Start dragging to select windows"), this);
  mWindowLabel->setStyleSheet("font-weight: bold");

  mCrosshairLabel = new QLabel(this);
  mCrosshairLabel->setAlignment(Qt::AlignHCenter);
  mCrosshairLabel->setPixmap(mCrosshair);

  QPushButton *closeButton = new QPushButton(tr("Close"));
  connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

  QHBoxLayout *lw = new QHBoxLayout;
  lw->addWidget(mWindowIcon);
  lw->addWidget(mWindowLabel);

  QHBoxLayout *lb = new QHBoxLayout;
  lb->addStretch(0);
  lb->addWidget(closeButton);

  QVBoxLayout *fl = new QVBoxLayout;
  fl->addWidget(helpLabel);
  fl->addLayout(lw);
  fl->addWidget(mCrosshairLabel);
  fl->addLayout(lb);
  fl->setMargin(0);

  QFrame *frame = new QFrame(this);
  frame->setObjectName("frame");
  frame->setLayout(fl);

  QVBoxLayout *l = new QVBoxLayout;
  l->setMargin(0);
  l->addWidget(frame);

  setLayout(l);

  setStyleSheet("#frame { padding: 7px 10px; border: 1px solid #898c95; background-color: qlineargradient(spread:pad, x1:1, y1:1, x2:0.988636, y2:0.608, stop:0 rgba(235, 235, 235, 255), stop:1 rgba(255, 255, 255, 255)); }");

  resize(sizeHint());

  move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(QCursor::pos())).center()-QPoint(width()/2, height()/2));
  show();
}

void WindowPicker::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
  {
     POINT mousePos;
     mousePos.x = event->globalX();
     mousePos.y = event->globalY();
     HWND window = GetAncestor(WindowFromPoint(mousePos), GA_ROOT);

     if (window == winId()) {
       // Cancelling
       mWindowIcon->setPixmap(QPixmap());
       mCrosshairLabel->setPixmap(mCrosshair);
       qApp->restoreOverrideCursor();
       return;
     }

     mTaken = true;

     qDebug() << window;
     emit pixmap(os::grabWindow(window));
  }


  close();
}

void WindowPicker::mousePressEvent(QMouseEvent *event)
{
    qApp->setOverrideCursor(QCursor(mCrosshair));
    mCrosshairLabel->setPixmap(QPixmap());
    QWidget::mousePressEvent(event);
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

  if (window == winId()) {
    mWindowIcon->setPixmap(QPixmap());
    mWindowLabel->setText("");
  }
  else {
    // Text (TODO: Application Icon?)
    WCHAR str[40];
    HICON icon;

    GetWindowText(window, str, 45);

    // Retrieving the application icon
    icon = (HICON)GetClassLong(window, GCL_HICON);

    if (icon != NULL) {
      mWindowIcon->setPixmap(QPixmap::fromWinHICON(icon));
    }
    else {
      mWindowIcon->setPixmap(QPixmap());
    }

    QString windowText = QString(" - %1").arg(QString::fromWCharArray(str));

    if (windowText == " - ") {
      mWindowLabel->setText("");
      return;
    }

    if (windowText.length() == 47) {
      mWindowLabel->setText(windowText + "...");
    }
    else {
      mWindowLabel->setText(windowText);
    }

  }
}

void WindowPicker::closeEvent(QCloseEvent*)
{
  if (!mTaken)
    emit pixmap(QPixmap());

  qApp->restoreOverrideCursor();
  deleteLater();
}

void WindowPicker::focusOutEvent(QFocusEvent*)
{
  qDebug() << "WindowPicker::focusOutEvent";
  close();
}
