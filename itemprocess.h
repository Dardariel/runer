#ifndef ITEMPROCESS_H
#define ITEMPROCESS_H

#include <QWidget>
#include <QProcess>

#include "ui_itemprocess.h"


class ItemProcess : public QWidget , public Ui::ItemProcess
{
    Q_OBJECT

public:
    explicit ItemProcess(QWidget *parent = nullptr);
    ~ItemProcess();

    void setOptions(QString name, QString prog, QStringList arg);
    void showError();

    void findAllChildProc();
    void killAllProc();

public slots:
    void slotStart();
    void slotRestart();
    void slotStop();
    void slotSend();

    void slotChangedState(QProcess::ProcessState state);


private:
    QProcess *proc;
    QString program;
    QStringList arguments;
    QList<Q_PID> pids_proc;
};

#endif // ITEMPROCESS_H
