#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QtGui/QDialog>
#include "../updater/updater.h"
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
  void checkUpdatesNow();
  void dialogButtonClicked(QAbstractButton *button);
  void flipButtonToggled(bool checked);
  void languageChange(QString language);
  void link(QString url);
  void openUrl(QString url);
  void rejected();
  void saveSettings();
  void updatePreview();

protected:
  bool event(QEvent *event);
#if defined(Q_WS_WIN)
  bool winEvent(MSG *message, long *result);
#endif

private:
  bool hotkeyCollision();
  void initConnections();
  void loadSettings();

private:
    Ui::OptionsDialog ui;

};

#endif // OPTIONSDIALOG_H
