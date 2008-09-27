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
#include "screenshotengine.h"

#include "../os.h"

ScreenshotEngine::ScreenshotEngine()
{
  mEnabled = true;
}

void ScreenshotEngine::setEnabled(bool enabled)
{
  mEnabled = enabled;
}

bool ScreenshotEngine::isEnabled()
{
  return mEnabled;
}

QPixmap ScreenshotEngine::getActiveWindow()
{
#ifdef Q_WS_WIN
  HWND fWindow = GetForegroundWindow();

  if (fWindow == NULL)
    return QPixmap();

  if (fWindow == GetDesktopWindow())
    return getWholeScreen();

  return os::grabWindow(GetForegroundWindow());
#else
  return QPixmap::grabWindow(QApplication::desktop()->winId());
#endif
}

QString ScreenshotEngine::getFileName(ScreenshotEngine::Options options)
{
  if (!options.directory.exists())
    options.directory.mkpath(options.directory.path());

  QString naming("%1");

  int naming_largest = 0;

  switch (options.naming)
  {
  case 0: // Numeric
    // Iterating through the folder to find the largest numeric naming.
    foreach(QString file, options.directory.entryList(QDir::Files))
    {
      if (file.contains(options.prefix))
      {
        file.chop(file.size() - file.lastIndexOf("."));
        file.remove(options.prefix);

        if (file.toInt() > naming_largest)
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
  QString extension = QString(getFormat(options.format)).toLower();
  QString path = QDir::toNativeSeparators(options.directory.path());

  // Cleanup
  if (path.at(path.size()-1) != QDir::separator() && !path.isEmpty())
    path.append(QDir::separator());

  if (options.flipNaming)
    fileName = fileName.arg(path).arg(naming).arg(options.prefix).arg(extension);
  else
    fileName = fileName.arg(path).arg(options.prefix).arg(naming).arg(extension);

  return fileName;
}

char* ScreenshotEngine::getFormat(int format)
{
  if (format == 0)
    return "PNG";
  if (format == 1)
    return "JPG";
  if (format == 2)
    return "BMP";

  return "PNG"; //default
}

QPixmap ScreenshotEngine::getSelectedArea()
{
  static bool alreadySelecting = false; // Prevents multiple AreaSelector instances

  if (alreadySelecting)
    return QPixmap();

  alreadySelecting = true;

  QPixmap screen = QPixmap::grabWindow(QApplication::desktop()->winId());

  AreaSelector selector;
  int result = selector.exec();

  alreadySelecting = false;

  if (result == QDialog::Accepted && selector.getRect().isValid())
    return screen.copy(selector.getRect());
  else
    return QPixmap();
}

QPixmap ScreenshotEngine::getWholeScreen(bool directx)
{
  if (directx)
    return os::getDxScreen();
  else
    return QPixmap::grabWindow(QApplication::desktop()->winId());
}

ScreenshotEngine::Result ScreenshotEngine::lastScreenshot()
{
  return mLastResult;
}

ScreenshotEngine::Result ScreenshotEngine::take(ScreenshotEngine::Options options)
{
  if (!mEnabled)
  {
    ScreenshotEngine::Result disabledResult;
    disabledResult.result = false;
    return disabledResult;
  }

  QPixmap screenshot;

  switch (options.mode)
  {
  case ScreenshotEngine::WholeScreen:
    screenshot = getWholeScreen(options.directX);
    break;

  case ScreenshotEngine::SelectedArea:
    screenshot = getSelectedArea();
    break;

  case ScreenshotEngine::ActiveWindow:
    screenshot = getActiveWindow();
    break;
  }

  QString fileName = getFileName(options);

  bool br = screenshot.save(fileName, getFormat(options.format), options.quality);

  ScreenshotEngine::Result result;
  result.options  = options;
  result.result   = br;
  result.fileName = fileName;

  if (!screenshot.isNull() && QSettings().value("options/clipboard", true).toBool())
    qApp->clipboard()->setPixmap(screenshot);

  mLastResult = result;
  return result;
}
