#include "previewdialog.h"
#include "../tools/screenshot.h"
#include "../tools/os.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QIcon>
#include <QPushButton>
#include <QPalette>
#include <QDesktopWidget>
#include <QLabel>
#include <QStackedLayout>

#include <QDebug>

PreviewDialog::PreviewDialog(QWidget *parent) :
    QDialog(parent)
{
  setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
  os::aeroGlass(this);
  setWindowTitle("Screenshot Preview");

  QHBoxLayout *l = new QHBoxLayout;
  mStack = new QStackedLayout;
  connect(mStack, SIGNAL(currentChanged(int)), this, SLOT(indexChanged(int)));

  mPrevButton = new QPushButton(QIcon(":/icons/arrowLeft"), "");
  connect(mPrevButton, SIGNAL(clicked()), this, SLOT(previous()));

  mNextButton = new QPushButton(QIcon(":/icons/arrowRight"), "");
  connect(mNextButton, SIGNAL(clicked()), this, SLOT(next()));

  mPrevButton->setFlat(true);
  mPrevButton->setIconSize(QSize(30, 50));

  mNextButton->setFlat(true);
  mNextButton->setIconSize(QSize(30, 50));

  mNextButton->setVisible(false);
  mPrevButton->setVisible(false);

  l->addWidget(mPrevButton);
  l->addLayout(mStack);
  l->addWidget(mNextButton);

  l->setMargin(0);
  mStack->setMargin(0);

  setLayout(l);
}

PreviewDialog::~PreviewDialog()
{
  qDebug() << "PreviewDialog destroyed";
}

void PreviewDialog::add(Screenshot *screenshot)
{
  if (!isVisible())
    show();

  QLabel *widget = new QLabel(this);

  connect(widget, SIGNAL(destroyed()), screenshot, SLOT(discard()));

  QSize size = screenshot->pixmap().size();

  if (!(size.width() < 200 && size.height() < 200))
  { // Scale the image when it's witdth or height exceed 200
    size.scale(200, 200, Qt::KeepAspectRatio);
  }

  QPixmap thumbnail = screenshot->pixmap().scaled(size);

  widget->setPixmap(thumbnail);

  widget->setMinimumSize(size);
  widget->setMaximumSize(size);
  widget->resize(size);

  QPushButton *confirmPushButton = new QPushButton(QIcon(":/icons/Good"), "", widget);
  QPushButton *discardPushButton = new QPushButton(QIcon(":/icons/Bad"), "", widget);

  connect(confirmPushButton, SIGNAL(clicked()), screenshot, SLOT(confirm()));
  connect(confirmPushButton, SIGNAL(clicked()), this, SLOT(closePreview()));

  connect(discardPushButton, SIGNAL(clicked()), screenshot, SLOT(discard()));
  connect(discardPushButton, SIGNAL(clicked()), this, SLOT(closePreview()));

  QHBoxLayout *wlayout = new QHBoxLayout;
  wlayout->addWidget(confirmPushButton);
  wlayout->addStretch();
  wlayout->addWidget(discardPushButton);

  QVBoxLayout *wl = new QVBoxLayout;
  wl->addLayout(wlayout);
  wl->addStretch();

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

void PreviewDialog::relocate()
{
  resize(sizeHint());

  QPoint where = QApplication::desktop()->availableGeometry(this).bottomRight();
  where.setX(where.x() - frameGeometry().width());
  where.setY(where.y() - frameGeometry().height());

  move(where);
}

void PreviewDialog::closePreview()
{
  QLabel *widget = qobject_cast<QLabel*>(sender()->parent());
  mStack->removeWidget(widget);
  widget->hide();
  widget->deleteLater();

  if (mStack->count() == 0)
    close();
  else
    relocate();
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
    mNextButton->setVisible(false);
    mPrevButton->setVisible(false);
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

void PreviewDialog::closeEvent(QCloseEvent *event)
{
  mInstance = 0;
  deleteLater();
}

// Singleton

PreviewDialog* PreviewDialog::mInstance = 0;

PreviewDialog *PreviewDialog::instance()
{
  if (!mInstance)
    mInstance = new PreviewDialog(0);

  return mInstance;
}
