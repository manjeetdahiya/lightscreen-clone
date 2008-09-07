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
  void focusInEvent(QFocusEvent *event);
  void focusOutEvent(QFocusEvent *event);
  void keyPressEvent(QKeyEvent *e);

private:
  QKeySequence mHotkey;
  bool mShowingError;

  QKeySequence getKeySequence(QKeyEvent* event) const;
  bool isValid(int key) const;
  bool isModifier(int key) const;

};

#endif // HOTKEYWIDGET_H
