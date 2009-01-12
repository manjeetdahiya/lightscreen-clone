#include "previewdialog.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>
#include <QTimeLine>
#include <QHBoxLayout>
#include <QPushButton>
#include "../tools/screenshot.h"

PreviewDialog::PreviewDialog(QWidget* parent, Screenshot::Options options) :
    QDialog(parent), mConfirmed(false)
{
  mScreenshot.setOptions(options);

  if (!mScreenshot.take())
  {
    emit result(false, "");
    return;
  }

  setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
  setWindowTitle("Screenshot Preview");
  setMouseTracking(true);

  QSize size = mScreenshot.pixmap().size();

  if (!(size.width() < 200 && size.height() < 200))
  { // Scale the image when it's witdth or height exceed 200
    size.scale(200, 200, Qt::KeepAspectRatio);
  }

  // Set our pixmap as the background of the widget.
  QPalette newPalette = palette();
  newPalette.setBrush(backgroundRole(), QBrush(mScreenshot.pixmap().scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
  setPalette(newPalette);

  resize(size);

  QPoint position = QApplication::desktop()->availableGeometry().bottomRight();
  position.setX(position.x() - size.width() - 15); //TODO: Get border size

  move(position);

  //TODO: It should be possible to disable this.
  QTimeLine *effectTimeLine = new QTimeLine(400, this);
  effectTimeLine->setFrameRange(position.y(), position.y() - size.height() - 35); //TODO: Get taskbar size

  connect(effectTimeLine, SIGNAL(frameChanged(int)), this, SLOT(effect(int)));
  connect(&deathTimer, SIGNAL(timeout()), this, SLOT(kill()));

  connect(this, SIGNAL(result(bool, QString)), parent, SLOT(screenshotCleanup(bool, QString)));

  QPushButton *confirmButton = new QPushButton(tr("Confirm"));
  connect(confirmButton, SIGNAL(clicked()), this, SLOT(confirm()));

  QPushButton *discardButton = new QPushButton(tr("Discard"));
  connect(discardButton, SIGNAL(clicked()), this, SLOT(kill()));

  QHBoxLayout *buttonLayout = new QHBoxLayout;
  buttonLayout->addWidget(confirmButton);
  buttonLayout->addStretch();
  buttonLayout->addWidget(discardButton);


  QVBoxLayout *layout = new QVBoxLayout;
  layout->addStretch();
  layout->addLayout(buttonLayout);
  layout->setMargin(0);
  layout->setContentsMargins(2, 2, 2, 2);
  setLayout(layout);

  show();
  deathTimer.start(5000);
  effectTimeLine->start();
}

void PreviewDialog::kill()
{
  if (mConfirmed)
  {
    QString fileName = mScreenshot.save();
    emit result(!mScreenshot.pixmap().isNull(), fileName);
  }
  else
  {
    emit result(false, "");
  }

  deleteLater();
  close();
}

void PreviewDialog::confirm()
{
  mConfirmed = true;
  kill();
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
    deathTimer.setInterval(3000);
  }

  return QDialog::event(event);
}

void PreviewDialog::effect(int frame)
{
  move(pos().x(), frame);
}
