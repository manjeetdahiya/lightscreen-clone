/*
 * Qt includes
 */

#include <QErrorMessage>

#include <QDate>
#include <QDesktopServices>
#include <QFileInfo>
#include <QHttp>
#include <QMessageBox>
#include <QMenu>
#include <QProcess>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QUrl>
#include <QSound>

/*
 * Lightscreen includes
 */
#include "lightscreenwindow.h"
#include "dialogs/aboutdialog.h"
#include "dialogs/optionsdialog.h"

#include "engines/screenshotengine.h"
#include "engines/hotkeyengine.h"
#include "engines/action.h"
#include "engines/actionengine.h"

#include "osspecific.h"

#include "updater/updater.h"

LightscreenWindow::LightscreenWindow(QWidget *parent) :
  QDialog(parent)
{

  if (mSettings.value("options/vistaGlass", true).toBool())
    OS::vistaGlass(this);

  ui.setupUi(this);

  // Actions
  connect(ui.optionsPushButton, SIGNAL(clicked()), this, SLOT(showOptions()));

  connect(ui.hidePushButton, SIGNAL(clicked()), this, SLOT(toggleVisibility()));

  createTrayIcon();
  createScreenshotButtonMenu();

  applySettings();

  // Initial hotkeys
  createActions();

  checkForUpdates();
}

/*
 * Slots
 */

void LightscreenWindow::action(int mode)
{
  if (mode == 4)
  {
    goToFolder();
  } else
  {
    show();
  }
}

bool LightscreenWindow::closingWithoutTray()
{
  if (mSettings.value("options/disableHideAlert", QVariant(false)).toBool())
    return false;

  QMessageBox msgBox;
  msgBox.setWindowTitle(tr("Lightscreen"));
  msgBox.setText(tr("You have chosen to hide Lightscreen when there's no system tray icon, so you will not be able to access the program <b>unless you have selected a hotkey to do so</b>.<br>What do you want to do?"));
  msgBox.setIcon(QMessageBox::Warning);

  QPushButton *enableButton = msgBox.addButton(tr("Hide but enable tray"), QMessageBox::ActionRole);
  QPushButton *enableAndDenotifyButton = msgBox.addButton( tr("Hide and don't warn"), QMessageBox::ActionRole);
  QPushButton *hideButton = msgBox.addButton(tr("Just hide"), QMessageBox::ActionRole);
  QPushButton *abortButton = msgBox.addButton(QMessageBox::Cancel);

  Q_UNUSED(abortButton);

  msgBox.exec();

  if (msgBox.clickedButton() == hideButton)
  {
    return false;
  } else if (msgBox.clickedButton() == enableAndDenotifyButton)
  {
    mSettings.setValue("options/disableHideAlert", true);
    applySettings();
    return false;
  } else if (msgBox.clickedButton() == enableButton)
  {
    mSettings.setValue("options/tray", true);
    applySettings();
    return false;
  }

  return true;// Cancel
}

void LightscreenWindow::goToFolder()
{
  QString folder = mSettings.value("file/target").toString();

  if (folder.isEmpty())
  {
    QDesktopServices::openUrl(QUrl(QApplication::applicationDirPath()));
  } else
  {
    QDesktopServices::openUrl(QUrl(folder));
  }
}

void LightscreenWindow::messageClicked()
{
  goToFolder();
}

void LightscreenWindow::restoreSystemTrayNotifier()
{
  mTrayIcon->setIcon(QIcon(":/icons/SystemTray"));
}

void LightscreenWindow::screenshotAction(int mode)
{
  //qDebug() << "screenshotAction";

  if (!mScreenshotEngine.isEnabled())
  {
    return;
  }

  // Applying pre-screenshot settings
  if (mSettings.value("options/hide").toBool())
    mTrayIcon->hide();

  bool shouldHideWindow = false;
  static int lastMode = -1;
  static bool lastShouldHide = false;
  int delayms = -1;

  if (mSettings.value("options/hide").toBool())
  {
    shouldHideWindow = isVisible();

    //qDebug() << "shouldHideWindow:" << shouldHideWindow;

    //Hiding the main window for the screenshot
    if (shouldHideWindow)
      setVisible(false);

    // Screenshot delay
    delayms = mSettings.value("options/delay", 0).toInt();
    delayms = delayms*1000; // Converting the delay to milliseconds.

    // When on Windows Vista, the window takes a little bit longer to hide
    if (QSysInfo::WindowsVersion == QSysInfo::WV_VISTA)
    {
      delayms += 400;
    } else
    {
      delayms += 200;
    }

    // The delayed functions works using static varibles lastMode and lastShouldHide
    // which keep the argument so a QTimer can call this function again.
    if (delayms > 0)
    {
      if (lastMode < 0)
      {
        //qDebug() << "activating delay";
        lastMode = mode;
        lastShouldHide = shouldHideWindow;

        //qDebug() << "delayms:" << delayms;

        QTimer::singleShot(delayms, this, SLOT(screenshotAction()));
        return;
      } else
      {
        //qDebug() << "mode = lastMode";
        mode = lastMode;
        shouldHideWindow = lastShouldHide;
        lastMode = -1;
      }
    }

  }

  // Populating the option object that will then be passed to the screenshot engine
  ScreenshotEngine::Options options;
  options.format = mSettings.value("file/format").toInt();
  options.prefix = mSettings.value("file/prefix").toString();
  options.directory = QDir(mSettings.value("file/target").toString());
  options.naming = mSettings.value("file/naming").toInt();
  options.mode = mode;
  options.quality = mSettings.value("options/quality" , 100).toInt();
  options.flipNaming = mSettings.value("options/flip" , false).toBool();
  options.directX = mSettings.value("options/dxScreen", false).toBool();

  // Taking the screenshot and saving the result.
  ScreenshotEngine::Result screenshotResult;
  screenshotResult = mScreenshotEngine.take(options);

  // Reversing settings
  if (mSettings.value("options/hide").toBool())
  {
    mTrayIcon->show();

    if (shouldHideWindow)
      setVisible(true);

    lastMode = -1;
  }
  // Showing message.
  if (mSettings.value("options/message").toBool())
    showScreenshotMessage(screenshotResult.result, screenshotResult.fileName);

  if (mSettings.value("options/tray").toBool())
    showTrayNotifier(screenshotResult.result);

  if (mSettings.value("options/playSound", false).toBool())
  {
    if (screenshotResult.result)
    {
      QSound sound("Media/notify.wav");
      sound.play();
    } else
    {
      QSound sound("Media/chords.wav");
      sound.play();
    }
  }

  if (!screenshotResult.result)
    return;

  if (mSettings.value("options/optipng").toBool() && mSettings.value("options/format").toInt() == ScreenshotEngine::PNG)
    compressPng(screenshotResult.fileName);

  //qDebug() << "/screenshotAction <<<<<<<<<<";
}

void LightscreenWindow::screenshotActionTriggered(QAction* action)
{
  //qDebug() << "Action from BUTTON.. (Mode:" << action->data().toInt() << ")";
  screenshotAction(action->data().toInt());
}

void LightscreenWindow::showOptions()
{
  ActionEngine::instance()->clear(this);
  mScreenshotEngine.setEnabled(false);
  OptionsDialog d;
  d.exec();
  mScreenshotEngine.setEnabled(true);

  applySettings();
}

void LightscreenWindow::showScreenshotMessage(bool result, QString fileName)
{
  // Showing message.
  QString title;
  QString message;

  if (result)
  {
    title = QFileInfo(fileName).fileName();

    if (mSettings.value("file/target").toString().isEmpty())
    {
      message
          = QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
    } else
    {
      message = tr("Saved to %1%2").arg(QDir::toNativeSeparators(mSettings.value("file/target").toString())).arg(QDir::separator());
    }
  } else
  {
    title = tr("The screenshot was not taken");
    message = tr("There was an error or you did not select a valid area.");
  }

  mTrayIcon->showMessage(title, message);
}

void LightscreenWindow::showTrayNotifier(bool result)
{
  if (result)
  {
    mTrayIcon->setIcon(QIcon(":/icons/SystemTraySucess"));
  } else
  {
    mTrayIcon->setIcon(QIcon(":/icons/SystemTrayFailure"));
  }

  QTimer::singleShot(1500, this, SLOT(restoreSystemTrayNotifier()));
}

void LightscreenWindow::showAbout()
{
  AboutDialog aboutDialog;
  aboutDialog.exec();
}

void LightscreenWindow::showHotkeyError(QStringList hotkeys)
{
  static bool dontShow = false;

  if (dontShow)
    return;

  QString messageText;
  QString changeString = tr("Change the hotkey");
  QString disableString = tr("Disable the hotkey");

  messageText
      = tr("Some hotkeys could not be registered, they might already be in use");

  if (hotkeys.count() > 1)
  {
    changeString += "s";
    disableString += "s";

    messageText += tr("<br>The failed hotkeys are the following:<ul>");

    foreach(QString hotkey, hotkeys)
    {
      messageText += QString("%1%2%3").arg("<li><b>").arg(hotkey).arg("</b></li>");
    }

    messageText += tr("</ul>");
  } else
  {
    messageText += tr("<br>The failed hotkey is the <b>%1</b>").arg(hotkeys[0]);
  }

  messageText += tr("<br><i>What do you want to do?</i>");

  QMessageBox msgBox(this);
  msgBox.setWindowTitle(tr("Lightscreen"));
  msgBox.setText(messageText);

  QPushButton *changeButton = msgBox.addButton(changeString,
      QMessageBox::ActionRole);
  QPushButton *disableButton = msgBox.addButton(disableString,
      QMessageBox::ActionRole);
  QPushButton *exitButton = msgBox.addButton(tr("Quit Lightscreen"),
      QMessageBox::ActionRole);

  msgBox.exec();

  if (msgBox.clickedButton() == exitButton)
  {
    //qWarning() << "Exiting due to hotkey conflict.";
    dontShow = true;
    QTimer::singleShot(10, this, SLOT(accept()));;
  } else if (msgBox.clickedButton() == changeButton)
  {
    showOptions();
  } else if (msgBox.clickedButton() == disableButton)
  {
    foreach(QString hotkey, hotkeys)
    {
      mSettings.setValue(QString("actions/%1/enabled").arg(hotkey), false);
    }
  }
}

void LightscreenWindow::toggleVisibility(
    QSystemTrayIcon::ActivationReason reason)
{
  if (reason != QSystemTrayIcon::DoubleClick)
    return;

  if (isVisible())
  {
    if (mSettings.value("options/tray").toBool() == false && closingWithoutTray())
      return;

    hide();
  } else
  {
    show();

#ifdef Q_WS_WIN
    SetForegroundWindow(winId());
#endif
  }

  if (mSettings.value("options/vistaGlass", true).toBool())
    OS::vistaGlass(this); // Fixes black background bug?

}

void LightscreenWindow::updaterDone(bool result)
{
  if (!result)
    return;

  QMessageBox msgBox;
  msgBox.setWindowTitle(tr("Lightscreen"));
  msgBox.setText(tr("There's a new version of Lightscreen available.<br>Would you like to see more information?<br>(<em>You can turn this notification off</em>)"));
  msgBox.setIcon(QMessageBox::Information);

  QPushButton *yesButton = msgBox.addButton(QMessageBox::Yes);
  QPushButton *turnOffButton = msgBox.addButton(tr("Turn Off"),
      QMessageBox::ActionRole);
  QPushButton *remindButton = msgBox.addButton(tr("Remind Me Later"),
      QMessageBox::RejectRole);

  Q_UNUSED(remindButton);

  msgBox.exec();

  if (msgBox.clickedButton() == yesButton)
  {
    QDesktopServices::openUrl(QUrl("http://lightscreen.sourceforge.net/new-version"));
  } else if (msgBox.clickedButton() == turnOffButton)
  {
    mSettings.setValue("disableUpdater", true);
  }

}

/*
 * Private 
 */

void LightscreenWindow::applySettings()
{
  if (!QSettings().contains("file/format"))
    showOptions(); // There are no options (or the options config is invalid or incomplete)

  mSettings.sync();

  mTrayIcon->setVisible(mSettings.value("options/tray").toBool());

  createActions();

#ifdef Q_WS_WIN
  // Windows startup settings

  QSettings init("Microsoft", "Windows");
  init.beginGroup("CurrentVersion");
  init.beginGroup("Run");

  if (mSettings.value("options/startup").toBool())
  {
    QString entry = QDir::toNativeSeparators(QApplication::applicationFilePath());

    if (mSettings.value("options/startupHide").toBool())
    entry.append(" -h"); // command to automatically hide.

    //qDebug() << "Creating LS run entry";
    //qDebug() << entry;

    init.setValue("Lightscreen", entry);
  }
  else
  {
    //qDebug() << "Removing LS run entry";
    init.remove("Lightscreen");
  }

  init.endGroup();
  init.endGroup();
#endif
}

void LightscreenWindow::compressPng(QString fileName)
{
  QStringList args;
  args << fileName;

  QProcess* optipng = new QProcess(this);
  connect(optipng, SIGNAL(finished(int, QProcess::ExitStatus)), optipng, SLOT(deleteLater()));

  optipng->setWorkingDirectory(QCoreApplication::applicationDirPath());
  optipng->start("optipng", args);

  //qDebug() << "Calling OptiPNG";
}

void LightscreenWindow::createActions()
{
  QStringList actionNameList;
  actionNameList << tr("screen") << tr("window") << tr("area") << tr("open")
      << tr("directory");

  bool error = false;
  QStringList failures;

  ActionEngine::instance()->clear(this);

  mSettings.beginGroup("actions");
  Q_FOREACH(QString action, actionNameList)
  {
    mSettings.beginGroup(action);
    if (mSettings.value("enabled").toBool())
    {
      int code;

      HotkeyEngine::Hotkey key;
      key.key = HotkeyEngine::indexToKey(mSettings.value("index").toInt());
      key.ctrl = mSettings.value("ctrl").toBool();
      key.alt = mSettings.value("alt").toBool();
      key.shift = mSettings.value("shift").toBool();
      code = HotkeyEngine::registerHotkey(this, key);

      if (code> 0)
      {
        Action *screenshotAction = new Action(code);

        int mode = actionNameList.indexOf(action);

        screenshotAction->setMode(mode);

        if (mode < 3)
        {
          connect(screenshotAction, SIGNAL(triggered(int)), this, SLOT(screenshotAction(int)));
        }
        else
        {
          connect(screenshotAction, SIGNAL(triggered(int)), this, SLOT(action(int)));
        }

        ActionEngine::instance()->add(screenshotAction);
      }
      else
      {
        //qDebug() << "Hotkey error?";
        error = true;
        failures << action;
      }
    }

    mSettings.endGroup();
  }

  mSettings.endGroup();

  if (error)
    showHotkeyError(failures);

}

void LightscreenWindow::createScreenshotButtonMenu()
{
  QAction *screenAction = new QAction(tr("&Screen"), this);
  screenAction->setData(QVariant(0));

  QAction *windowAction = new QAction(tr("&Window"), this);
  windowAction->setData(QVariant(1));

  QAction *areaAction = new QAction(tr("&Area"), this);
  areaAction->setData(QVariant(2));

  QAction *goAction = new QAction(tr("&Go to Folder"), this);
  connect(goAction, SIGNAL(triggered()), this, SLOT(goToFolder()));

  QActionGroup *screenshotGroup = new QActionGroup(this);
  screenshotGroup->addAction(screenAction);
  screenshotGroup->addAction(windowAction);
  screenshotGroup->addAction(areaAction);

  connect(screenshotGroup, SIGNAL(triggered(QAction*)), this, SLOT(screenshotActionTriggered(QAction*)));

  QMenu *buttonMenu = new QMenu;
  buttonMenu->addAction(screenAction);
  buttonMenu->addAction(windowAction);
  buttonMenu->addAction(areaAction);
  buttonMenu->addSeparator();
  buttonMenu->addAction(goAction);

  ui.screenshotPushButton->setMenu(buttonMenu);
}

void LightscreenWindow::createTrayIcon()
{
  mTrayIcon = new QSystemTrayIcon(QIcon(":/icons/SystemTray"), this);

  connect(mTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(toggleVisibility(QSystemTrayIcon::ActivationReason)));
  connect(mTrayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));

  QAction *hideAction = new QAction(tr("Show&/Hide"), mTrayIcon);
  connect(hideAction, SIGNAL(triggered()), this, SLOT(toggleVisibility()));

  QAction *screenAction = new QAction(tr("&Screen"), mTrayIcon);
  screenAction->setData(QVariant(0));

  QAction *windowAction = new QAction(tr("&Window"), mTrayIcon);
  windowAction->setData(QVariant(1));

  QAction *areaAction = new QAction(tr("&Area"), mTrayIcon);
  areaAction->setData(QVariant(2));

  QActionGroup *screenshotGroup = new QActionGroup(mTrayIcon);
  screenshotGroup->addAction(screenAction);
  screenshotGroup->addAction(windowAction);
  screenshotGroup->addAction(areaAction);

  connect(screenshotGroup, SIGNAL(triggered(QAction*)), this, SLOT(screenshotActionTriggered(QAction*)));

  QAction *optionsAction = new QAction(tr("View &Options"), mTrayIcon);
  connect(optionsAction, SIGNAL(triggered()), this, SLOT(showOptions()));

  QAction *goAction = new QAction(tr("&Go to Folder"), mTrayIcon);
  connect(goAction, SIGNAL(triggered()), this, SLOT(goToFolder()));

  QAction *aboutAction = new QAction(tr("&About Lightscreen"), mTrayIcon);
  connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));

  QAction *quitAction = new QAction(tr("&Quit"), mTrayIcon);
  connect(quitAction, SIGNAL(triggered()), this, SLOT(accept()));

  QMenu* screenshotMenu = new QMenu("Screenshot");
  screenshotMenu->addAction(screenAction);
  screenshotMenu->addAction(windowAction);
  screenshotMenu->addAction(areaAction);

  QMenu * trayIconMenu = new QMenu;
  trayIconMenu->addAction(hideAction);
  trayIconMenu->addSeparator();
  trayIconMenu->addMenu(screenshotMenu);
  trayIconMenu->addAction(optionsAction);
  trayIconMenu->addAction(goAction);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(aboutAction);
  trayIconMenu->addAction(quitAction);

  mTrayIcon->setContextMenu(trayIconMenu);
}

void LightscreenWindow::checkForUpdates()
{
  if (mSettings.value("disableUpdater", false).toBool())
    return;

  mUpdater = new Updater(this);
  connect(mUpdater, SIGNAL(done(bool)), this, SLOT(updaterDone(bool)));
  mUpdater->check();
}

// Events

void LightscreenWindow::hideEvent(QHideEvent *event)
{
  mSettings.setValue("geometry", saveGeometry());
  QDialog::hideEvent(event);
}


void LightscreenWindow::closeEvent(QCloseEvent *event)
{
  mSettings.setValue("geometry", saveGeometry());
  QDialog::closeEvent(event);
}

void LightscreenWindow::showEvent(QShowEvent *event)
{
  restoreGeometry(mSettings.value("geometry").toByteArray());
}
