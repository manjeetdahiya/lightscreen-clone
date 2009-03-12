#include <QDesktopServices>
#include <QUrl>

#include "aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
  QDialog(parent)
{
  ui.setupUi(this);

  ui.versionLabel->setText(tr("Version %1").arg(qApp->applicationVersion()));

  connect(ui.mainLabel ,    SIGNAL(linkActivated(QString)), this, SLOT(openUrl(QString)));
  connect(ui.linksLabel,    SIGNAL(linkActivated(QString)), this, SLOT(openUrl(QString)));
}

void AboutDialog::openUrl(QString url)
{
  QDesktopServices::openUrl(QUrl(url));
}


