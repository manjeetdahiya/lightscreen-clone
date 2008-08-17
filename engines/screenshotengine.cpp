#include <QApplication>
#include <QDateTime>
#include <QDesktopWidget>
#include <QPixmap>
#include <QClipboard>
#include <QSettings>
#include <QDebug>

#ifdef Q_WS_WIN
#include <windows.h>
#endif

#include "../dialogs/areaselector.h"
#include "screenshotengine.h"

#include "../osspecific.h"

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
    return getWholeScreen(); //TODO: Don't really know if it can happen, check.

  /*
    if (IsZoomed(fWindow) != 0) // If the window is maximized, just get the whole screen.
    return getWholeScreen();
  */

  return OS::grabWindow(GetForegroundWindow());
#else
  return QPixmap::grabWindow(QApplication::desktop()->winId());
#endif
}

QString ScreenshotEngine::getFileName(ScreenshotEngine::Options options)
{// TODO: Rewrite with Options in mind.
  QString path;
  QString prefix;
  QString suffix;
  QString extension;

  path = options.directory.path();
  path = QDir::toNativeSeparators(path);

  if (path.at(path.size()) != QDir::separator() && !path.isEmpty())
    path.append(QDir::separator());

  QDir dir(path);
  if (!dir.exists())
    dir.mkpath(path);

  prefix = options.prefix;

  extension = QString(getFormat(options.format)).toLower();

  int type = options.naming;
  if (type == 0)
  {
    QStringList files = QDir(path).entryList(QDir::Files);
    QString file;

    int largest = 0;
    foreach(file, files)
    {
      if (file.contains(prefix))
      {
        file.chop(file.size() - file.lastIndexOf("."));
        file.remove(prefix);

        if (file.toInt() > largest)
          largest = file.toInt();
      }
    }

    suffix = QString("%1").arg(largest + 1);
  }
  else
  {
    suffix = QString("%1").arg(QDateTime::currentDateTime().toTime_t());
  }

  if (options.flipNaming)
    return QString("%1%2%3.%4").arg(path).arg(suffix).arg(prefix).arg(extension);
  else
    return QString("%1%2%3.%4").arg(path).arg(prefix).arg(suffix).arg(extension);

}

char* ScreenshotEngine::getFormat(int format)
{
  if (format == 0)
    return "PNG";
  if (format == 1)
    return "JPEG";
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

  AreaSelector s;
  int r = s.exec();

  alreadySelecting = false;

  if (r == QDialog::Accepted)
  {
    qDebug() << "Dialog ends";
    return screen.copy(s.getRect());
  }
  else
  {
    return QPixmap();
  }
}

QPixmap ScreenshotEngine::getWholeScreen(bool directx)
{
  if (directx)
  {
    return OS::getDxScreen();
  }
  else
  {
    return QPixmap::grabWindow(QApplication::desktop()->winId());
  }
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

  //qDebug() << "Taking a screenshot";
  //qDebug() << "Prefix: " << options.prefix << "\nMode:" << options.mode;

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

  //qDebug() << "Result: " << br;

  ScreenshotEngine::Result result;
  result.options  = options;
  result.result   = br;
  result.fileName = fileName;

  qDebug() << screenshot.isNull();

  if (!screenshot.isNull() && QSettings().value("options/clipboard", true).toBool())
  {// TODO: Add UI for this option (advanced)
    qApp->clipboard()->setPixmap(screenshot);
  }

  mLastResult = result;
  return result;
}
