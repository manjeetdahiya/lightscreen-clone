#include <QDate>
#include <QHttp>
#include <QSettings>
#include <QApplication>

#include "updater.h"
#include "../dialogs/updaterdialog.cpp"

#include <QFileInfo>
#include <QStringList>
#include <QProcess>
#include <QDir>

Updater::Updater() :
  mState(Updater::Idle)
{
  mHost = "lightscreen.sourceforge.net";
  connect(&mHttp, SIGNAL(done(bool)), this, SLOT(httpDone(bool)));
}

void Updater::check()
{
  mHttp.setHost(mHost);
  mHttp.get("/version");
  mState = Updater::Check;
  //TODO: Show the options dialog when sucessful
}

void Updater::check(QObject* controller)
{
  if (!controller)
    return;

  connect(this  , SIGNAL(checkDone(Updater::Result)), controller, SLOT(updaterCheckDone(Updater::Result)));
  connect(this  , SIGNAL(downloadDone(bool)),         controller, SLOT(updaterDownloadDone(bool)));
  connect(this  , SIGNAL(downloading(QString)),       controller, SLOT(updaterDownloading(QString)));
  connect(&mHttp, SIGNAL(dataReadProgress(int, int)), controller, SLOT(updaterProgressBar(int, int)));

  connect(controller, SIGNAL(updaterCancel())     , this, SLOT(cancel()));
  connect(controller, SIGNAL(updaterDisable())    , this, SLOT(disable()));

  connect(controller, SIGNAL(updaterFinished(int)), this, SLOT(abort()));
  connect(controller, SIGNAL(updaterDownload())   , this, SLOT(download()));

  check();
}

void Updater::download()
{
  mState = Updater::VersionData;
  mHttp.setHost(mHost);
  mHttp.get("/versiondata");
}

void Updater::httpDone(bool error)
{
  qDebug() << "updaterHttp(error=" << error << ", state=" << mState << ")";

  switch (mState)
  {
    case Updater::Check:
      Result result;
      if (!error)
      {
        QByteArray data = mHttp.readAll();
        double version  = QString(data).toDouble();

        if  (version > qApp->applicationVersion().toDouble())
          result = Updater::NewVersion;
        else
          result = Updater::NoVersion;
      }
      else
      {
        result = Updater::Error;
      }

      emit checkDone(result);
    break;
    case Updater::VersionData:
    {
      QStringList versionData;
      versionData = QString(mHttp.readAll()).split(",", QString::SkipEmptyParts);

      if (versionData.length() > 1)
      {
        QString host = versionData[1];
        QString file = versionData[2];
        QString fileName = QFileInfo(versionData[2]).fileName();
        QDir dir = QApplication::applicationDirPath() + "/installers/";

        mState = Updater::Download;

        if (!dir.exists())
          dir.mkpath(dir.path());

        mDownload.setFileName(dir.path() + "/" + fileName);

        mDownload.open(QIODevice::WriteOnly);

        mHttp.setHost(host);
        mHttp.get(file, &mDownload);

        emit downloading(fileName);
      }
      else
      {
        mState = Updater::Idle;
        emit downloading(QString());
      }
    break;
    }
    case Updater::Download:
      mDownload.close();

      emit downloadDone(error);

      mState = Updater::Idle;

      if (!error)
      {
        if (QProcess::startDetached(mDownload.fileName()))
        {
          QApplication::exit(0);
        }
        else
        {
          emit downloadDone(true);
        }
      }
    break;
    default:
    break;
  }
}

void Updater::cancel()
{
  emit canceled(true);
  abort();
}

void Updater::disable()
{
  emit canceled(false);
  abort();
}

void Updater::abort()
{
  mHttp.abort();
  mDownload.remove();
  mState = Updater::Idle;
}

Updater* Updater::mInstance = 0;

Updater *Updater::instance()
{
  if (!mInstance)
    mInstance = new Updater();

  return mInstance;
}

