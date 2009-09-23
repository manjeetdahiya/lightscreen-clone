#ifndef LIGHTSCREENWINDOW_H
#define LIGHTSCREENWINDOW_H

#include <QtGui/QDialog>
#include <QPointer>
#include <QSystemTrayIcon>
#include <QSettings>

#include "updater/updater.h"
#include "tools/screenshot.h"

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
  void cleanup(Screenshot::Options options);
  void checkForUpdates();
  void goToFolder();
  void messageClicked();
  void preview(Screenshot* screenshot);
  void restoreSystemTrayNotifier();
  void screenshotAction(int mode = 0);
  void screenshotActionTriggered(QAction* action);
  void showAbout();
  void showHotkeyError(QStringList hotkeys);
  void showOptions(bool updater = false);
  void showScreenshotMessage(bool result, QString fileName);
  void showScreenshotMenu();
  void showTrayNotifier(bool result);
  void toggleVisibility(QSystemTrayIcon::ActivationReason reason = QSystemTrayIcon::DoubleClick);
  void updaterCheckDone(Updater::Result result);
  void updaterCanceled(bool reminder);
  void windowHotkey();

private:
  void applySettings();
  void compressPng(QString fileName);
  void connectHotkeys();
  void createTrayIcon();

protected:
  bool event(QEvent *event);

private:
  bool             mReviveMain;
  bool             mDoCache;
  bool             mWasHidden;
  int              mLastMode;
  QSettings        mSettings;
  QPointer<QSystemTrayIcon> mTrayIcon;
  Ui::LightscreenWindowClass ui;

};

#endif // LIGHTSCREENWINDOW_H

