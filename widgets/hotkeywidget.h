#ifndef HOTKEYWIDGET_H
#define HOTKEYWIDGET_H

#include <QPushButton>

class HotkeyWidget : public QPushButton
{
    Q_OBJECT

public:
  HotkeyWidget(QWidget *parent = 0);

  void setHotkey(QKeySequence hotkey);
  QKeySequence hotkey();

signals:
  void invalidHotkeyError();

private:
  void setHotkeyText();
  void showError();

private slots:
  void hideError();
 // void sysKeyPress(Qt::Key key, QFlags<Qt::KeyboardModifier> keyboardModifiers);

protected:
  // Event overrides:
  bool event(QEvent* event);
#ifdef Q_WS_WIN
  bool winEvent(MSG * message, long * result);
#endif

private:
  QKeySequence mHotkey;
  bool mShowingError;

  QKeySequence getKeySequence(QKeyEvent* event) const;
  bool isValid(int key) const;
  bool isModifier(int key) const;

};

#endif // HOTKEYWIDGET_H
