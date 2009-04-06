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
  {
   wholeScreen();
   return;
  }

  mPixmap = os::grabWindow(GetForegroundWindow());
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
  case 1: // Date
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

  return ".jpg"; //default
}

void Screenshot::selectedArea()
{
  static bool alreadySelecting = false; // Prevents multiple AreaSelector instances

  if (alreadySelecting)
    return;

  alreadySelecting = true;

  grabDesktop();

  if (mPixmap.isNull())
    return;

  AreaSelector selector (mPixmap, mOptions.magnify);
  int result = selector.exec();

  alreadySelecting = false;

  if (result == QDialog::Accepted && selector.rect().isValid())
    mPixmap = selector.pixmap();
  else
    return;
}

void Screenshot::wholeScreen()
{
  grabDesktop();

  if (mOptions.cursor && !mPixmap.isNull())
  {
    QPainter painter(&mPixmap);
    painter.drawPixmap(QCursor::pos(), os::cursor());
  }
}

void Screenshot::grabDesktop()
{
  QRect geometry;

  if (mOptions.currentMonitor)
      geometry = qApp->desktop()->screenGeometry(QCursor::pos());
  else
      geometry = qApp->desktop()->geometry();

  mPixmap = QPixmap::grabWindow(qApp->desktop()->winId(), geometry.x(), geometry.y(), geometry.width(), geometry.height());
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

  return !(mPixmap.isNull());
}

QString Screenshot::save()
{
  QString fileName = "";
  bool    action = false;

  if (mOptions.file && !mOptions.saveAs)
  {
    fileName = newFileName();

    fileName = fileName + extension();
  } else if (mOptions.file && mOptions.saveAs)
  {
    fileName = QFileDialog::getSaveFileName(0, tr("Save as.."), newFileName(), "*"+extension());
  }

  if (fileName.isEmpty())
      action = false;
    else
      action = mPixmap.save(fileName, 0, mOptions.quality);

  if (mOptions.file)
  { // Windows only
    os::addToRecentDocuments(fileName);
  }

  if (mOptions.clipboard)
    QApplication::clipboard()->setPixmap(mPixmap, QClipboard::Clipboard);

  if (action)
    return fileName;
  else
    return QString();
}
