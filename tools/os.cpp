#include <QApplication>
#include <QBitmap>
#include <QDesktopWidget>
#include <QDialog>
#include <QDir>
#include <QSettings>
#include <QLibrary>
#include <QPixmap>
#include <QTextEdit>
#include <QTranslator>
#include <QTimer>
#include <QTimeLine>
#include <QWidget>
#include <string>

#include <QPointer>

#include <QDebug>

#include <QMessageBox>

#include "qtwin.h"

#if defined(Q_WS_WIN)
  #include <qt_windows.h>
  #include <shlobj.h>
#endif

#include "os.h"

void os::addToRecentDocuments(QString fileName)
{
#ifdef Q_WS_WIN
  QT_WA ( {
      SHAddToRecentDocs (0x00000003, QDir::convertSeparators(fileName).utf16());
    } , {
      SHAddToRecentDocs (0x00000002, QDir::convertSeparators(fileName).toLocal8Bit().data());
  } ); // QT_WA
#else
  Q_UNUSED(fileName)
#endif
}

bool os::aeroGlass(QWidget* target)
{
  if (QtWin::isCompositionEnabled())
  {
    QtWin::extendFrameIntoClientArea(target);
    return true;
  }

  return false;
}

void os::setStartup(bool startup, bool hide)
{
QString lightscreen = QDir::toNativeSeparators(qApp->applicationFilePath());

if (hide)
  lightscreen.append(" -h");

#ifdef Q_WS_WIN
  // Windows startup settings
  QSettings init("Microsoft", "Windows");
  init.beginGroup("CurrentVersion");
  init.beginGroup("Run");

  if (startup)
  {
    init.setValue("Lightscreen", lightscreen);
  }
  else
  {
    init.remove("Lightscreen");
  }

  init.endGroup();
  init.endGroup();
#endif

#if defined(Q_WS_X11)
  QFile desktopFile(QDir::homePath() + "/.config/autostart/lightscreen.desktop");

  desktopFile.remove();

  if (startup)
  {   
    desktopFile.open(QIODevice::WriteOnly);
    desktopFile.write(QString("[Desktop Entry]\nExec=%1\nType=Application").arg(lightscreen).toAscii());
  }
#endif

  Q_UNUSED(startup)
  Q_UNUSED(hide)
}

QString os::getDocumentsPath()
{
#ifdef Q_WS_WIN
  TCHAR szPath[MAX_PATH];

  if(SUCCEEDED(SHGetFolderPath(NULL,
                               CSIDL_PERSONAL|CSIDL_FLAG_CREATE,
                               NULL,
                               0,
                               szPath)))
  {
    std::wstring path(szPath);

    return QString::fromWCharArray(path.c_str());
  }

  return QDir::homePath() + QDir::separator() + "My Documents";
#else
  return QDir::homePath() + QDir::separator() + "Documents";
#endif
}

QPixmap os::grabWindow(WId winId)
{
#ifdef Q_WS_WIN
  HDC hdcScreen = GetDC(NULL);

  BringWindowToTop(winId);

  RECT rcWindow;
  GetWindowRect(winId, &rcWindow);


  if (IsZoomed(winId))
  {
    if (QSysInfo::WindowsVersion >= QSysInfo::WV_VISTA)
    {// TODO: WTF!
      rcWindow.right -= 8;
      rcWindow.left += 8;
      rcWindow.top += 8;
      rcWindow.bottom -= 8;
    }
    else
    {
      rcWindow.right += 4;
      rcWindow.left -= 4;
      rcWindow.top += 4;
      rcWindow.bottom -= 4;
    }
  }

  HDC hdcMem = CreateCompatibleDC(hdcScreen);
  HBITMAP hbmCapture = CreateCompatibleBitmap(hdcScreen, rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top);
  SelectObject(hdcMem, hbmCapture);

  BitBlt(hdcMem, 0, 0, rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top, hdcScreen, rcWindow.left, rcWindow.top, SRCCOPY);

  ReleaseDC(winId, hdcMem);
  DeleteDC(hdcMem);

  QPixmap pixmap = QPixmap::fromWinHBITMAP(hbmCapture);

  DeleteObject(hbmCapture);

  return pixmap;
#else
  return QPixmap::grabWindow(winId);
#endif
}

void os::setForegroundWindow(QWidget *window)
{
#ifdef Q_WS_WIN
  ShowWindow(window->winId(), SW_RESTORE);
  SetForegroundWindow(window->winId());
#else
  Q_UNUSED(window)
#endif
}

QPixmap os::cursor()
{
#ifdef Q_WS_WIN
  // Get the icon info
  ICONINFO iconInfo;
  GetIconInfo(GetCursor(), &iconInfo);

  QPixmap result = QPixmap::fromWinHBITMAP(iconInfo.hbmColor);

  QPixmap mask = mask.fromWinHBITMAP(iconInfo.hbmMask);
  QBitmap bitmap(mask);

  result.setMask(bitmap);

  mask   = QPixmap();
  bitmap = QBitmap();

  return result;
#else
  return QPixmap();
#endif
}

void os::singleInstance()
{
#ifdef Q_WS_WIN
  WCHAR* mutexName = new WCHAR[QApplication::applicationName().size()+1];
  QApplication::applicationName().toWCharArray(mutexName);

  ::CreateMutex(NULL, FALSE, mutexName);

  if (::GetLastError() == ERROR_ALREADY_EXISTS)
  {
    HWND hLsWnd = ::FindWindow(NULL, mutexName);

    if (hLsWnd)
      ::PostMessage(hLsWnd, WM_QUIT, 0, 0);
  }
#endif
}

void os::translate(QString language)
{
  static QPointer<QTranslator> translator;

  if (translator)
  {
    qApp->removeTranslator(translator);
    delete translator;
    translator = 0;
  }

  if (language.compare("English", Qt::CaseInsensitive) == 0
      || language.isEmpty())
    return;

  translator = new QTranslator;
  translator->load(language, "lang");

  qApp->installTranslator(translator);
}

void os::effect(QObject* target, const char *slot, int frames, int duration, const char* cleanup)
{
  QTimeLine* timeLine = new QTimeLine(duration);
  timeLine->setFrameRange(0, frames);

  timeLine->connect(timeLine, SIGNAL(frameChanged(int)), target, slot);

  if (cleanup != 0)
    timeLine->connect(timeLine, SIGNAL(finished()), target, SLOT(cleanup()));

  timeLine->connect(timeLine, SIGNAL(finished()), timeLine, SLOT(deleteLater()));


  timeLine->start();
}

