#include <QApplication>
#include <QClipboard>
#include <QDateTime>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QPainter>
#include <QPixmap>
#include <QSettings>

#if defined(Q_WS_WIN)
  #include <windows.h>
#endif

#include "../dialogs/areaselector.h"
#include "screenshot.h"

#include "os.h"

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

  QString naming;
  int naming_largest = 0;

  if (mOptions.flipNaming)
    naming = "%1" + mOptions.prefix;
  else
    naming = mOptions.prefix + "%1";

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

  QString fileName;

  QString path = QDir::toNativeSeparators(mOptions.directory.path());

  // Cleanup
  if (path.at(path.size()-1) != QDir::separator() && !path.isEmpty())
    path.append(QDir::separator());

  fileName.append(path);
  fileName.append(naming);

  return fileName;
}

QString Screenshot::extension()
{
  if (mOptions.format == 0)
    return ".png";
  if (mOptions.format == 1)
    return ".jpg";
  if (mOptions.format == 2)
    return ".bmp";
  if (mOptions.format == 3)
    return ".tiff";

  return ".jpg"; //default
}

void Screenshot::selectedArea()
{
  static bool alreadySelecting = false; // Prevents multiple AreaSelector instances

  if (alreadySelecting)
    return;

  alreadySelecting = true;

  QPixmap desktop = grabDesktop();

  if (desktop.isNull())
    return;

  AreaSelector selector (desktop, mOptions.magnify);
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

  if (mOptions.cursor && !pixmap().isNull())
  {
    QPainter painter(&pixmap());
    painter.drawPixmap(QCursor::pos(), os::cursor());
  }
}

QPixmap Screenshot::grabDesktop()
{
  QRect geometry;

  if (mOptions.currentMonitor)
      geometry = QApplication::desktop()->screenGeometry(QCursor::pos());
  else
      geometry = QApplication::desktop()->geometry();

  return QPixmap::grabWindow(QApplication::desktop()->winId(), geometry.x(), geometry.y(), geometry.width(), geometry.height());
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
  QString fileName = "";
  bool    action = false;

  if (mOptions.file && !mOptions.saveAs)
  {
    fileName = newFileName();

    // Optimize?
    int n = 1;
    QString original = fileName;
    while (QFile::exists(fileName+extension()))
    {
      fileName = QString("%1 (%2)").arg(original).arg(n);
      n++;
    }

    action = pixmap().save(fileName+extension(), 0, mOptions.quality);
  }

  if (mOptions.file && mOptions.saveAs)
  {
    fileName = QFileDialog::getSaveFileName(0, tr("Save as.."), newFileName(), "*"+extension());

    if (fileName.isEmpty())
      action = false;
    else
      action = pixmap().save(fileName, 0, mOptions.quality);
  }

  if (mOptions.file)
  { // Windows only
    os::addToRecentDocuments(fileName);
  }

  if (mOptions.clipboard)
    qApp->clipboard()->setPixmap(pixmap());

  if (action)
    return fileName;
  else
    return QString();
}
