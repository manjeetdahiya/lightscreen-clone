#include "previewdialog.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>
#include <QTimeLine>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMouseEvent>
#include <QHttp>
#include <QProgressBar>
#include <QBuffer>
#include <QUrl>

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
  setCursor(Qt::PointingHandCursor);

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
  setFixedSize(this->size());

  QPoint position = QApplication::desktop()->availableGeometry().bottomRight();
  position.setX(position.x() - size.width() - 15); //TODO: Get border size

  move(position);

  //TODO: It should be possible to disable this.
  QTimeLine *effectTimeLine = new QTimeLine(400, this);
  effectTimeLine->setFrameRange(position.y(), position.y() - size.height() - 35); //TODO: Get taskbar size

  connect(effectTimeLine, SIGNAL(frameChanged(int)), this, SLOT(effect(int)));
  connect(&deathTimer, SIGNAL(timeout()), this, SLOT(kill()));

  connect(this, SIGNAL(result(bool, QString)), parent, SLOT(screenshotCleanup(bool, QString)));

  QPushButton *confirmButton = new QPushButton("", this);
  confirmButton->setIcon(QIcon(":/icons/Good"));
  confirmButton->setIconSize(QSize(22, 22));
  confirmButton->setCursor(Qt::ArrowCursor);
  connect(confirmButton, SIGNAL(clicked()), this, SLOT(confirm()));

  QPushButton *discardButton = new QPushButton("", this);
  discardButton->setIcon(QIcon(":/icons/Bad"));
  discardButton->setIconSize(QSize(22, 22));
  discardButton->setCursor(Qt::ArrowCursor);
  connect(discardButton, SIGNAL(clicked()), this, SLOT(kill()));

  mButtonLayout = new QHBoxLayout;
  mButtonLayout->addWidget(confirmButton);
  mButtonLayout->addStretch();
  mButtonLayout->addWidget(discardButton);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addStretch();
  layout->addLayout(mButtonLayout);
  layout->setMargin(0);
  layout->setContentsMargins(2, 2, 2, 2);
  setLayout(layout);

  show();
  deathTimer.start(5000);
  effectTimeLine->start();
}

void PreviewDialog::kill()
{
  QString fileName = "";

  if (mConfirmed)
  {
    if (mScreenshot.options().file)
      fileName = mScreenshot.save();

    emit result(!mScreenshot.pixmap().isNull(), fileName);
  }
  else
  {
    emit result(false, fileName);
  }

  deleteLater();
  close();
}

void PreviewDialog::confirm()
{
  mConfirmed = true;
  deathTimer.stop();

  if (mScreenshot.options().screenshare)
  {
    mHttp = new QHttp(QUrl::toPercentEncoding("http://imageshack.us/"), 80, this);
    connect(mHttp, SIGNAL(requestFinished(int, bool)), this, SLOT(screenshareFinished(int, bool)));

    QByteArray screenshot;
    QBuffer buffer(&screenshot);
    buffer.open(QIODevice::WriteOnly);
    mScreenshot.pixmap().save(&buffer, "PNG");

    setWindowTitle(QString("Post: %1").at(mHttp->post("/", screenshot)));
  }
  else
  {
    kill();
  }
}

void PreviewDialog::screenshareFinished(int id, bool error)
{
  if (!error)
    setWindowTitle(tr("Finished, %1").at(id));
  else
    setWindowTitle("Error!");
}

bool PreviewDialog::event(QEvent *event)
{
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
