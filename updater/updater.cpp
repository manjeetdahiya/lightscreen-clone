#include <QSettings>
#include <QHttp>
#include <QSettings>
#include <QDate>

#include "updater.h"

#define LS_CURRENT_VERSION 0.5

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
  return true; // am checking
}

void Updater::httpDone(bool result)
{
  QByteArray data = mHttp.readAll();

  //qDebug() << "httpDone, error:" << result;
  //qDebug() << data;

  bool ok;

  double version = QString(data).toDouble(&ok);
  //qDebug() << version;

  QSettings().setValue("lastUpdateCheck", QDate::currentDate().dayOfYear());

  /*
  if (result)
    qWarning() << mHttp.errorString();
  */

  if (version > LS_CURRENT_VERSION)
  { // New version available
    emit done(true);
  }
  else
  {
    emit done(false);
  }

}
