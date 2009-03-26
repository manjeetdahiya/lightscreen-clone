/*
 * Qt includes
 */

#include <QDate>
#include <QDesktopServices>
#include <QFileInfo>
#include <QHttp>
#include <QMenu>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QSound>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QUrl>

/*
 * Lightscreen includes
 */
#include "lightscreenwindow.h"
#include "dialogs/aboutdialog.h"
#include "dialogs/optionsdialog.h"

#include "tools/globalshortcut/globalshortcutmanager.h"
#include "tools/os.h"
#include "tools/screenshot.h"

#include "updater/updater.h"

LightscreenWindow::LightscreenWindow(QWidget *parent) :
  QDialog(parent)
{
  os::aeroGlass(this);

  setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint); // Remove the what's this button, no real use in the main window.

  os::translate(mSettings.value("options/language").toString().toLower());

  ui.setupUi(this);

  // Actions
  connect(ui.optionsPushButton, SIGNAL(clicked()), this, SLOT(showOptions()));
  connect(ui.hidePushButton   , SIGNAL(clicked()), this, SLOT(toggleVisibility()));

  createTrayIcon();
  createScreenshotButtonMenu();

  if (!mSettings.contains("file/format"))
    showOptions(); // There are no options (or the options config is invalid or incomplete)

  applySettings();

  checkForUpdates();
}

LightscreenWindow::~LightscreenWindow()
{
  GlobalShortcutManager::instance()->clear();
}

/*
 * Slots
 */

void LightscreenWindow::action(int mode)
{
  if (mode == 4)
  {
    goToFolder();
  }
  else
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

  QPushButton *enableButton = msgBox.addButton(tr("Hide but enable tray"),
      QMessageBox::ActionRole);
  QPushButton *enableAndDenotifyButton = msgBox.addButton(tr("Hide and don't warn"),
      QMessageBox::ActionRole);
  QPushButton *hideButton = msgBox.addButton(tr("Just hide"),
      QMessageBox::ActionRole);
  QPushButton *abortButton = msgBox.addButton(QMessageBox::Cancel);

  Q_UNUSED(abortButton);

  msgBox.exec();

  if (msgBox.clickedButton() == hideButton)
  {
    return false;
  }
  else if (msgBox.clickedButton() == enableAndDenotifyButton)
  {
    mSettings.setValue("options/disableHideAlert", true);
    applySettings();
    return false;
  }
  else if (msgBox.clickedButton() == enableButton)
  {
    mSettings.setValue("options/tray", true);
    applySettings();
    return false;
  }

  return true; // Cancel
}

void LightscreenWindow::goToFolder()
{
  QString folder = mSettings.value("file/target").toString();

  if (folder.isEmpty())
    folder = qApp->applicationDirPath();

  if (QDir::toNativeSeparators(folder.at(folder.size()-1)) != QDir::separator())
    folder.append(QDir::separator());

  QDesktopServices::openUrl(QUrl::fromLocalFile(folder));
}

void LightscreenWindow::messageClicked()
{
  goToFolder();
}

void LightscreenWindow::restoreSystemTrayNotifier()
{
  mTrayIcon->setIcon(QIcon(":/icons/SystemTray"));
  setWindowTitle(tr("Lightscreen"));
}

void LightscreenWindow::screenshotAction(int mode)
{
  bool image  = false;
  bool result = false;
  QString fileName;
  static int lastMode = -1;
  int delayms = -1;
  static bool wasHidden;

  if (lastMode == -1)
    wasHidden = !isVisible();

  bool optionsHide = mSettings.value("options/hide").toBool(); // Option cache, used a couple of times.
  static Screenshot::Options options;

  // Applying pre-screenshot settings
  if (optionsHide && !wasHidden)
  {
    setVisible(false);
    mTrayIcon->hide();
  }

  // Screenshot delay
  delayms = mSettings.value("options/delay", 0).toInt();
  delayms = delayms * 1000; // Converting the delay to milliseconds.

  delayms += 200;

#if defined(Q_WS_WIN)
  if (optionsHide)
  {
    // When on Windows Vista, the window takes a little bit longer to hide
    if (QSysInfo::WindowsVersion == QSysInfo::WV_VISTA)
      delayms += 200;
  }
#endif

  // The delayed functions works using the static variable lastMode
  // which keeps the argument so a QTimer can call this function again.
  if (delayms > 0)
  {
    if (lastMode < 0)
    {
      lastMode = mode;

      QTimer::singleShot(delayms, this, SLOT(screenshotAction()));
      return;
    }
    else
    {
      mode = lastMode;
      lastMode = -1;
    }
  }

  if (!mDoCache)
  {
    // Populating the option object that will then be passed to the screenshot engine
    options.file          = mSettings.value("file/enabled").toBool();
    options.format        = mSettings.value("file/format").toInt();
    options.prefix        = mSettings.value("file/prefix").toString();
    options.directory     = QDir(mSettings.value("file/target").toString());
    options.naming        = mSettings.value("file/naming").toInt();
    options.flipNaming    = mSettings.value("options/flip", false).toBool();
    options.quality       = mSettings.value("options/quality", 100).toInt();
    options.currentMonitor= mSettings.value("options/currentMonitor", false).toBool();
    options.clipboard     = mSettings.value("options/clipboard", true).toBool();
    options.preview       = mSettings.value("options/preview", false).toBool();
    options.magnify       = mSettings.value("options/magnify", false).toBool();
    options.cursor        = mSettings.value("options/cursor" , false).toBool();
    options.saveAs        = mSettings.value("options/saveAs" , false).toBool();

    mDoCache = true;
  }

  options.mode = mode;

  // Taking the screenshot and saving the result.
  Screenshot screenshot(options);

  image = screenshot.take();

  if (image && options.file)
  {
    fileName = screenshot.save();
    result   = !(fileName.isEmpty());
  }
  else
  {
    result = image;
  }

  // Reversing settings
  if (mSettings.value("options/hide").toBool())
  {
    if (mSettings.value("options/tray").toBool())
      mTrayIcon->show();

    if (!wasHidden)
      setVisible(true);
  }

  if (mSettings.value("options/tray").toBool())
  {
    showTrayNotifier(result);

    if (mSettings.value("options/message").toBool())
      showScreenshotMessage(result, fileName);
  }

  if (mSettings.value("options/playSound", false).toBool())
  {
    if (result)
      QSound("sounds/ls.screenshot.wav").play();
    else
      QSound("sounds/ls.error.wav").play();
  }

  if (result
   && mSettings.value("options/optipng").toBool()
   && mSettings.value("file/format").toInt() == Screenshot::PNG)
    compressPng(fileName);

  lastMode = -1;
}

void LightscreenWindow::screenshotActionTriggered(QAction* action)
{
  screenshotAction(action->data().toInt());
}

void LightscreenWindow::showOptions()
{
  GlobalShortcutManager::clear();
  OptionsDialog optionsDialog(this);
  optionsDialog.exec();

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
      message = QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
    }
    else
    {
      message = tr("Saved to %1%2").arg(QDir::toNativeSeparators(
          mSettings.value("file/target").toString())).arg(QDir::separator());
    }
  }
  else
  {
    title   = tr("The screenshot was not taken");
    message = tr("There was an error or you did not select a valid area.");
  }

  mTrayIcon->showMessage(title, message);
}

void LightscreenWindow::showTrayNotifier(bool result)
{
  if (result)
  {
    mTrayIcon->setIcon(QIcon(":/icons/SystemTraySucess"));
    setWindowTitle(tr("Sucess!"));
  }
  else
  {
    mTrayIcon->setIcon(QIcon(":/icons/SystemTrayFailure"));
    setWindowTitle(tr("Failed!"));
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

   messageText = tr("Some hotkeys could not be registered, they might already be in use");

   if (hotkeys.count() > 1)
   {
     messageText += tr("<br>The failed hotkeys are the following:") + "<ul>";

     foreach(const QString &hotkey, hotkeys)
     {
       messageText += QString("%1%2%3").arg("<li><b>").arg(hotkey).arg("</b></li>");
     }

    messageText += "</ul>";
   }
   else
   {
    messageText += tr("<br>The failed hotkey is <b>%1</b>").arg(hotkeys[0]);
   }

   messageText += tr("<br><i>What do you want to do?</i>");

   QMessageBox msgBox(this);
   msgBox.setWindowTitle(tr("Lightscreen"));
   msgBox.setText(messageText);

   QPushButton *changeButton  = msgBox.addButton(tr("Change") , QMessageBox::ActionRole);
   QPushButton *disableButton = msgBox.addButton(tr("Disable"), QMessageBox::ActionRole);
   QPushButton *exitButton    = msgBox.addButton(tr("Quit")   , QMessageBox::ActionRole);

   msgBox.exec();

   if (msgBox.clickedButton() == exitButton)
   {
     dontShow = true;
     QTimer::singleShot(10, this, SLOT(accept()));;
   }
   else if (msgBox.clickedButton() == changeButton)
   {
    showOptions();
   }
   else if (msgBox.clickedButton() == disableButton)
   {
     foreach(const QString &hotkey, hotkeys)
     {
      mSettings.setValue(QString("actions/%1/enabled").arg(hotkey), false);
     }
   }

}

void LightscreenWindow::toggleVisibility(QSystemTrayIcon::ActivationReason reason)
{
  if (reason != QSystemTrayIcon::DoubleClick)
    return;

  if (isVisible())
  {
    if (mSettings.value("options/tray").toBool() == false
        && closingWithoutTray())
      return;

    hide();
  }
  else
  {
    show();
  }
}

void LightscreenWindow::updaterDone(bool result)
{
  mSettings.setValue("lastUpdateCheck", QDate::currentDate().dayOfYear());

  if (!result)
    return;

  QMessageBox msgBox;
  msgBox.setWindowTitle(tr("Lightscreen"));
  msgBox.setText(tr("There's a new version of Lightscreen available.<br>Would you like to see more information?<br>(<em>You can turn this notification off</em>)"));
  msgBox.setIcon(QMessageBox::Information);

  QPushButton *yesButton     = msgBox.addButton(QMessageBox::Yes);
  QPushButton *turnOffButton = msgBox.addButton(tr("Turn Off"), QMessageBox::ActionRole);
  QPushButton *remindButton  = msgBox.addButton(tr("Remind Me Later"), QMessageBox::RejectRole);

  Q_UNUSED(remindButton);

  msgBox.exec();

  if (msgBox.clickedButton() == yesButton)
    QDesktopServices::openUrl(QUrl("http://lightscreen.sourceforge.net/new-version"));
  else if (msgBox.clickedButton() == turnOffButton)
    mSettings.setValue("disableUpdater", true);

}

// Aliases
void LightscreenWindow::windowHotkey() { screenshotAction(1); }
void LightscreenWindow::areaHotkey()   { screenshotAction(2); }

/*
 * Private
 */

void LightscreenWindow::applySettings()
{
  mSettings.sync();

  mTrayIcon->setVisible(mSettings.value("options/tray").toBool());

  connectHotkeys();

  mDoCache = false;

  os::setStartup(mSettings.value("options/startup").toBool(), mSettings.value("options/startupHide").toBool());
}

void LightscreenWindow::compressPng(QString fileName)
{
#if defined(Q_OS_UNIX)
  QProcess::startDetached("optipng " + fileName);
#else
  QStringList args;
  args << fileName;

  QProcess* optipng = new QProcess(this);

  // Delete the QProcess once it's done.
  connect(optipng, SIGNAL(finished(int, QProcess::ExitStatus)), optipng, SLOT(deleteLater()));

  optipng->start("optipng", args);
#endif
}

void LightscreenWindow::connectHotkeys()
{
  // Set to true because if the hotkey is disabled it will show an error.
  bool screen = true, area = true, window = true, open = true, directory = true;

  if (mSettings.value("actions/screen/enabled").toBool())
    screen = GlobalShortcutManager::instance()->connect(mSettings.value(
        "actions/screen/hotkey").value<QKeySequence> (), this, SLOT(screenshotAction()));

  if (mSettings.value("actions/area/enabled").toBool())
    area   = GlobalShortcutManager::instance()->connect(mSettings.value(
        "actions/area/hotkey").value<QKeySequence> (), this, SLOT(areaHotkey()));

  if (mSettings.value("actions/window/enabled").toBool())
    window = GlobalShortcutManager::instance()->connect(mSettings.value(
        "actions/window/hotkey").value<QKeySequence> (), this, SLOT(windowHotkey()));

  if (mSettings.value("actions/open/enabled").toBool())
    open   = GlobalShortcutManager::instance()->connect(mSettings.value(
        "actions/open/hotkey").value<QKeySequence> (), this, SLOT(show()));

  if (mSettings.value("actions/directory/enabled").toBool())
    directory = GlobalShortcutManager::instance()->connect(mSettings.value(
        "actions/directory/hotkey").value<QKeySequence> (), this, SLOT(goToFolder()));

  QStringList failed;
  if (!screen)    failed << "screen";
  if (!area)      failed << "area";
  if (!window)    failed << "window";
  if (!open)      failed << "open";
  if (!directory) failed << "directory";

  if (!failed.isEmpty())
    showHotkeyError(failed);
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
  screenshotGroup->addAction(areaAction);
  screenshotGroup->addAction(windowAction);

  connect(screenshotGroup, SIGNAL(triggered(QAction*)), this, SLOT(screenshotActionTriggered(QAction*)));

  QMenu *buttonMenu = new QMenu;
  buttonMenu->addAction(screenAction);
  buttonMenu->addAction(areaAction);
  buttonMenu->addAction(windowAction);
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

  if (QSettings().value("lastUpdateCheck").toInt() + 7
      > QDate::currentDate().dayOfYear())
    return; // If 7 days have not passed since the last update check.

  connect(Updater::instance(), SIGNAL(done(bool)), this, SLOT(updaterDone(bool)));
  Updater::instance()->check();
}

// Event handling

bool LightscreenWindow::event(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    ui.retranslateUi(this);

  if (event->type() == QEvent::Show)
    os::aeroGlass(this);

  return QDialog::event(event);
}

