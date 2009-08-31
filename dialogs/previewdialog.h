#ifndef PREVIEWDIALOG_H
#define PREVIEWDIALOG_H

#include <QDialog>

class Screenshot;
class QStackedLayout;
class QPushButton;
class PreviewDialog : public QDialog
{
    Q_OBJECT

public:
  PreviewDialog(QWidget *parent);
  ~PreviewDialog();

  void add(Screenshot* screenshot);

  static PreviewDialog *instance();

public slots:
  void setWidth(int w)  { resize(w, height()); }
  void setHeight(int h) { resize(width(), h);  }

private slots:
  void closePreview();
  void relocate();
  void previous();
  void next();
  void indexChanged(int i);

protected:
  void closeEvent(QCloseEvent* event);

private:
  static PreviewDialog* mInstance;
  QStackedLayout* mStack;
  QPushButton*    mNextButton;
  QPushButton*    mPrevButton;
};

#endif // PREVIEWDIALOG_H
