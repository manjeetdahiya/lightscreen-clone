#ifndef OSSPECIFIC_H_
#define OSSPECIFIC_H_

#include <QWidget>
#include <QLibrary>
#include <string>
#include <QDesktopWidget>

#ifdef Q_WS_WIN
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

class OS
{
  
public:
  OS() { }

static void vistaGlass(QWidget* target)
{
#ifdef Q_WS_WIN
  QLibrary dwmapi("dwmapi");
  
  if (QSysInfo::WindowsVersion == QSysInfo::WV_VISTA)
  { // Glass frame only for Windows Vista
    
    pDwmIsCompositionEnabled = (PtrDwmIsCompositionEnabled)dwmapi.resolve("DwmIsCompositionEnabled");

    BOOL enabled;
    pDwmIsCompositionEnabled(&enabled);
    
    if (enabled)
    {
      pDwmExtendFrameIntoClientArea = (PtrDwmExtendFrameIntoClientArea)dwmapi.resolve("DwmExtendFrameIntoClientArea");
      
      MARGINS margins =
      {-1}; //setting it to -1 makes the glass available throughout the entire window
      
      pDwmExtendFrameIntoClientArea(target->winId(), &margins);
      target->setAttribute(Qt::WA_NoSystemBackground);
    }
  }
  
  dwmapi.unload();
#else
  Q_UNUSED(target)
#endif
}


static bool singleInstance(QString name)
{
#ifdef Q_WS_WIN
  WCHAR* mutexName = (WCHAR*)name.toAscii().data();
  
  HANDLE mutex = ::CreateMutex(NULL, FALSE, mutexName);
  
  if (!mutex)
    return false;
  
  bool alreadyExists = (::GetLastError() == ERROR_ALREADY_EXISTS);
 
  std::wstring str = QString("Lightscreen").toStdWString();
  HWND hLsWnd = ::FindWindow(NULL, str.c_str());
  
  if (hLsWnd)
  {
    ::PostMessage(hLsWnd, WM_QUIT, 0, 0);
  }
  
  return alreadyExists;
#else
  Q_UNUSED(name)
#endif
}

static QPixmap grabWindow(WId winId)
{
  #ifdef Q_WS_WIN
  HDC hdcScreen = GetDC(NULL);

  RECT rcWindow;
  GetWindowRect(winId, &rcWindow);
  
  if (IsZoomed(winId))
  {
    if (QSysInfo::WindowsVersion == QSysInfo::WV_VISTA)
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

static QPixmap getDxScreen()
{
#if Q_WS_WINDOWS
  //RenderTargetSurface.
  IDirect3DSurface9* pRenderTargetSurface = NULL;

  //DestinationTargetSurface
  IDirect3DSurface9* pDestinationTargetSurface = NULL;

  //DisplayMode
  D3DDISPLAYMODE d3dDipMode;

  //HBITMAP that will be the return of the method
  HBITMAP hbm;

  if (m_pd3dDevice == NULL)
     return QPixmap();

  //Get the client rectangle
  RECT rc;
  GetClientRect(myhWnd, &rc);
  ClientToScreen(myhWnd, LPPOINT(&rc.left));
  ClientToScreen(myhWnd, LPPOINT(&rc.right));

  //GetRenderTargetSurface.
  if(FAILED(m_pd3dDevice->GetRenderTarget(0, &pRenderTargetSurface)))
   return QPixmap();

  //Display Mode (d3dDipMode)
  if(FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3dDipMode)))
      return QPixmap();
  
  //GetDestinationTargetSurface
  if(FAILED(m_pd3dDevice->CreateOffscreenPlainSurface((rc.right - rc.left),
                                                     (rc.bottom - rc.top),
                                                     d3dDipMode.Format,
                                                     D3DPOOL_SYSTEMMEM,
                                                     &pDestinationTargetSurface,
                                                     NULL)))
              return QPixmap();

  //copy RenderTargetSurface -> DestTarget
  if(FAILED(m_pd3dDevice->GetRenderTargetData(pRenderTargetSurface, pDestinationTargetSurface)))
   return QPixmap();

  LPD3DXBUFFER bufferedImage = NULL;

  //Save the DestinationTargetSurface into memory (as a bitmap)
  if(FAILED(D3DXSaveSurfaceToFileInMemory(&bufferedImage,
                                        D3DXIFF_BMP,
                                         pDestinationTargetSurface,
                                         NULL,
                                         NULL)))
   return QPixmap();

  //Release Resources
  pRenderTargetSurface->Release();
  pDestinationTargetSurface->Release();

  //Return HBITMAT
  return QPixmap::fromWinHBITMAP(hbm);
#else
  return QPixmap::grabWindow(QApplication::desktop()->winId());
#endif
}

};

#endif /*OSSPECIFIC_H_*/
