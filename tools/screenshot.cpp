#include <QApplication>
#include <QClipboard>
#include <QDateTime>
#include <QDebug>
#include <QDesktopWidget>
#include <QPixmap>
#include <QSettings>
#include <QPainter>

#if defined(Q_WS_WIN)
  #include <windows.h>
#endif

#include "../dialogs/areaselector.h"
#include "screenshot.h"

#include "os.h"
#include "customnaming.h"

Screenshot::Screenshot() {}
Screenshot::Screenshot(Screenshot::Options options) :  mOptions(options) {}

Screenshot::Options &Screenshot::options()
{
  return mOptions;
}

void Screenshot::setOptions(Screenshot::Options options)
{
  mOptions = options;
}

void Screenshot::activeWindow()
{
#if defined(Q_WS_WIN)
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
  if (!mOptions.directory.exists())
    mOptions.directory.mkpath(mOptions.directory.path());

  QString naming = "";

  if (mOptions.custom)
  {
    CustomNaming customNaming(mOptions.customString, mOptions.directory.entryList(QDir::Files));

    if (customNaming.isValid())
      naming = customNaming.string();
  }

  if (!mOptions.custom || naming.isEmpty())
  { // TODO: Cleanup
    naming = "%1";

    int naming_largest = 0;

    switch (mOptions.naming)
    {
    case 0: // Numeric
      // Iterating through the folder to find the largest numeric naming.
      foreach(QString file, mOptions.directory.entryList(QDir::Files)){
        if (file.contains(mOptions.prefix))
        {
          file.chop(file.size() - file.lastIndexOf("."));
          file.remove(mOptions.prefix);

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
  }

  QString fileName;

  QString extension = QString(formatString()).toLower();
  QString path = QDir::toNativeSeparators(mOptions.directory.path());

  // Cleanup
  if (path.at(path.size()-1) != QDir::separator() && !path.isEmpty())
    path.append(QDir::separator());

  fileName.append(path);

  if (!mOptions.custom)
    fileName.append(mOptions.prefix);

  fileName.append(naming+"."+extension);

  return fileName;
}

char* Screenshot::formatString()
{
  if (mOptions.format == 0)
    return "PNG";
  if (mOptions.format == 1)
    return "JPG";
  if (mOptions.format == 2)
    return "BMP";

  return "PNG"; //default
}

void Screenshot::selectedArea()
{
  static bool alreadySelecting = false; // Prevents multiple AreaSelector instances

  if (alreadySelecting)
    return;

  alreadySelecting = true;

  mOptions.currentMonitor = true;
  AreaSelector selector (grabDesktop(), mOptions.magnify);
  int result = selector.exec();

  alreadySelecting = false;

  if (result == QDialog::Accepted && selector.rect().isValid())
    setPixmap(selector.pixmap());
  else
    return;
}

void Screenshot::wholeScreen()
{
  setPixmap(grabDesktop());

  if (mOptions.cursor)
  {
    QPainter painter(&pixmap());
    painter.drawPixmap(QCursor::pos(), os::cursor());
  }
}

QPixmap Screenshot::grabDesktop()
{
  if (mOptions.currentMonitor)
  {
    //Grabbing only the current screen (as indicated by mouse position)
    int screenId = QApplication::desktop()->screenNumber(QCursor::pos());
    return QPixmap::grabWindow(QApplication::desktop()->screen(screenId)->winId());
  }
  else
  {
    //Grabbing all screens
    int numScreens = QApplication::desktop()->numScreens();

    //if (numScreens <= 1)
    //  return QPixmap::grabWindow(QApplication::desktop()->screen(1)->winId());

    QPixmap allScreens = QPixmap::grabWindow(QApplication::desktop()->screen(1)->winId());
    QPainter painter(&allScreens);
    QPoint point = QPoint(0, 0);

    for (int i = 2; i <= numScreens; i++)
    { // Iterate through the screens and paint them into one pixmap

      painter.drawPixmap(point, QPixmap::grabWindow(QApplication::desktop()->screen(i)->winId()));
      point = allScreens.rect().topRight();
    }

    return allScreens;
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

bool Screenshot::take()
{
  switch (mOptions.mode)
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

  return !pixmap().isNull();
}

QString Screenshot::save()
{
  QString fileName;
  bool    action = false;

  if (mOptions.file)
  {
    fileName = newFileName();
    action   = pixmap().save(fileName, formatString(), mOptions.quality);
  }

  if (mOptions.clipboard)
    qApp->clipboard()->setPixmap(pixmap());

  if (action)
    return fileName;
  else
    return QString();
}
