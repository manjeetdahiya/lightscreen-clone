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
  void dialogButtonClicked(QAbstractButton *button);
  void flipButtonToggled(bool checked);
  void openUrl(QString url);
  void saveSettings();
  void startupRelatedStateChange(int state);
  void trayRelatedStateChange(int state);

private:
  bool hotkeyCollision();

private:
    Ui::OptionsDialog ui;

    void loadSettings();
};

#endif // OPTIONSDIALOG_H
