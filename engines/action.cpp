#include "action.h"

Action::Action(int code)
{
  setCode(code);
}

int Action::getCode() const
{
  return mCode;
}

void Action::setCode(int code)
{
  mCode = code;
}

int Action::getMode() const
{
  return mMode;
}

void Action::setMode(int mode)
{
  mMode = mode;
}

void Action::exec()
{
  emit triggered(getMode());
}
