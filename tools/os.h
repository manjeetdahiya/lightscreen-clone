
#ifndef OS_H_
#define OS_H_

namespace os
{
  QPixmap getDxScreen();
  QPixmap grabWindow(WId winId);
  bool singleInstance(QString name);
  void translate(QString language);
  void vistaGlass(QWidget *target);
};

#endif /*OS_H_*/
