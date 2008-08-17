#ifndef HOTKEYWIDGET_H
#define HOTKEYWIDGET_H

#include <QPushButton>
#include "../engines/hotkeyengine.h"

class HotkeyWidget : public QPushButton
{
    Q_OBJECT

public:
  HotkeyWidget(QWidget *parent = 0);
    
  void setHotkey(HotkeyEngine::Hotkey hotkey);
  HotkeyEngine::Hotkey hotkey();

signals:
  void invalidHotkeyError();
  
private:
  void setHotkeyText();
  void showError();

private slots:
  void hideError();
  void sysKeyPress(Qt::Key key, QFlags<Qt::KeyboardModifier> keyboardModifiers);
  
protected:
  // Event overrides:
  void focusInEvent(QFocusEvent *event);
  void focusOutEvent(QFocusEvent *event);
  void keyPressEvent(QKeyEvent *e);
  
private:
  HotkeyEngine::Hotkey mHotkey;
  bool mShowingError;
  
};

#endif // HOTKEYWIDGET_H
