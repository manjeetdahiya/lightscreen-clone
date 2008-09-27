#include <QDate>
#include <QHttp>
#include <QSettings>

#include "updater.h"

#define LS_CURRENT_VERSION 0.6

Updater::Updater(QObject *parent) : QObject(parent)
{
  connect(&mHttp, SIGNAL(done(bool)), this, SLOT(httpDone(bool)));
}

/*
 * NOTE: Returns true if the check was initiated (not completed).
 */
bool Updater::check()
{
  if (QSettings().value("lastUpdateCheck").toInt()+7 > QDate::currentDate().dayOfYear())
    return false; // If 7 days have not passed since the last update check.

  mHttp.setHost("lightscreen.sourceforge.net");
  mHttp.get("/version");

  return true;
}

void Updater::httpDone(bool result)
{
  QByteArray data = mHttp.readAll();

  bool ok;

  double version = QString(data).toDouble(&ok);

  QSettings().setValue("lastUpdateCheck", QDate::currentDate().dayOfYear());

  if (version > LS_CURRENT_VERSION)
    emit done(true);// New version available
  else
    emit done(false);
}

Updater* Updater::mInstance = 0;

Updater *Updater::instance()
{
  if (!mInstance)
    mInstance = new Updater();

  return mInstance;
}

