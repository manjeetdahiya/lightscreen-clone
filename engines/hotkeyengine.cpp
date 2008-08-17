#include <QWidget>
#include <QStringList>

#ifdef Q_WS_WIN
#include <windows.h>
#endif

#include "hotkeyengine.h"

HotkeyEngine::HotkeyEngine()
{

}

int HotkeyEngine::registerHotkey(QWidget* target, HotkeyEngine::Hotkey hotkey)
{
  int code = qRound(qrand()*qrand()*2);
  //qDebug() << "Register hotkey, random code is:" << code;
#ifdef Q_WS_WIN
  UINT modkey = 0;
 
  if (hotkey.ctrl)
  {
    modkey = MOD_CONTROL;
  }
  
  if (hotkey.shift)
  {
    if (modkey)
      modkey = modkey | MOD_SHIFT;
    else
      modkey = MOD_SHIFT;
  }
  
  if (hotkey.alt)
  {
    if (modkey)
      modkey = modkey | MOD_ALT;
    else
      modkey = MOD_ALT;
  }
   
  bool result = RegisterHotKey(target->winId(), code, modkey, hotkey.key); 

  //qDebug() << "Code: " << code;
  //qDebug() << "ModKey: " << modkey;
  //qDebug() << "Key: " << hotkey.key << "(VK_SNAPSHOT: " << VK_SNAPSHOT << ")";
  //qDebug() << "Hotkey registration result:" << result;
  
  if (!result)
    code = -1;
  
#else
  //qDebug() << "Hotkeys not implemented in this platform."
#endif
  
  //qDebug() << "/Register <<<<<<<<";

  return code;
}

bool HotkeyEngine::unregisterHotkey(QWidget* target, int hotkeyCode)
{
  //qDebug() << "Unregistering hotkey" << hotkeyCode;
  //qDebug() << "/Unregister <<<<<<<<<<<";
  bool result;
  
#ifdef Q_WS_WIN
  result = UnregisterHotKey(target->winId(), hotkeyCode);
#endif
  return result;
}

HotkeyEngine::Key HotkeyEngine::indexToKey(int index)
{
  //qDebug() << "indexToKey";
  QList<HotkeyEngine::Key> itemList;
  
  itemList << HotkeyEngine::KPrintScreen;
  itemList << HotkeyEngine::KScrollLock;
  itemList << HotkeyEngine::KInsert;
  itemList << HotkeyEngine::KHome;
  itemList << HotkeyEngine::KNext;
  itemList << HotkeyEngine::KDelete;
  itemList << HotkeyEngine::KEnd;
  itemList << HotkeyEngine::KPrior;
  itemList << HotkeyEngine::KSpace;
  itemList << HotkeyEngine::KF1;
  itemList << HotkeyEngine::KF2;
  itemList << HotkeyEngine::KF3;
  itemList << HotkeyEngine::KF4;
  itemList << HotkeyEngine::KF5;
  itemList << HotkeyEngine::KF6;
  itemList << HotkeyEngine::KF7;
  itemList << HotkeyEngine::KF8;
  itemList << HotkeyEngine::KF9;
  itemList << HotkeyEngine::KF10;
  itemList << HotkeyEngine::KF11;
  
  if (itemList.count() < index || index < 0)
  {
    //qWarning() << "Index out of bounds";
    index = 0;
  }

  return itemList.at(index);
}

QString HotkeyEngine::indexToString(int index)
{
  QStringList itemList;
  
  itemList << QObject::tr("Print Screen");
  itemList << QObject::tr("Scroll Lock");
  itemList << QObject::tr("Insert");
  itemList << QObject::tr("Home");
  itemList << QObject::tr("Page Down");
  itemList << QObject::tr("Delete");
  itemList << QObject::tr("End");
  itemList << QObject::tr("Page Up");
  itemList << QObject::tr("Space");
  itemList << QObject::tr("F1");
  itemList << QObject::tr("F2");
  itemList << QObject::tr("F3");
  itemList << QObject::tr("F4");
  itemList << QObject::tr("F5");
  itemList << QObject::tr("F6");
  itemList << QObject::tr("F7");
  itemList << QObject::tr("F8");
  itemList << QObject::tr("F9");
  itemList << QObject::tr("F10");
  itemList << QObject::tr("F11");
  
  if (itemList.count() < index || index < 0)
  {
    //qWarning() << "Index out of bounds";
    index = 0;
  }

  return itemList.at(index);
}

int HotkeyEngine::keyToIndex(Key key)
{
  QList<HotkeyEngine::Key> itemList;
  
  itemList << HotkeyEngine::KPrintScreen;
  itemList << HotkeyEngine::KScrollLock;
  itemList << HotkeyEngine::KInsert;
  itemList << HotkeyEngine::KHome;
  itemList << HotkeyEngine::KNext;
  itemList << HotkeyEngine::KDelete;
  itemList << HotkeyEngine::KEnd;
  itemList << HotkeyEngine::KPrior;
  itemList << HotkeyEngine::KSpace;
  itemList << HotkeyEngine::KF1;
  itemList << HotkeyEngine::KF2;
  itemList << HotkeyEngine::KF3;
  itemList << HotkeyEngine::KF4;
  itemList << HotkeyEngine::KF5;
  itemList << HotkeyEngine::KF6;
  itemList << HotkeyEngine::KF7;
  itemList << HotkeyEngine::KF8;
  itemList << HotkeyEngine::KF9;
  itemList << HotkeyEngine::KF10;
  itemList << HotkeyEngine::KF11;
   
  //qDebug() << "keyToIndex\nKey: " << key << "\nIndex: " << itemList.indexOf(key);
  return itemList.indexOf(key);
}


int HotkeyEngine::vkToIndex(quint32 key)
{
  QList<quint32> itemList;
  
#ifdef Q_WS_WIN
  itemList << VK_SNAPSHOT;
  itemList << VK_SCROLL;
  itemList << VK_INSERT;
  itemList << VK_HOME;
  itemList << VK_NEXT;
  itemList << VK_DELETE;
  itemList << VK_END;
  itemList << VK_PRIOR;
  itemList << VK_SPACE;
  itemList << VK_F1;
  itemList << VK_F2;
  itemList << VK_F3;
  itemList << VK_F4;
  itemList << VK_F5;
  itemList << VK_F6;
  itemList << VK_F7;
  itemList << VK_F8;
  itemList << VK_F9;
  itemList << VK_F10;
  itemList << VK_F11;
#endif
   
  return itemList.indexOf(key);
}
