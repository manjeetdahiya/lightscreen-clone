#include <QWidget>
#include <QApplication>
#include <QLibrary>
#include <QDesktopWidget>
#include <QTranslator>
#include <QDialog>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPixmap>
#include <QPoint>
#include <QBitmap>

#include <QDebug>

#include <string>

#if defined(Q_WS_WIN)
  #include <windows.h>

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

QPixmap os::grabWindow(WId winId)
{
#if defined(Q_WS_WIN)
  HDC hdcScreen = GetDC(NULL);

  RECT rcWindow;
  GetWindowRect(winId, &rcWindow);

  if (IsZoomed(winId))
  {
    if (QSysInfo::WindowsVersion == QSysInfo::WV_VISTA)
    {// TODO: WTF! (Test this for 0.6!)
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

void os::vistaGlass(QWidget* target)
{
#if defined(Q_WS_WIN)
  static QLibrary dwmapi("dwmapi");

  if (QSysInfo::WindowsVersion == QSysInfo::WV_VISTA)
  { // Glass frame only for Windows Vista

    pDwmIsCompositionEnabled = (PtrDwmIsCompositionEnabled)dwmapi.resolve("DwmIsCompositionEnabled");

    BOOL enabled;
    pDwmIsCompositionEnabled(&enabled);

    if (enabled)
    {
      pDwmExtendFrameIntoClientArea = (PtrDwmExtendFrameIntoClientArea)dwmapi.resolve("DwmExtendFrameIntoClientArea");

      MARGINS margins =
      { -1}; //setting it to -1 makes the glass available throughout the entire window

      pDwmExtendFrameIntoClientArea(target->winId(), &margins);
      target->setAttribute(Qt::WA_NoSystemBackground);
    }
  }
#else
  Q_UNUSED(target)
#endif
}
