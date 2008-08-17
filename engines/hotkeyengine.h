#ifndef HOTKEYENGINE_H_
#define HOTKEYENGINE_H_

#ifdef Q_WS_WIN
#include <windows.h>
#endif

#include <QStringList>

class HotkeyEngine
{

public:
  
  enum Key
  {
#ifdef Q_WS_WIN
    KPrintScreen = VK_SNAPSHOT,
    KScrollLock  = VK_SCROLL,
    KInsert      = VK_INSERT,
    KHome        = VK_HOME,
    KNext        = VK_NEXT,
    KDelete      = VK_DELETE,
    KEnd         = VK_END,
    KPrior       = VK_PRIOR,
    KSpace       = VK_SPACE,
    KF1 = VK_F1,
    KF2 = VK_F2,
    KF3 = VK_F3,
    KF4 = VK_F4,
    KF5 = VK_F5,
    KF6 = VK_F6,
    KF7 = VK_F7,
    KF8 = VK_F8,
    KF9 = VK_F9,
    KF10 = VK_F10,
    KF11 = VK_F11,
    KF12 = VK_F12
#endif
  };
  
  struct Hotkey
  {// REMEMBER that ctrl, alt, and shift all start initializaed to true.
    Key key;
    bool ctrl;
    bool alt;
    bool shift;
    
    bool operator==(const Hotkey& rhs)
    {
      if (key == rhs.key
       && ctrl == rhs.ctrl
       && alt == rhs.alt
       && shift == rhs.shift)
        return true;
      else
        return false;
    }
  };
  
	HotkeyEngine();

	/*
	 * Accepts a target widget for the hotkeys and a Hotkey object.
	 * Returns the hotkey code.
	 */
	static int registerHotkey(QWidget* target, HotkeyEngine::Hotkey hotkey);
	
	/*
	 * Uses the hotkey code to unregister the given hotkey
	 */
	static bool unregisterHotkey(QWidget* target, int hotkeyCode);
	
	static Key indexToKey(int index);
	
  static QString indexToString(int index);

	static int keyToIndex(Key key);
	
	// Same as above, just with VK (virtual key codes)
	static int vkToIndex(quint32 key);
	
	
};

#endif /*HOTKEYENGINE_H_*/
