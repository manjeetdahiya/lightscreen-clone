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

  mResult.replace("/timestamp/" , QString::number(QDateTime::currentDateTime().toTime_t()));
  mResult.replace("/date/"      , QDateTime::currentDateTime().toString("dd-MM-yyyy hh.mm.ss"));

  // Convenience function, parses /date/s with options.
  date();

  // The date() function MUST be called before the number function since it sets mNumberString which is used by the number() function to skip /date/ parameters.
  mResult.replace("/number/"    , QString::number(number()));

  qDebug() << mResult;
}

int CustomNaming::number()
{
  int naming_largest = 0;

  QString pattern = mNumberString; // NumberString is set by the date() function

  pattern.replace("/number/"    , "(\\d+)");
  pattern.replace("/timestamp/" , "(\\d{10})");
  pattern.replace("/date/"      , "(\\d{2}-\\d{2}-\\d{4} \\d{2}.\\d{2}.\\d{2})");

  QRegExp rx  (pattern);

  foreach(QString file, mFileList)
  {
    // Remove the extension
    file.chop(file.size() - file.lastIndexOf("."));

    if (file.contains(rx))
    {
      file = rx.capturedTexts().last();

      if (file.toInt() > naming_largest)
        naming_largest = file.toInt();
    }
  }

  return naming_largest + 1;
}

void CustomNaming::date()
{
  QRegExp expression ("/date\\(((\\w|\\-)+)\\)/");
  QString string = mResult;
  QString command;

  mNumberString = mResult;

  int pos = 0;    // where we are in the string
  int count = 0;  // how many expressions we've counted

  while (pos >= 0) {
    pos = expression.indexIn(string, pos);

    if (!expression.capturedTexts()[0].isEmpty())
      command = expression.capturedTexts()[1]; // Insert the command into the list.

    if (!command.isEmpty())
    {
      string.replace(pos, expression.capturedTexts()[0].length(), QDateTime::currentDateTime().toString(command));
      mNumberString.replace(pos, expression.capturedTexts()[0].length(), QString("(\\w+|-+){%1}").arg(command.length()));
    }

    if (pos >= 0)
    {
      ++pos;      // move along in str
      ++count;    // count expression
    }

    command = ""; // Reset
  }

  mResult = string;
}
