#include <QApplication>
#include <QBitmap>
#include <QDesktopWidget>
#include <QDialog>
#include <QLibrary>
#include <QPixmap>
#include <QTextEdit>
#include <QTranslator>
#include <QWidget>

#include <QDebug>

#include <string>

#if defined(Q_WS_WIN)
  #include <windows.h>
  #include <shlobj.h>

  typedef struct
  {
    int cxLeftWidth;
    int cxRightWidth;
    int cyTopHeight;
    int cyBottomHeight;
  } MARGINS;

  typedef HRESULT (WINAPI *PtrDwmExtendFrameIntoClientArea)(HWND hWnd, const MARGINS *margins);
  static PtrDwmExtendFrameIntoClientArea pDwmExtendFrameIntoClientArea = 0;

  typedef HRESULT (WINAPI *PtrDwmIsCompositionEnabled)(BOOL *pfEnabled);
  static PtrDwmIsCompositionEnabled pDwmIsCompositionEnabled = 0;
#endif

#include "os.h"

void os::addToRecentDocuments(QString fileName)
{
#if defined(Q_WS_WIN)
  SHAddToRecentDocs(SHARD_PATH, fileName.toAscii().data()); // Windows 7 jump lists use this.
#endif
}

QPixmap os::grabWindow(WId winId)
{
#if defined(Q_WS_WIN)
  HDC hdcScreen = GetDC(NULL);

  RECT rcWindow;
  GetWindowRect(winId, &rcWindow);

  if (IsZoomed(winId))
  {
    if (QSysInfo::WindowsVersion >= QSysInfo::WV_VISTA)
    {// TODO: WTF!
      rcWindow.right += 8;
      rcWindow.left -= 8;
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
#if defined(Q_WS_WIN)
  ShowWindow(window->winId(), SW_RESTORE);
  SetForegroundWindow(window->winId());
#endif
}

QPixmap os::cursor()
{
#if defined(Q_WS_WIN)
  // Get the icon info
  ICONINFO iconInfo;
  GetIconInfo(GetCursor(), &iconInfo);

  QPixmap result = QPixmap::fromWinHBITMAP(iconInfo.hbmColor);
  result.setMask(QBitmap(QPixmap::fromWinHBITMAP(iconInfo.hbmMask)));

  return result;
#else
  return QPixmap();
#endif
}

void os::translate(QString language)
{
  static QTranslator *translator = 0;

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

bool os::aeroGlass(QWidget* target)
{
#if defined(Q_WS_WIN)

  if (QSysInfo::WindowsVersion < QSysInfo::WV_VISTA)
    return false;  // Glass frame only for Windows Vista and above.

  static QLibrary dwmapi("dwmapi");

  pDwmIsCompositionEnabled = (PtrDwmIsCompositionEnabled)dwmapi.resolve("DwmIsCompositionEnabled");

  BOOL enabled;
  pDwmIsCompositionEnabled(&enabled);

  if (enabled)
  {
    pDwmExtendFrameIntoClientArea = (PtrDwmExtendFrameIntoClientArea)dwmapi.resolve("DwmExtendFrameIntoClientArea");

    MARGINS margins = { -1}; //setting it to -1 makes the glass available throughout the entire window

    enabled = SUCCEEDED(pDwmExtendFrameIntoClientArea(target->winId(), &margins));

    if (enabled)
    {
      target->setAttribute(Qt::WA_NoSystemBackground);
      target->setAttribute(Qt::WA_OpaquePaintEvent);
    }
  }

  return enabled;
#else
  Q_UNUSED(target)
#endif
}
