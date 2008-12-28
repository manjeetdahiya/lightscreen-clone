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
    PNG  = 0,
    JPEG = 1,
    BMP  = 2,
    TIFF = 3
  };

  enum Naming
  {
    Numeric = 0,
    Timestamp = 1
  };

  enum Mode
  {
    WholeScreen  = 0,
    ActiveWindow = 1,
    SelectedArea = 2
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

  Screenshot(Screenshot::Options opt);

public slots:
  Screenshot::Result take();

private:
  void  activeWindow();
  QString  newFileName();
  char* formatString();
  void  selectedArea();
  void  wholeScreen();

  void setPixmap(QPixmap pixmap);
  QPixmap& pixmap();
  QPixmap grabDesktop();

private:
  Screenshot::Options options;
  QPixmap mPixmap;

};

#endif // SCREENSHOT_H
