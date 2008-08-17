/*
 *
 * TODO .next:
 *
 * QwtGlobalHotkey
 * Rewrite Area select
 *
 *
 */

#include <QApplication>
#include <QDesktopWidget>
#include <QTranslator>
#include <QLocale>
#include <QImageWriter>
#include <QAbstractEventDispatcher>

#include "osspecific.h"
#include "lightscreenwindow.h"
#include "engines/actionengine.h"

#include <stdio.h>
#include <stdlib.h>

void myMessageOutput(QtMsgType type, const char *msg)
{ // Message output to print everything into stdout, so Windows can catch it.
  //Comment out debug ouput for release build
  switch (type)
  {
  case QtDebugMsg:
    fprintf(stdout, "Debug: %s\n", msg);
    break;
  case QtWarningMsg:
    fprintf(stdout, "Warning: %s\n", msg);
    break;
  case QtCriticalMsg:
    fprintf(stdout, "Critical: %s\n", msg);
    break;
  case QtFatalMsg:
    fprintf(stdout, "Fatal: %s\n", msg);
    abort();
  }
}

bool myEventFilter(void* message)
{
#ifdef Q_WS_WIN
  MSG* msg = (MSG*)message;

  if (msg->message == WM_HOTKEY)
  {
    int code;
    // Get the hotkey code (which is also the Action code) from the message.
    code = msg->wParam;

    ActionEngine::instance()->triggerAction(code);
  }

  if (msg->message == WM_KEYUP || msg->message == WM_SYSKEYUP)
  { // Windows sends the printscreen key a syskey event, and Qt won't catch it.

    int vk = msg->wParam;

    if (vk == VK_SNAPSHOT)
    {
      QFlags<Qt::KeyboardModifier> keyboardModifiers;

      if (GetAsyncKeyState(VK_CONTROL) < 0)
      keyboardModifiers = keyboardModifiers | Qt::ControlModifier;
      if (GetAsyncKeyState(VK_SHIFT) < 0)
      keyboardModifiers = keyboardModifiers | Qt::ShiftModifier;
      if (GetAsyncKeyState(VK_LMENU) < 0)
      keyboardModifiers = keyboardModifiers | Qt::AltModifier;

      ActionEngine::instance()->slotSysKeyPress(Qt::Key_Print, keyboardModifiers);
    }

    return true;
  }
#endif

  return false;
}

int main(int argc, char *argv[])
{
  qInstallMsgHandler(myMessageOutput);

  QApplication a(argc, argv);
  a.setOrganizationName("K");
  a.setApplicationName("Lightscreen");
  a.setApplicationVersion("0.5");
  a.setQuitOnLastWindowClosed(false);

  QString locale = QLocale::system().name();

  //qDebug() << "Locale: " << locale;

  QTranslator translator;
  bool loadTry = translator.load(QString("lang/lightscreen_") + locale);

  if (!loadTry)
  {
    locale = locale.remove(2, 3); //Remove the country code and try again
    translator.load(QString("lang/lightscreen_") + locale);
  }

  a.installTranslator(&translator);

  OS::singleInstance("LightscreenN");

  LightscreenWindow w;

  if (a.arguments().size() == 1)
    w.show();

  QObject::connect(&w, SIGNAL(accepted()), &a, SLOT(quit()));
  QObject::connect(&w, SIGNAL(rejected()), &a, SLOT(quit()));

  QAbstractEventDispatcher::instance()->setEventFilter(myEventFilter);

  return a.exec();
}

