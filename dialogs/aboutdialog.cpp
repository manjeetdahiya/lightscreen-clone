#include "aboutdialog.h"
#include <QApplication>
#include <QDesktopServices>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	
	ui.versionLabel->setText(tr("Version %1").arg(qApp->applicationVersion()));
	
	connect(ui.linksLabel   , SIGNAL(linkActivated(QString)), this, SLOT(openUrl(QString)));
	connect(ui.creationLabel, SIGNAL(linkActivated(QString)), this, SLOT(openUrl(QString)));
	connect(ui.qtPushButton , SIGNAL(clicked()), qApp, SLOT(aboutQt()));
}

void AboutDialog::openUrl(QString url)
{
  QDesktopServices::openUrl(QUrl(url));
}

