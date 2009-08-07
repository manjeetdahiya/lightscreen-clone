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

void PreviewDialog::add(Screenshot *screenshot)
{
  if (!isVisible())
    show();

  QLabel *widget = new QLabel(this);

  QSize size = screenshot->pixmap().size();

  if (!(size.width() < 200 && size.height() < 200))
  { // Scale the image when it's witdth or height exceed 200
    size.scale(200, 200, Qt::KeepAspectRatio);
  }

  QPixmap thumbnail = screenshot->pixmap().scaled(size);

  //QPalette p = widget->palette();
  //p.setBrush(QPalette::Window, QBrush(thumbnail));
  //widget->setPalette(p);
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
  resize(sizeHint()); // Effected resize?

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
  widget->deleteLater(); // Effect, Fadeout?

  relocate();

  if (mStack->count() < 1)
  {
    close();
    deleteLater();
  }
}

void PreviewDialog::previous()
{  
  mNextButton->setEnabled(true); //TODO: Fix enabling/disabling the buttons when deleting/confirming a screenshot

  if (mStack->currentIndex() <= 1)
  {
    mPrevButton->setEnabled(false);
  }

  mStack->setCurrentIndex(mStack->currentIndex()-1);
}

void PreviewDialog::next()
{
  mPrevButton->setEnabled(true);

  if (mStack->currentIndex() == mStack->count()-1)
  {
    mNextButton->setEnabled(false);
  }

  mStack->setCurrentIndex(mStack->currentIndex()+1);
}

// Singleton

PreviewDialog* PreviewDialog::mInstance = 0;

PreviewDialog *PreviewDialog::instance()
{
  if (!mInstance)
    mInstance = new PreviewDialog(0);

  return mInstance;
}
