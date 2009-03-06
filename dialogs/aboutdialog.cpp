#include <QApplication>
#include <QDesktopServices>
#include <QUrl>

#include "aboutdialog.h"
#include "../tools/os.h"

AboutDialog::AboutDialog(QWidget *parent) :
  QDialog(parent)
{
  ui.setupUi(this);

  if (os::aeroGlass(this))
  {
    setStyleSheet("QLabel { background-color: qlineargradient(spread:pad, x1:0.494, y1:0, x2:0.483, y2:1, stop:0 rgba(237, 237, 237, 255), stop:1 rgba(255, 255, 255, 255)); border: 1px solid gray; border-radius: 4px; padding: 3px; }");
    ui.layout->setMargin(0);
    ui.layout->setSpacing(3);
    resize(minimumSizeHint());
    setMaximumSize(size());
    setMinimumSize(size());
  }

  ui.versionLabel->setText(tr("Version %1").arg(qApp->applicationVersion()));

  connect(ui.mainLabel ,    SIGNAL(linkActivated(QString)), this, SLOT(openUrl(QString)));
  connect(ui.linksLabel,    SIGNAL(linkActivated(QString)), this, SLOT(openUrl(QString)));
}

void AboutDialog::openUrl(QString url)
{
  QDesktopServices::openUrl(QUrl(url));
}


