#include "itemprocess.h"
#include <QThread>
#include <QDebug>
ItemProcess::ItemProcess(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    proc=nullptr;
    proc = new QProcess(this);

    connect(pb_start, &QPushButton::released, this, &ItemProcess::slotStart);
    connect(pb_restart, &QPushButton::released, this, &ItemProcess::slotRestart);
    connect(pb_stop, &QPushButton::released, this, &ItemProcess::slotStop);
    connect(pb_send, &QPushButton::released, this, &ItemProcess::slotSend);
    connect(proc, &QProcess::stateChanged, this, &ItemProcess::slotChangedState);

    setMaximumHeight(150);
}

ItemProcess::~ItemProcess()
{
    slotStop();
}

void ItemProcess::slotChangedState(QProcess::ProcessState state)
{
    switch(state)
    {
    case QProcess::NotRunning:
        l_status->setText(QStringLiteral(R"(<font color='red'>NotRunning</font>)"));
        showError();
        killAllProc();
        break;
    case QProcess::Starting:
        l_status->setText(QStringLiteral(R"(<font color='orange'>Starting</font>)"));
        break;
    case QProcess::Running:
        l_status->setText(QStringLiteral(R"(<font color='green'>Running</font>)"));
        l_exit_status->clear();
        pids_proc.append(proc->pid());
        findAllChildProc();
        qDebug()<<program<<"pids="<<pids_proc;
        break;
    default:
        l_status->setText("???");
    }
}

void ItemProcess::showError()
{


    switch(proc->error())
    {
    case QProcess::FailedToStart:
        l_exit_status->setText(QStringLiteral(R"(<font color='red'>FailedToStart</font>)"));
        break;
    case QProcess::Crashed:
        l_exit_status->setText(QStringLiteral(R"(<font color='red'>Crashed</font>)"));
        break;
    case QProcess::Timedout:
        l_exit_status->setText(QStringLiteral(R"(<font color='red'>Timedout</font>)"));
        break;
    case QProcess::WriteError:
        l_exit_status->setText(QStringLiteral(R"(<font color='red'>WriteError</font>)"));
        break;
    case QProcess::ReadError:
        l_exit_status->setText(QStringLiteral(R"(<font color='red'>ReadError</font>)"));
        break;
    case QProcess::UnknownError:
        l_exit_status->setText(QStringLiteral(R"(<font color='orange'>UnknownError</font>)"));
        break;
    default:
        l_exit_status->setText("???");
    }
}
// функция костыль для поиска всего дерева процессов, так как поражденные процессы при закрытии родительских остаются работать
void ItemProcess::findAllChildProc()
{
    QThread::usleep(5000);
    QProcess *p = new QProcess(this);
    for(int i=0; i<pids_proc.count(); i++)
    {
        p->start(QString("ps -o pid=  --ppid %1").arg(pids_proc.at(i)));
        if(p->waitForFinished())
        {
            QProcess::ExitStatus Status = p->exitStatus();

            if (Status == QProcess::ExitStatus::NormalExit)
            {
                QStringList l = QString(p->readAllStandardOutput()).split('\n');
                //qDebug()<<l;
                for(int i=0; i<l.count(); i++)
                {
                    if(l.at(i).length())
                    {
                        pids_proc.append(l.at(i).toInt());
                    }
                }
            }
        }
    }
}
// функция костыль для закрытия всего дерева процессов, так как поражденные процессы при закрытии родительских остаются работать
void ItemProcess::killAllProc()
{
    QString s;
    QThread::usleep(5000);
    QProcess *p = new QProcess(this);


    // попытка убить все процессы без проверки
    for(int i=0; i<pids_proc.count(); i++)
    {
        s.append(QString(" %1").arg(pids_proc.at(i)));
    }
    //qDebug()<<s;

    p->start(QString("kill%1").arg(s));
    if(p->waitForFinished())
    {
        qDebug()<<"killAllProc"<<s<<"ok";
    }
    delete p;


    // проверка на отсутствие процессов и попытка их ибить если они еще есть
    int wd=0;

    while(1)
    {
        s.clear();
        for(int i=0; i<pids_proc.count(); i++)
        {
            p = new QProcess(this);
            p->start(QString("ps -o pid=  --pid %1").arg(s));
            if(p->waitForFinished())
            {
                if(proc->exitCode()==0)
                {
                    s.append(QString(" %1").arg(pids_proc.at(i)));
                    qDebug()<<"proc"<<pids_proc.at(i)<<"alive";
                }
                else
                {
                    qDebug()<<"proc"<<pids_proc.at(i)<<"dead";
                }
            }
            delete p;
        }
        if(!s.length())
        {
            break;
        }
        p = new QProcess(this);
        p->start(QString("kill%1").arg(s));
        if(p->waitForFinished())
        {
            if(proc->exitCode()==0)
            {
                qDebug()<<"killAllProc"<<s<<"ok";
            }
            else
            {
                qDebug()<<"killAllProc"<<s<<"error";
            }
        }
        delete p;
        if(wd>10)
        {
            break;
        }
        wd++;
    }


    pids_proc.clear();

}

void ItemProcess::setOptions(QString name, QString prog, QStringList arg)
{
    program=prog;
    arguments=arg;
    l_prog->setText(program);
    l_name->setText(name);
}
void ItemProcess::slotStart()
{
    if(!proc)
    {
        return;
    }
    proc->start(program, arguments);
}
void ItemProcess::slotRestart()
{
    if(!proc)
    {
        return;
    }
    slotStop();
    QThread::sleep(1);
    slotStart();
}
void ItemProcess::slotStop()
{
    if(!proc)
    {
        return;
    }
    proc->close();
}
void ItemProcess::slotSend()
{
    if(le_send->text().length())
    {
        proc->write(le_send->text().toUtf8());
    }
}
