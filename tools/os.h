
#ifndef OS_H_
#define OS_H_

#include <QObject>
#include <QPixmap>

namespace os
{ //TODO: Comment functions
  QPixmap getDxScreen();
  QPixmap grabWindow(WId winId);
  void translate(QString language);
  void vistaGlass(QWidget *target);
}


#endif /*OS_H_*/
