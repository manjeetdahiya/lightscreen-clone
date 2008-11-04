#include <QApplication>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QKeySequence>
#include <QTimer>

#include <QDebug>

#ifdef Q_WS_WIN
#include <windows.h>
#endif

#include "hotkeywidget.h"
#include "../tools/globalshortcut/globalshortcutmanager.h"

//TODO: QWT: Fix Printscreen
HotkeyWidget::HotkeyWidget(QWidget *parent) :
  QPushButton(parent)
{
  mHotkey = QKeySequence();
  mShowingError = false;

  setText(tr("Click to select hotkey..."));
}

void HotkeyWidget::setHotkey(QKeySequence hotkey)
{
  mHotkey = hotkey;
  setHotkeyText();
}

QKeySequence HotkeyWidget::hotkey()
{
  return mHotkey;
}

void HotkeyWidget::setHotkeyText()
{
  QString hotkeyText = mHotkey.toString(QKeySequence::NativeText);

  setText(hotkeyText);
  parentWidget()->setFocus();
}

bool HotkeyWidget::event(QEvent *event)
{
  if (event->type() == QEvent::FocusIn)
  {
    setText(tr("Type your hotkey"));
#ifdef Q_WS_WIN
    GlobalShortcutManager::instance()->connect(Qt::Key_Print, this, SLOT(printKeyPress()));
    GlobalShortcutManager::instance()->connect(QKeySequence(Qt::CTRL + Qt::Key_Print), this, SLOT(printKeyPress()));
#endif
  }
  if (event->type() == QEvent::FocusOut)
  {
    if (text() == tr("Invalid hotkey"))
    {
      emit invalidHotkeyError();
      showError();
    }

    setHotkeyText(); // Reset the text

#ifdef Q_WS_WIN
    GlobalShortcutManager::instance()->disconnect(Qt::Key_Print, this, SLOT(printKeyPress()));
    GlobalShortcutManager::instance()->disconnect(QKeySequence(Qt::CTRL + Qt::Key_Print), this, SLOT(printKeyPress()));
#endif
  }
  if (event->type() == QEvent::KeyPress)
  {
    QKeyEvent *keyEvent = static_cast<QKeyEvent*> (event);

    if (isModifier(keyEvent->key()))
      return true;

    mHotkey = getKeySequence(keyEvent);

    setHotkeyText();

    event->ignore();
  }

  return QPushButton::event(event);
}

void HotkeyWidget::printKeyPress()
{
#ifdef Q_WS_WIN
  QFlags<Qt::KeyboardModifier> keyboardModifiers;

  if (GetAsyncKeyState(VK_CONTROL) < 0)
    keyboardModifiers = keyboardModifiers | Qt::ControlModifier;
  if (GetAsyncKeyState(VK_SHIFT) < 0)
    keyboardModifiers = keyboardModifiers | Qt::ShiftModifier;
  if (GetAsyncKeyState(VK_LMENU) < 0)
    keyboardModifiers = keyboardModifiers | Qt::AltModifier;

  QCoreApplication::postEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Print, keyboardModifiers));
#endif
}

void HotkeyWidget::showError()
{
  if (mShowingError)
    return;

  mShowingError = true;

  setStyleSheet("color: #d90000;");
  QTimer::singleShot(500, this, SLOT(hideError()));
}

void HotkeyWidget::hideError()
{
  setStyleSheet("");
  mShowingError = false;
}

QKeySequence HotkeyWidget::getKeySequence(QKeyEvent* event) const
{
  return QKeySequence((isValid(event->key()) ? event->key() : 0)
      + (event->modifiers() & ~Qt::KeypadModifier));
}

/**
 * Returns true if \param key could be used in a shortcut.
 */
bool HotkeyWidget::isValid(int key) const
{
  switch (key)
  {
  case 0:
  case Qt::Key_unknown:
    return false;
  }

  return !isModifier(key);
}

/**
 * Returns true if \param key is modifier.
 */
bool HotkeyWidget::isModifier(int key) const
{
  switch (key)
  {
  case Qt::Key_Shift:
  case Qt::Key_Control:
  case Qt::Key_Meta:
  case Qt::Key_Alt:
  case Qt::Key_AltGr:
  case Qt::Key_Super_L:
  case Qt::Key_Super_R:
  case Qt::Key_Menu:
    return true;
  }
  return false;
}

