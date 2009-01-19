#ifndef CUSTOMNAMING_H
#define CUSTOMNAMING_H

#include <QString>
#include <QStringList>

class CustomNaming
{
public:
    CustomNaming(QString string, QStringList fileList);

    bool isValid();
    static bool isValid(QString string);
    QString string();

private:
    QString mString;
    QString mResult;
    QStringList mFileList;

private:
    void process();
    int  number();


};

#endif // CUSTOMNAMING_H
