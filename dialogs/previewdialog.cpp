#include "previewdialog.h"
#include "screenshotdialog.h"
#include "../tools/screenshot.h"
#include "../tools/screenshotmanager.h"
#include "../tools/os.h"

#include <QApplication>
#include <QObject>
#include <QList>
#include <QHBoxLayout>
#include <QIcon>
#include <QPushButton>
#include <QPalette>
#include <QDesktopWidget>
#include <QLabel>
#include <QStackedLayout>
#include <QSettings>

#include <QDebug>

PreviewDialog::PreviewDialog(QWidget *parent) :
    QDialog(parent), mAutoclose(0)
{
  setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
  os::aeroGlass(this);
  setWindowTitle("Screenshot Preview");

  QSettings settings;
  mSize = settings.value("options/previewSize", 300).toInt();
  mPosition  = settings.value("options/previewPosition", 3).toInt();

  if (settings.value("options/previewAutoclose", false).toBool()) {
    mAutoclose = settings.value("options/previewAutocloseTime").toInt();
    mAutocloseReset = mAutoclose;
  }

  QHBoxLayout *l = new QHBoxLayout;
  mStack = new QStackedLayout;
  connect(mStack, SIGNAL(currentChanged(int)), this, SLOT(indexChanged(int)));

  mPrevButton = new QPushButton(QIcon(":/icons/arrowLeft"), "");
  connect(mPrevButton, SIGNAL(clicked()), this, SLOT(previous()));

  mNextButton = new QPushButton(QIcon(":/icons/arrowRight"), "");
  connect(mNextButton, SIGNAL(clicked()), this, SLOT(next()));

  mPrevButton->setFlat(true);
  mPrevButton->setIconSize(QSize(24, 24));

  mNextButton->setFlat(true);
  mNextButton->setIconSize(QSize(24, 24));

  mNextButton->setVisible(false);
  mPrevButton->setVisible(false);

  l->addWidget(mPrevButton);
  l->addLayout(mStack);
  l->addWidget(mNextButton);

  l->setMargin(0);
  mStack->setMargin(0);

  setMaximumHeight(mSize);
  setLayout(l);

  if (mAutoclose) {
    startTimer(1000);
  }
}

PreviewDialog::~PreviewDialog()
{
  qDebug() << "PreviewDialog destroyed";
}

void PreviewDialog::add(Screenshot *screenshot)
{
  if (!isVisible())
    show();

  if (mAutoclose) {
    mAutoclose = mAutocloseReset;
  }

  QLabel *widget = new QLabel(this);
  bool small = false;

  connect(widget, SIGNAL(destroyed()), screenshot, SLOT(discard()));

  QSize size = screenshot->pixmap().size();

  if (size.width() > mSize || size.height() > mSize)
  {
    size.scale(mSize, mSize, Qt::KeepAspectRatio);
  }
  else {
    small = true;
  }

  QPixmap thumbnail = screenshot->pixmap().scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

  widget->setPixmap(thumbnail);
  widget->setAlignment(Qt::AlignCenter);

  if (size.height() < 80)
  {
    size.setHeight(80);
  }

  if (size.width() < 80)
  {
    size.setWidth(80);
  }

  widget->setMinimumSize(size);
  widget->setMaximumSize(size);
  widget->resize(size);

  QPushButton *confirmPushButton = new QPushButton(QIcon(":/icons/Good")  , "", widget);
  QPushButton *discardPushButton = new QPushButton(QIcon(":/icons/Bad")   , "", widget);
  QPushButton *enlargePushButton = new QPushButton(QIcon(":/icons/zoomIn"), "", widget);

  confirmPushButton->setFlat(true);
  confirmPushButton->setIconSize(QSize(24, 24));

  discardPushButton->setFlat(true);
  discardPushButton->setIconSize(QSize(24, 24));

  enlargePushButton->setFlat(true);
  enlargePushButton->setIconSize(QSize(22, 22));
  enlargePushButton->setDisabled(small);

  connect(this, SIGNAL(acceptAll()), confirmPushButton, SLOT(click()));
  connect(confirmPushButton, SIGNAL(clicked()), screenshot, SLOT(confirm()));
  connect(confirmPushButton, SIGNAL(clicked()), this, SLOT(closePreview()));

  connect(discardPushButton, SIGNAL(clicked()), screenshot, SLOT(discard()));
  connect(discardPushButton, SIGNAL(clicked()), this, SLOT(closePreview()));

  connect(enlargePushButton, SIGNAL(clicked()), this, SLOT(enlargePreview()));

  QHBoxLayout *wlayout = new QHBoxLayout;
  wlayout->addWidget(confirmPushButton);
  wlayout->addStretch();
  wlayout->addWidget(enlargePushButton);
  wlayout->addStretch();
  wlayout->addWidget(discardPushButton);
  wlayout->setMargin(0);

  QVBoxLayout *wl = new QVBoxLayout;
  wl->addStretch();
  wl->addLayout(wlayout);
  wl->setMargin(0);

  widget->setLayout(wl);

  mStack->addWidget(widget);
  mStack->setCurrentIndex(mStack->count()-1);

  mNextButton->setEnabled(false);

  if (mStack->count() >= 2 && !mNextButton->isVisible())
  {
    mNextButton->setVisible(true);
    mPrevButton->setVisible(true);
  }

  relocate();
}

int PreviewDialog::count()
{
  return mStack->count();
}

void PreviewDialog::relocate()
{
  updateGeometry();
  resize(sizeHint());

  QPoint where;
  switch (mPosition) {
  case 0:
    where = QApplication::desktop()->availableGeometry(this).topLeft();
    break;
  case 1:
    where = QApplication::desktop()->availableGeometry(this).topRight();
    where.setX(where.x() - frameGeometry().width());
    break;
  case 2:
    where = QApplication::desktop()->availableGeometry(this).bottomLeft();
    where.setY(where.y() - frameGeometry().height());
    break;
  case 3:
  default:
    where = QApplication::desktop()->availableGeometry(this).bottomRight();
    where.setX(where.x() - frameGeometry().width());
    where.setY(where.y() - frameGeometry().height());
    break;
  }

  move(where);
}

void PreviewDialog::closePreview()
{
  QLabel *widget = qobject_cast<QLabel*>(sender()->parent());
  mStack->removeWidget(widget);
  widget->hide();
  widget->deleteLater();

  qDebug() << "Stack count" << mStack->count();

  if (mStack->count() == 0)
  {
    close();
  }
  else
  {
    relocate();
  }
}

void PreviewDialog::indexChanged(int i)
{
  if (i == mStack->count()-1)
  {
    mNextButton->setEnabled(false);
    mPrevButton->setEnabled(true);
  }

  if (i == 0 && mStack->count() > 1)
  {
    mNextButton->setEnabled(true);
    mPrevButton->setEnabled(false);
  }

  if (i != 0 && i != mStack->count()-1)
  {
    mNextButton->setEnabled(true);
    mPrevButton->setEnabled(true);
  }

  if (mStack->count() < 2)
  {
    mNextButton->setEnabled(false);
    mPrevButton->setEnabled(false);
  }

  if (mStack->widget(i))
    mStack->widget(i)->setFocus();
}

void PreviewDialog::previous()
{
  mStack->setCurrentIndex(mStack->currentIndex()-1);
}

void PreviewDialog::next()
{
  mStack->setCurrentIndex(mStack->currentIndex()+1);
}

void PreviewDialog::enlargePreview()
{
  Screenshot *screenshot = qobject_cast<Screenshot*>(ScreenshotManager::instance()->children().at(mStack->currentIndex()));

  if (screenshot == 0) {
    return;
  }

  if (screenshot->pixmap().size().height() < mSize || screenshot->pixmap().size().width() < mSize) {
    return;
  }

  new ScreenshotDialog(screenshot);
}

void PreviewDialog::closeEvent(QCloseEvent *event)
{
  mInstance = 0;
  deleteLater();
}

void PreviewDialog::mouseDoubleClickEvent(QMouseEvent *event)
{
  Q_UNUSED(event)
  enlargePreview();
}

void PreviewDialog::timerEvent(QTimerEvent *event)
{
  if (mAutoclose == 0) {
    emit acceptAll();
  }
  else if (mAutoclose < 0) {
    killTimer(event->timerId());
  }
  else {
    setWindowTitle(tr("Screenshot Preview: Closing in %1").arg(mAutoclose));
    mAutoclose--;
  }
}

// Singleton

PreviewDialog* PreviewDialog::mInstance = 0;

PreviewDialog *PreviewDialog::instance()
{
  if (!mInstance)
    mInstance = new PreviewDialog(0);

  return mInstance;
}

bool PreviewDialog::isActive()
{
  if (mInstance)
    return true;

  return false;
}
