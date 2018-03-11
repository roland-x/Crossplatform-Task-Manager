#ifndef LINUX_TASKMGR_H
#define LINUX_TASKMGR_H

#ifdef linux
#include <QDebug>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <QCoreApplication>


#include "proclist.h"


#include <unistd.h>
#include <fcntl.h>
#include "version.h"
#include "sysinfo.h"

#include <fstream>
#include <numeric>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <signal.h>

#include <sys/sysinfo.h>



class LINUX_TASKMGR:public ProcList
{
public:

    LINUX_TASKMGR();

    virtual QMap<QString,QStringList>  get_proc_info();

    virtual void kill_process(QString PID);

    virtual void create_process(QString proc_name);


    virtual void create_proc_dump(QString PID);

    virtual void kill_proc_tree(QString PID);

    virtual QMap<QString,QString> status_info();



private:
    QStringList running_processes;
    QMap<QString,QStringList> process_info;
    std::vector<size_t> get_cpu_times();
    size_t previous_idle_time, previous_total_time;

    bool get_cpu_times(size_t &idle_time, size_t &total_time);
    QString get_running(char* pid);
    QString get_proc_path(long pid);
    QMap<QString,QString> get_process_info(double& vm_usage, double& resident_set,long pid);

};

#endif
#endif // LINUX_TASKMGR_H
