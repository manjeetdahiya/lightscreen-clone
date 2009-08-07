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
  void add(Screenshot* screenshot);

  static PreviewDialog *instance();

private slots:
  void closePreview();
  void relocate();
  void previous();
  void next();

private:
  static PreviewDialog* mInstance;
  QStackedLayout* mStack;
  QPushButton*    mNextButton;
  QPushButton*    mPrevButton;
};

#endif // PREVIEWDIALOG_H
