#include <QApplication>
#include <QClipboard>
#include <QDateTime>
#include <QDebug>
#include <QDesktopWidget>
#include <QPixmap>
#include <QSettings>

#ifdef Q_WS_WIN
#include <windows.h>
#endif

#include "../dialogs/areaselector.h"
#include "../dialogs/previewdialog.h"
#include "screenshot.h"

#include "os.h"

Screenshot::Screenshot(Screenshot::Options opt) :
    options(opt)
{

}

void Screenshot::activeWindow()
{
#ifdef Q_WS_WIN
  HWND fWindow = GetForegroundWindow();

  if (fWindow == NULL)
  return;

  if (fWindow == GetDesktopWindow())
   wholeScreen();

  setPixmap(os::grabWindow(GetForegroundWindow()));
#else
  wholeScreen();
#endif
}

QString Screenshot::newFileName()
{
  if (!options.directory.exists())
    options.directory.mkpath(options.directory.path());

  QString naming("%1");

  int naming_largest = 0;

  switch (options.naming)
  {
  case 0: // Numeric
    // Iterating through the folder to find the largest numeric naming.
    foreach(QString file, options.directory.entryList(QDir::Files)){
    if (file.contains(options.prefix))
    {
      file.chop(file.size() - file.lastIndexOf("."));
      file.remove(options.prefix);

      if (file.toInt()> naming_largest)
      naming_largest = file.toInt();
    }
  }

  naming = naming.arg(naming_largest + 1);
  break;
  case 1: // Timestamp
  naming = naming.arg(QDateTime::currentDateTime().toTime_t());
  break;
  case 2: // Date
  naming = naming.arg(QDateTime::currentDateTime().toString("dd-MM-yyyy hh.mm.ss"));
  break;
}

  // %1: Path
  // %2-%3: Naming / Prefix
  // %4: File extension
  QString fileName("%1%2%3.%4");
  QString extension = QString(formatString()).toLower();
  QString path = QDir::toNativeSeparators(options.directory.path());

  // Cleanup
  if (QDir::toNativeSeparators(path.at(path.size()-1)) != QDir::separator() && !path.isEmpty())
    path.append(QDir::separator());

  if (options.flipNaming)
    fileName = fileName.arg(path).arg(naming).arg(options.prefix).arg(extension);
  else
    fileName = fileName.arg(path).arg(options.prefix).arg(naming).arg(extension);

  return fileName;
}

char* Screenshot::formatString()
{
  if (options.format == 0)
    return "PNG";
  if (options.format == 1)
    return "JPG";
  if (options.format == 2)
    return "BMP";

  return "PNG"; //default
}

void Screenshot::selectedArea()
{
  static bool alreadySelecting = false; // Prevents multiple AreaSelector instances

  if (alreadySelecting)
    return;

  alreadySelecting = true;

  QPixmap screen = grabDesktop();

  AreaSelector selector;
  int result = selector.exec();

  alreadySelecting = false;

  if (result == QDialog::Accepted && selector.rect().isValid())
    setPixmap(selector.pixmap());
  else
    return;
}

void Screenshot::wholeScreen()
{
  if (options.directX)
    setPixmap(os::getDxScreen());
  else
    setPixmap(grabDesktop());
}

QPixmap Screenshot::grabDesktop()
{
  if (options.currentMonitor)
  { // Shamelessly stolen from KSnapshot: http://websvn.kde.org/trunk/KDE/kdegraphics/ksnapshot/ksnapshot.cpp?revision=845665&view=markup
    QDesktopWidget *desktop = QApplication::desktop();
    int screenId = desktop->screenNumber( QCursor::pos() );;
    QRect geom = desktop->screenGeometry( screenId );
    return QPixmap::grabWindow( desktop->winId(), geom.x(), geom.y(), geom.width(), geom.height() );
  } else {
    return QPixmap::grabWindow(QApplication::desktop()->winId());
  }
}

void Screenshot::setPixmap(QPixmap pixmap)
{
  mPixmap = pixmap;
}

QPixmap& Screenshot::pixmap()
{
  return mPixmap;
}

Screenshot::Result Screenshot::take()
{
  switch (options.mode)
  {
  case Screenshot::WholeScreen:
    wholeScreen();
    break;

  case Screenshot::SelectedArea:
    selectedArea();
    break;

  case Screenshot::ActiveWindow:
    activeWindow();
    break;
  }

  QString fileName;
  bool    action;

  if (options.file)
  {
    fileName = newFileName();
    action   = pixmap().save(fileName, formatString(), options.quality);
  }
  else
  {
    QString fileName = "";
    action   = !(pixmap().isNull());
  }

  Screenshot::Result result;
  result.options  = options;
  result.result   = action;
  result.fileName = fileName;

  if (action && options.preview)
    new PreviewDialog(pixmap());

  if (action && options.clipboard)
    qApp->clipboard()->setPixmap(pixmap());

  return result;
}
