#include <QDesktopServices>
#include <QProgressBar>
#include <QLabel>
#include <QLayout>
#include <QUrl>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QAbstractButton>


#include "updaterdialog.h"
#include "../updater/updater.h"
#include "../tools/os.h"

#include <QDebug>

UpdaterDialog::UpdaterDialog(bool silent) :
QDialog(0), mSilent(silent)
{
  setWindowTitle(tr("Updater - Lightscreen"));
  setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

  if (os::aeroGlass(this))
    setStyleSheet("QLabel { padding: 3px; border: 1px solid #898c95; border-radius: 5px; background-color: qlineargradient(spread:pad, x1:1, y1:1, x2:0.988636, y2:0.608, stop:0 rgba(235, 235, 235, 255), stop:1 rgba(255, 255, 255, 255)); }");

  mProgressBar = new QProgressBar(this);
  mProgressBar->setTextVisible(false);
  mProgressBar->setRange(0, 0);

  mLabel = new QLabel(tr("Checking for updates"), this);

  connect(mLabel, SIGNAL(linkActivated(QString)), this, SLOT(link(QString)));

  mButtonBox = new QDialogButtonBox(this);
  mButtonBox->addButton(QDialogButtonBox::Cancel);

  mProgressLabel = new QLabel("", this);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->setSpacing(8);
  layout->setMargin(6);

  layout->addWidget(mLabel);
  layout->addWidget(mProgressBar);

  QHBoxLayout *hLayout = new QHBoxLayout;
  hLayout->addWidget(mProgressLabel);
  hLayout->addWidget(mButtonBox);

  layout->addLayout(hLayout);

  mProgressLabel->setVisible(false);

  setLayout(layout);
  setModal(true);

  resize(260, 0); // The aproximate size of the "enlarged" version (with all the buttons)

  connect(mButtonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(button(QAbstractButton*)));
  connect(mButtonBox, SIGNAL(accepted()), this, SLOT(yes()));
  connect(this, SIGNAL(rejected()), this, SLOT(deleteLater()));
}

void UpdaterDialog::checkDone(Updater::Result result)
{
  switch (result)
  {
    case Updater::NewVersion:
      setLabelText(tr("There's a <a href=\"http://lightscreen.sourceforge.net/new-version\">new Lightscreen version</a> available,<br><br><b>What would you like to do?</b><br><i>(You can turn this notification off)</i>"));
      mButtonBox->clear();
      mButtonBox->addButton(tr("Download"), QDialogButtonBox::AcceptRole);

      if (mSilent) // If not called from the options menu.
        mButtonBox->addButton(tr("Turn Off"), QDialogButtonBox::ActionRole);

      mButtonBox->addButton(tr("Cancel")  , QDialogButtonBox::RejectRole);
      mProgressBar->setMaximum(1);
      mProgressBar->setValue(0);

      if (mSilent)
        show();
    break;
    case Updater::MajorUpgrade:
      setLabelText(tr("There's a new version available,<br> please see <a href=\"http://lightscreen.sourceforge.net/new-version\">the Lighscreen website</a>."));

      if (mSilent)
        show();
    break;
    case Updater::NoVersion:
      setLabelText(tr("No new versions available"));
      mProgressBar->setEnabled(false);
    break;
    case Updater::Error:
      setLabelText(tr("Update check failed"));
    break;
  }

}

void UpdaterDialog::downloading(QString file)
{
  if (file.isEmpty())
  {
    downloadDone(true);
    return;
  }

  setLabelText(tr("Downloading ") + file);
}

void UpdaterDialog::downloadDone(bool error)
{
  if (error)
    setLabelText(tr("Download failed"));
  else
    setLabelText(tr("Download complete.. launching installer."));
}

void UpdaterDialog::progressBar(int value, int maximum)
{
  if (maximum < 500 && value > 0)
  {
    mProgressBar->setMaximum(0);
    return; // Too small
  }

  double mbValue   = value/1048576.0;
  double mbMaximum = maximum/1048576.0;

  mProgressBar->setMaximum(maximum);
  mProgressBar->setValue(value);

  if (!mProgressLabel->isVisible())
    mProgressLabel->setVisible(true);

  mProgressLabel->setText(tr("%1 mb of %2 mb").arg(QString::number(mbValue, 'f', 1)).arg(QString::number(mbMaximum, 'f', 1)));
}

void UpdaterDialog::setLabelText(QString text)
{
  mLabel->setText(text);
}

void UpdaterDialog::link(QString url)
{
  QDesktopServices::openUrl(url);
}

void UpdaterDialog::yes()
{
  mButtonBox->clear();
  mButtonBox->addButton(QDialogButtonBox::Cancel);

  emit download();
}

void UpdaterDialog::button(QAbstractButton* button)
{
  if (button->text() == tr("Download"))
    emit download();

  if (button->text() == tr("Turn Off"))
  {
    emit disable();
    reject();
  }

  if (button->text() == tr("Cancel"))
  {
    emit cancel();
    reject();
  }
}
