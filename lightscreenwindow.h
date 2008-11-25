#ifndef LIGHTSCREENWINDOW_H
#define LIGHTSCREENWINDOW_H

#include <QtGui/QDialog>
#include <QSystemTrayIcon>
#include <QSettings>

#include "ui_lightscreenwindow.h"

class QHttp;
class Updater;
class LightscreenWindow : public QDialog
{
    Q_OBJECT

public:
  LightscreenWindow(QWidget *parent = 0);
  ~LightscreenWindow();

public slots:
  void action(int mode = 3);
  void areaHotkey();
  bool closingWithoutTray();
  void checkForUpdates();
  void goToFolder();
  void instanceMessage(QString message);
  void messageClicked();
  void restoreSystemTrayNotifier();
  void screenshotAction(int mode = 0);
  void screenshotActionTriggered(QAction* action);
  void showAbout();
  void showHotkeyError(QStringList hotkeys);
  void showOptions();
  void showScreenshotMessage(bool result, QString fileName);
  void showTrayNotifier(bool result);
  void toggleVisibility(QSystemTrayIcon::ActivationReason reason = QSystemTrayIcon::DoubleClick);
  void updaterDone(bool result);
  void windowHotkey();

private:
  void applySettings();
  void compressPng(QString fileName);
  void connectHotkeys();
  void createScreenshotButtonMenu();
  void createTrayIcon();

protected:
  //Events:
  void changeEvent(QEvent *event);
  void hideEvent(QHideEvent *event);
  void closeEvent(QCloseEvent *event);
  void showEvent(QShowEvent *event);

private:
  bool             mReviveMain;
  QSettings        mSettings;
  QSystemTrayIcon *mTrayIcon;
  Ui::LightscreenWindowClass ui;

};

#endif // LIGHTSCREENWINDOW_H

