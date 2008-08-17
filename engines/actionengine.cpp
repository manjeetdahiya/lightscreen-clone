#include "actionengine.h"
#include "hotkeyengine.h"

ActionEngine *ActionEngine::mInstance = 0;


ActionEngine::ActionEngine()
{
}

void ActionEngine::clear(QWidget *actionParent)
{
  Q_FOREACH(Action* action, mActionList)
  {
    HotkeyEngine::unregisterHotkey(actionParent, action->getCode());
    action->deleteLater();
  }
  mActionList.clear();
}

void ActionEngine::add(Action* action)
{
  mActionList << action;
}

void ActionEngine::slotSysKeyPress(Qt::Key key, QFlags<Qt::KeyboardModifier> keyboardModifiers)
{
  emit sysKeyPress(key, keyboardModifiers);
}

void ActionEngine::triggerAction(int code)
{
  Q_FOREACH(Action* action, mActionList)
  {
    if (action->getCode() == code)
    {
      action->exec();
    }
  }
}
