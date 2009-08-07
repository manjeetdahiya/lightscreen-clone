#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <QObject>
#include <QString>
#include <QDir>
#include <QPixmap>
#include <QVariantMap>

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
    Date = 1,
  };

  enum Mode
  {
    WholeScreen  = 0,
    ActiveWindow = 1,
    SelectedArea = 2
  };

  struct Options
  {
    QString fileName;
    bool result;

    int format;
    QString prefix;
    int naming;
    QDir directory;
    int mode;
    int quality;
    bool flipNaming;
    bool currentMonitor;
    bool clipboard;
    bool file;
    bool preview;
    bool magnify;
    bool cursor;
    bool saveAs;
  };

  Screenshot(Screenshot::Options options);

  Screenshot::Options options();
  QPixmap &pixmap();

public slots:
  void take();
  void confirm(bool result = true);
  void discard();
  void save();

signals:
  void askConfirmation();
  void finished();

private:
  void    activeWindow();
  QString newFileName();
  QString extension();
  void    selectedArea();
  void    wholeScreen();
  void    grabDesktop();

private:
  Screenshot::Options mOptions;
  QPixmap mPixmap;

};

#endif // SCREENSHOT_H
