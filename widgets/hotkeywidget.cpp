#include <QApplication>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QKeySequence>
#include <QTimer>

#include "hotkeywidget.h"

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
  if (event->type() == QEvent::LanguageChange)
  {
    setHotkeyText();
  }
  if (event->type() == QEvent::FocusIn)
  {
    setText(tr("Type your hotkey"));
    grabKeyboard();
  }
  if (event->type() == QEvent::FocusOut)
  {
    if (text() == tr("Invalid hotkey"))
    {
      emit invalidHotkeyError();
      showError();
    }

    releaseKeyboard();
    setHotkeyText(); // Reset the text
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

