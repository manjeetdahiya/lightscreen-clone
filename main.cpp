#include <QApplication>
#include <QDesktopWidget>
#include <QLocale>
#include <QSettings>

#include "tools/os.h"
#include "lightscreenwindow.h"

int main(int argc, char *argv[])
{
  QApplication application(argc, argv);
  application.setOrganizationName("K");
  application.setApplicationName ("Lightscreen");
  application.setApplicationVersion("1.1");
  application.setQuitOnLastWindowClosed(false);

  os::singleInstance();

  LightscreenWindow lightscreen;

  if (application.arguments().size() == 1)
    lightscreen.show();

  QObject::connect(&lightscreen, SIGNAL(finished(int)), &application, SLOT(quit()));
  return application.exec();
}

