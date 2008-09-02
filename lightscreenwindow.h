#ifndef LIGHTSCREENWINDOW_H
#define LIGHTSCREENWINDOW_H

#include <QtGui/QDialog>
#include <QSystemTrayIcon>
#include <QSettings>

#include "ui_lightscreenwindow.h"
#include "engines/screenshotengine.h"

class QHttp;
class Updater;
class LightscreenWindow : public QDialog
{
    Q_OBJECT

public:
  LightscreenWindow(QWidget *parent = 0);

public slots:
  void action(int mode = 3);
  bool closingWithoutTray();
  void goToFolder();
  void messageClicked();
  void restoreSystemTrayNotifier();
  void screenshotAction(int mode = 0);
  void screenshotActionTriggered(QAction* action);
  void showAbout();
  void showScreenshotMessage(bool result, QString fileName);
  void showTrayNotifier(bool result);
  void showHotkeyError(QStringList hotkeys);
  void showOptions();
  void toggleVisibility(QSystemTrayIcon::ActivationReason reason = QSystemTrayIcon::DoubleClick);
  void updaterDone(bool result);

private:
  void applySettings();
  void compressPng(QString fileName);
  void createActions();
  void createScreenshotButtonMenu();
  void createTrayIcon();
  void checkForUpdates();

protected:
  //Events:
  void hideEvent(QHideEvent *event);
  void closeEvent(QCloseEvent *event);
  void showEvent(QShowEvent *event);

private:
  bool             mReviveMain;
  ScreenshotEngine mScreenshotEngine;
  QSettings        mSettings;
  QSystemTrayIcon *mTrayIcon;
  Ui::LightscreenWindowClass ui;
  Updater         *mUpdater;


};

#endif // LIGHTSCREENWINDOW_H

