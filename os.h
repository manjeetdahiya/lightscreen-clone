
#ifndef OS_H_
#define OS_H_

namespace os
{
  void vistaGlass(QWidget *target);
  bool singleInstance(QString name);
  void translate(QString language);
  QPixmap grabWindow(WId winId);
  QPixmap getDxScreen();
};

#endif /*OS_H_*/
