#ifndef PREVIEWDIALOG_H
#define PREVIEWDIALOG_H

#include <QDialog>
#include <QPixmap>
#include <QTimer>

class PreviewDialog : public QDialog
{
  Q_OBJECT

public:
    PreviewDialog(QPixmap &screenshot);

private slots:
    void kill();
    void effect(int frame);

protected:
    bool event(QEvent *event);

private:
  QTimer deathTimer;


};

#endif // PREVIEWDIALOG_H
