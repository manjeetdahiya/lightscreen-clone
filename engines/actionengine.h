#ifndef ACTIONENGINE_H_
#define ACTIONENGINE_H_

#include <QList>
#include "action.h"

class ActionEngine: public QObject
{
  Q_OBJECT

public:
	ActionEngine();

	void add(Action* action);
	void clear(QWidget *actionParent);
	void triggerAction(int code);
	
  static ActionEngine *instance()
  {
    if (mInstance == 0)
      mInstance = new ActionEngine();
    
    return (mInstance);
  };
  
  
public slots:
	void slotSysKeyPress(Qt::Key key, QFlags<Qt::KeyboardModifier> keyboardModifiers);
	
signals:
  void sysKeyPress(Qt::Key key, QFlags<Qt::KeyboardModifier> keyboardModifiers);

private:
  QList<Action*> mActionList;
  static ActionEngine *mInstance;
  
};


#endif /*ACTIONENGINE_H_*/
