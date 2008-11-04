#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <QObject>
#include <QString>
#include <QDir>
#include <QPixmap>

class Screenshot : public QObject
{
  Q_OBJECT

public:
  enum Format
  {
    PNG = 0,
    JPEG,
    BMP,
    TIFF
  };

  enum Naming
  {
    Numeric = 0,
    Timestamp
  };

  enum Mode
  {
    WholeScreen = 0,
    ActiveWindow,
    SelectedArea
  };

  struct Options
  {
    int format;
    QString prefix;
    int naming;
    QDir directory;
    int mode;
    int quality;
    bool flipNaming;
    bool directX;
    bool currentMonitor;
    bool clipboard;
    bool file;
    bool preview;
  };

  struct Result
  {
    Options options;
    bool result;
    QString fileName;
  };

  Screenshot(Screenshot::Options options);

public slots:
  Screenshot::Result take();

private:
  QPixmap getActiveWindow();
  QString getFileName();
  char*   formatString();
  QPixmap getSelectedArea();
  QPixmap getWholeScreen();

private:
  Screenshot::Options options;

};

#endif // SCREENSHOT_H
