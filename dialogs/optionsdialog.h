#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QtGui/QDialog>
#include "ui_optionsdialog.h"

class QAbstractButton;
class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    OptionsDialog(QWidget *parent = 0);

public slots:
  void accepted();
  void browse();
  void changePage();
  void checkUpdatesNow();
  void dialogButtonClicked(QAbstractButton *button);
  void flipButtonToggled(bool checked);
  void languageChange(QString language);
  void saveSettings();
  void startupRelatedStateChange(int state);
  void trayRelatedStateChange(int state);


protected:
  void changeEvent(QEvent *event);
#if defined(Q_WS_WIN)
  bool winEvent(MSG *message, long *result);
#endif

private:
  bool hotkeyCollision();

private:
    Ui::OptionsDialog ui;

    void loadSettings();
};

#endif // OPTIONSDIALOG_H
