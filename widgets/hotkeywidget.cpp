
#include <QCoreApplication>
#include <QTimer>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QFlags>
#include <QTimeLine>

#include "../engines/actionengine.h"
#include "../engines/hotkeyengine.h"
#include "hotkeywidget.h"

HotkeyWidget::HotkeyWidget(QWidget *parent)
    : QPushButton(parent)
{
  mHotkey.ctrl = false;
  mHotkey.alt  = false;
  mHotkey.shift = false;
  mHotkey.key  = HotkeyEngine::KPrintScreen;
  
  mShowingError = false;
   
  setText(tr("Click to select hotkey..."));
  connect(ActionEngine::instance(), SIGNAL(sysKeyPress(Qt::Key, QFlags<Qt::KeyboardModifier>)), this, SLOT(sysKeyPress(Qt::Key, QFlags<Qt::KeyboardModifier>)));
}

void HotkeyWidget::setHotkey(HotkeyEngine::Hotkey hotkey)
{
  mHotkey = hotkey;
  setHotkeyText();
}

HotkeyEngine::Hotkey HotkeyWidget::hotkey()
{
  return mHotkey;
}

void HotkeyWidget::setHotkeyText()
{    
    QString hotkeyText;
    
    if (mHotkey.ctrl)
      hotkeyText += "Ctrl+";
    
    if (mHotkey.shift)
      hotkeyText += "Shift+";
    
    if (mHotkey.alt)
      hotkeyText += "Alt+";
    
    hotkeyText += HotkeyEngine::indexToString(HotkeyEngine::keyToIndex(mHotkey.key));
    
    setText(hotkeyText);
    parentWidget()->setFocus();
}

void HotkeyWidget::focusInEvent(QFocusEvent *event)
{
  Q_UNUSED(event);
  setText(tr("Type your hotkey"));
}

void HotkeyWidget::focusOutEvent(QFocusEvent *event)
{
  Q_UNUSED(event);
  
  if (text() == tr("Invalid hotkey"))
  {
    emit invalidHotkeyError();
    showError();
  }
  
  setHotkeyText(); // Reset the text
}

void HotkeyWidget::keyPressEvent(QKeyEvent *event)
{  
  if (event->key() == Qt::Key_Control
      || event->key() == Qt::Key_Shift
      || event->key() == Qt::Key_Alt)
  {
      //qDebug() << "Just a modifier";
      return;
  }
    
  if (event->key() == Qt::Key_Escape)
  {
    parentWidget()->setFocus();
    return;
  }
  
  HotkeyEngine::Hotkey h;
  
  h.ctrl  = event->modifiers().testFlag(Qt::ControlModifier);
  h.alt   = event->modifiers().testFlag(Qt::AltModifier);
  h.shift = event->modifiers().testFlag(Qt::ShiftModifier);
  // Nice... NOT:
  int hotkeyIndex = HotkeyEngine::vkToIndex(event->nativeVirtualKey());
  h.key   = HotkeyEngine::indexToKey(hotkeyIndex);
    
 
  //qDebug() << QString("ctrl: %1 alt: %2 shift: %3... key: %4").arg(h.ctrl).arg(h.alt).arg(h.shift).arg(h.key);
  
  if (h.key == HotkeyEngine::KPrintScreen && event->key() != Qt::Key_Print)
  {
    showError();
    parentWidget()->setFocus(); // Loose our focus
    return;
  }
  
  mHotkey = h;
  
  setHotkeyText();
  
  event->ignore();
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

void HotkeyWidget::sysKeyPress(Qt::Key key, QFlags<Qt::KeyboardModifier> keyboardModifiers)
{
  
  //qDebug() << "sysKeyPress";
  if (hasFocus()) 
  {
    //qDebug() << "posting sysKeyPress";
    QCoreApplication::postEvent(this, new QKeyEvent(QEvent::KeyPress, key, keyboardModifiers));
  }
  
}
