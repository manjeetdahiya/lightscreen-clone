#include "customnaming.h"

#include <QString>
#include <QStringList>
#include <QRegExp>
#include <QDateTime>
#include <QDebug>

CustomNaming::CustomNaming(QString string, QStringList fileList) : mString(string), mFileList(fileList)
{
  process();
}

bool CustomNaming::isValid()
{
  return isValid(mString);
}

bool CustomNaming::isValid(QString string)
{
  if (string.isEmpty() || string.isNull())
    return false;

  if (string.count("/number/") > 1)
    return false;

  return true;
}

QString CustomNaming::string()
{
  return mResult;
}

void CustomNaming::process()
{
  if (!isValid())
    return;

  mResult = mString;

  mResult.replace("/number/"    , QString::number(number()));
  mResult.replace("/timestamp/" , QString::number(QDateTime::currentDateTime().toTime_t()));
  mResult.replace("/date/"      , QDateTime::currentDateTime().toString("dd-MM-yyyy hh.mm.ss"));

  //TODO: Date options.
}

int CustomNaming::number()
{
  int naming_largest = 0;

  QString pattern = mString;

  pattern.replace("/number/"    , "(\\d+)");
  pattern.replace("/timestamp/" , "(\\d{10})");
  pattern.replace("/date/"      , "(\\d{2}-\\d{2}-\\d{4} \\d{2}.\\d{2}.\\d{2})");

  QRegExp rx (pattern);

  QStringList toRemove = pattern.split("(\\d+)");

  foreach(QString file, mFileList)
  {
    if (rx.indexIn(file) != -1)
    {
      //Remove the extension
      file.chop(file.size() - file.lastIndexOf("."));

      // Remove everything surrounding the number
      foreach (QString p, toRemove)
      {
        if (!p.isNull() || !p.isEmpty())
          file.remove(QRegExp(p));
      }

      if (file.toInt() > naming_largest)
        naming_largest = file.toInt();
    }
  }

  return naming_largest + 1;
}
