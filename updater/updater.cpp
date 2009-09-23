#include <QDate>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QSettings>
#include <QApplication>

#include "updater.h"

#include <QFileInfo>
#include <QStringList>
#include <QProcess>
#include <QDir>
#include <QDebug>

Updater::Updater() :
  mState(Updater::Idle)
{
  mNetwork = new QNetworkAccessManager(this);
  connect(mNetwork, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
}

void Updater::check()
{
  mNetwork->get(QNetworkRequest(QUrl("http://lightscreen.sourceforge.net/version")));
  mState = Updater::Check;
}

void Updater::check(QObject* controller)
{
  if (!controller)
    return;

  if (mController != controller)
  {
    mController = controller;

    connect(this  , SIGNAL(checkDone(Updater::Result)), controller, SLOT(updaterCheckDone(Updater::Result)));
    connect(this  , SIGNAL(downloadDone(bool)),         controller, SLOT(updaterDownloadDone(bool)));
    connect(this  , SIGNAL(downloading(QString)),       controller, SLOT(updaterDownloading(QString)));

    connect(controller, SIGNAL(updaterFinished(int)), this, SLOT(abort()));
    connect(controller, SIGNAL(updaterCancel())     , this, SLOT(abort()));
    connect(controller, SIGNAL(updaterDownload())   , this, SLOT(download()));
  }

  check();
}

void Updater::download()
{
  mState = Updater::VersionData;
  mNetwork->get(QNetworkRequest(QUrl("http://lightscreen.sourceforge.net/versiondata")));
}

void Updater::requestFinished(QNetworkReply* reply)
{
  qDebug() << "requestFinished(state=" << mState << "error=" << (reply->error() != QNetworkReply::NoError) << ", errorString=" << reply->errorString() << ")";

  if (reply->error() == QNetworkReply::TimeoutError)
  {
    qDebug() << "Connection timed out";
    //emit timeout(); //TODO
    return;
  }
  switch (mState)
  {
    case Updater::Idle:
      qDebug() << "This should not happen";
    break;
    case Updater::Check:
    {
      Result result;
      if (reply->error() == QNetworkReply::NoError)
      {
        QByteArray data = reply->readAll();
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
    }
    break;
    case Updater::VersionData:
    {
      QStringList versionData;
      versionData = QString(reply->readAll()).split(",", QString::SkipEmptyParts);

      if (versionData.length() > 1)
      {
        QString url = versionData[1];
        mNetwork->get(QNetworkRequest(url));
        mState = Updater::Redirect;
      }
      else
      {
        mState = Updater::Idle;
        emit downloading(QString());
      }
    }
    break;
    case Updater::Redirect:
    {
      QUrl redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

      if (redirect.isValid())
      {
        QString fileName = QFileInfo(redirect.toString()).fileName();
        QDir dir = QApplication::applicationDirPath() + "/installer/";

        if (!dir.exists())
          dir.mkpath(dir.path());

        mDownloadFile.setFileName(dir.path() + "/" + fileName);
        mDownloadFile.open(QIODevice::WriteOnly);

        mDownload = mNetwork->get(QNetworkRequest(redirect));
        connect(mDownload, SIGNAL(downloadProgress(qint64,qint64)), mController, SLOT(updaterProgressBar(qint64, qint64)));
        connect(mDownload, SIGNAL(finished()), SLOT(downloadFinished()));
        connect(mDownload, SIGNAL(readyRead()), SLOT(downloadReadyRead()));

        emit downloading(fileName);
      }
      else
      {
        mState = Updater::Idle;
        emit downloading(QString()); 
      }
    }
    break;
    case Updater::Download:
    {
      if (reply->error() == QNetworkReply::NoError)
      {
        emit downloadDone(false);
        qDebug() << "File to open:" << mDownloadFile.fileName();

        if (QProcess::startDetached(mDownloadFile.fileName()))
          QApplication::exit(0);
      }
      else
      {
        emit downloadDone(true);
      }

      mState = Updater::Idle;
    }
    break;
  }
}

void Updater::abort()
{
  mDownloadFile.remove();

  if (mDownload)
  {
    mDownload->abort();
    mDownload->deleteLater();
  }

  mState = Updater::Idle;
}

void Updater::downloadFinished()
{
    mDownloadFile.close();
    mDownload->deleteLater();
}

void Updater::downloadReadyRead()
{
  mDownloadFile.write(mDownload->readAll());
}

Updater* Updater::mInstance = 0;

Updater *Updater::instance()
{
  if (!mInstance)
    mInstance = new Updater();

  return mInstance;
}

