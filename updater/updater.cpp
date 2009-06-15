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

void Updater::check(bool silent)
{
  qDebug() << mUpdaterDialog;

  if (!mUpdaterDialog)
    mUpdaterDialog = new UpdaterDialog(silent);

  connect(this  , SIGNAL(checkDone(Updater::Result)), mUpdaterDialog, SLOT(checkDone(Updater::Result)));
  connect(this  , SIGNAL(downloadDone(bool)),         mUpdaterDialog, SLOT(downloadDone(bool)));
  connect(this  , SIGNAL(downloading(QString)),       mUpdaterDialog, SLOT(downloading(QString)));
  connect(&mHttp, SIGNAL(dataReadProgress(int, int)), mUpdaterDialog, SLOT(progressBar(int, int)));

  connect(mUpdaterDialog, SIGNAL(cancel())     , this, SLOT(cancel()));
  connect(mUpdaterDialog, SIGNAL(disable())    , this, SLOT(disable()));

  connect(mUpdaterDialog, SIGNAL(finished(int)), this, SLOT(abort()));
  connect(mUpdaterDialog, SIGNAL(download())   , this, SLOT(download()));

  mHttp.setHost(mHost);
  mHttp.get("/version");
  mState = Updater::Check;

  if (!silent)
   mUpdaterDialog->show(); //TODO: Show it later (on sucess)
}

void Updater::download()
{
  mState = Updater::VersionData;
  mHttp.setHost(mHost);
  mHttp.get("/versiondata");
}

void Updater::httpDone(bool error)
{
  qDebug() << "httoDone(" << error << ") " << mState;
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

