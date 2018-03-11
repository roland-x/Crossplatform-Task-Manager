#ifndef PROCLIST_H
#define PROCLIST_H
#include <QMap>
#include <QProcess>

class ProcList
{
public:
    ProcList();

    virtual QMap<QString,QStringList> get_proc_info()=0;

    virtual void kill_process(QString PID)=0;

    virtual void create_process(QString proc_name)=0;

    virtual void create_proc_dump(QString PID)=0;

    virtual void kill_proc_tree(QString PID)=0;

    virtual QMap<QString,QString> status_info()=0;

    virtual void set_proc_priory(QString PID,int Priory)=0;

    virtual ~ProcList();

};

#endif // PROCLIST_H
