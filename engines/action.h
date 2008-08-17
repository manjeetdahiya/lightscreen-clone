#ifndef ACTION_H_
#define ACTION_H_

#include <QObject>

class Action: public QObject
{
  Q_OBJECT
  
public:
	Action(int code);
	
	int getCode() const;
	int getMode() const;
	
	void setCode(int code);
	void setMode(int mode);


public slots:
  void exec();
	
signals:
  void triggered(int mode);
  
private:
  int mCode;
  int mMode;
  
};

#endif /*ACTION_H_*/
