#ifdef linux
#include "linux_taskmgr.h"







LINUX_TASKMGR::LINUX_TASKMGR()
{
    previous_idle_time=0, previous_total_time=0;
}

/// ---------------------------------------------------------------
///        Get proc info
/// ---------------------------------------------------------------
QMap<QString,QStringList> LINUX_TASKMGR::get_proc_info()
{
    process_info.clear();

    QStringList proc_ids,proc_status,proc_path,proc_name,proc_memory,proc_ppid,
            proc_priory,proc_starttime;


    // open proc dir
    DIR* proc = opendir("/proc");
    struct dirent* ent=0;
    long pid;

    // if open error
    if(proc == NULL) {
        qDebug()<<"opendir(/proc) error";
        QCoreApplication::quit();
    }

    // read proc dir
    while(ent = readdir(proc))
    {
        // read proc ids
        if(!isdigit(*ent->d_name))
            continue;

        // process id
        pid = strtol(ent->d_name, NULL, 10);

        double vm, rss;


        proc_ids.append(QString::fromUtf8(ent->d_name));


        proc_path.append(get_proc_path(pid));


        // get running
        proc_status.append(get_running(ent->d_name));

        QMap<QString,QString> info=LINUX_TASKMGR::get_process_info(vm, rss,pid);
        proc_ppid.append(info["PPID"]);
        proc_starttime.append(info["Start time"]);
        proc_priory.append(info["Priory"]);
        proc_name.append(info["Name"]);


    }
    closedir(proc);



    process_info.insert("PID",proc_ids);
    process_info.insert("Status",proc_status);
    process_info.insert("Path",proc_path);
    process_info.insert("Name",proc_name);
    process_info.insert("Memory",proc_memory);
    process_info.insert("PPID",proc_ppid);
    process_info.insert("Start time",proc_starttime);
    process_info.insert("Priory",proc_priory);
//    qDebug()<<"func"<<process_info["Name"];
    return process_info;

}




/// ---------------------------------------------------------------
///        Get running processes
/// ---------------------------------------------------------------
QString LINUX_TASKMGR::get_running(char* pid)
{
    char tbuf[32];
    char *cp;
    int fd;
    char c;

    sprintf(tbuf, "/proc/%s/stat", pid);
    fd = open(tbuf, O_RDONLY, 0);
    if (fd == -1) return QString("no open");

    memset(tbuf, '\0', sizeof tbuf); // didn't feel like checking read()
    read(fd, tbuf, sizeof tbuf - 1);

    cp = strrchr(tbuf, ')');
    if(!cp) return QString("no read");

    c = cp[2];
    close(fd);

    if (c=='R') {
      return "running";
//      running_processes.append(QString::fromUtf8(pid));
    }

    if (c=='D') {
      return "blocked";
    }
    return QString("blocked");
}


/// ---------------------------------------------------------------
///        Get status info
/// ---------------------------------------------------------------
QMap<QString, QString> LINUX_TASKMGR::status_info()
{
    struct sysinfo info;

    // memory usage info
    if (sysinfo(&info) != 0)
        qDebug()<<"sysinfo: error reading system statistics";

//    qDebug()<<"Uptime"<<info.uptime/3600<<info.uptime%3600/60<< info.uptime%60;
//    qDebug()<<"Total RAM: %ld MB\n"<< info.totalram/1024/1024;
//    qDebug()<<"Free RAM: %ld MB\n"<< (info.totalram-info.freeram)/1024/1024;
//    qDebug()<<"Process count: %d\n"<<info.procs;
//    qDebug()<<"Page size: %ld bytes\n"<< sysconf(_SC_PAGESIZE);

    QString uptime=QString::number(info.uptime/3600)+"h "+QString::number(info.uptime%3600/60)+
            "m "+QString::number(info.uptime%60)+"s";
    QMap<QString,QString> sys_info;

    sys_info.insert("Uptime",uptime);
    sys_info.insert("Total RAM",QString::number(info.totalram/1024/1024));
    sys_info.insert("Free RAM",QString::number((info.totalram-info.freeram)/1024/1024));


    // cpu usage info

//    for (size_t idle_time, total_time; get_cpu_times(idle_time, total_time); sleep(1)) {
    size_t idle_time, total_time;
    get_cpu_times(idle_time, total_time);

        const float idle_time_delta = idle_time - previous_idle_time;
        const float total_time_delta = total_time - previous_total_time;
        const float utilization = 100.0 * (1.0 - idle_time_delta / total_time_delta);

        previous_idle_time = idle_time;
        previous_total_time = total_time;
      sys_info.insert("CPU",QString::number(utilization));
//    }

      return sys_info;
}


/// ---------------------------------------------------------------
///        Get process path
/// ---------------------------------------------------------------
QString LINUX_TASKMGR::get_proc_path(long pid)
{
    /*char* name = (char*)calloc(1024,sizeof(char));
    if(name)
    {
        sprintf(name, "/proc/%d/cmdline",pid);
        FILE* f = fopen(name,"r");
        if(f){
            size_t size;
            size = fread(name, sizeof(char), 1024, f);
            if(size>0){
                if('\n'==name[size-1])
                    name[size-1]='\0';
              fclose(f);
            }
//            fclose(f);
        }
    }

    QString proc_name=QString::fromUtf8(name);
    delete name;
    return proc_name;*/

//    char arg1[20];
//    char exepath[PATH_MAX + 1] = {0};

//    sprintf( arg1, "/proc/%d/exe", pid );
//    readlink( arg1, exepath, 1024 );
////    qDebug()<<QString::fromUtf8(exepath);
//    return QString::fromUtf8(exepath);

    char exePath[PATH_MAX];
    char arg1[20];
    sprintf( arg1, "/proc/%d/exe", pid );
    ssize_t len = ::readlink(arg1, exePath, sizeof(exePath));
    if (len == -1 || len == sizeof(exePath))
        len = 0;
    exePath[len] = '\0';
    return QString::fromUtf8(exePath);

}

/// ---------------------------------------------------------------
///        Kill process
/// ---------------------------------------------------------------
void LINUX_TASKMGR::kill_process(QString PID)
{
    errno = 0;
    pid_t pid=PID.toInt();
        int  killReturn = kill( pid, SIGKILL);  // Kill child process group
        if(killReturn == -1)
        {
            if( errno == ESRCH)      // pid does not exist
            {
               qDebug() << "Group does not exist!" << endl;
            }
            else if( errno == EPERM) // No permission to send signal
            {
               qDebug() << "No permission to send signal!" << endl;
            }
            else
               qDebug()<< "Signal sent. All Ok!" << endl;
        }
}


/// ---------------------------------------------------------------
///        Kill process group
/// ---------------------------------------------------------------
void LINUX_TASKMGR::kill_proc_tree(QString PID)
{
    errno = 0;
    pid_t pid=PID.toInt();
        int  killReturn = killpg( pid, SIGKILL);  // Kill child process group
        if(killReturn == -1)
        {
            if( errno == ESRCH)      // pid does not exist
            {
               qDebug() << "Group does not exist!" << endl;
            }
            else if( errno == EPERM) // No permission to send signal
            {
               qDebug() << "No permission to send signal!" << endl;
            }
            else
               qDebug()<< "Signal sent. All Ok!" << endl;
        }
}


/// ---------------------------------------------------------------
///        Create process
/// ---------------------------------------------------------------
void LINUX_TASKMGR::create_process(QString path)
{
    QUrl url;
    url.setPath(path);
    QDesktopServices::openUrl(url);
}



void LINUX_TASKMGR::create_proc_dump(QString PID)
{


}




/// ---------------------------------------------------------------
///        Functions for get CPU info
/// ---------------------------------------------------------------
std::vector<size_t> LINUX_TASKMGR::get_cpu_times() {
    std::ifstream proc_stat("/proc/stat");
    proc_stat.ignore(5, ' '); // Skip the 'cpu' prefix.
    std::vector<size_t> times;
    for (size_t time; proc_stat >> time; times.push_back(time));
    return times;
}

bool LINUX_TASKMGR::get_cpu_times(size_t &idle_time, size_t &total_time) {
    const std::vector<size_t> cpu_times = get_cpu_times();
    if (cpu_times.size() < 4)
        return false;
    idle_time = cpu_times[3];
    total_time = std::accumulate(cpu_times.begin(), cpu_times.end(), 0);
    return true;
}


/// ---------------------------------------------------------------
///        Process info
/// ---------------------------------------------------------------
QMap<QString,QString> LINUX_TASKMGR::get_process_info(double& vm_usage, double& resident_set,long proc_id)
{
   using std::ios_base;
   using std::ifstream;
   using std::string;

   vm_usage     = 0.0;
   resident_set = 0.0;

   // 'file' stat seems to give the most reliable results
   char arg1[20];
   sprintf( arg1, "/proc/%d/stat", proc_id );
   ifstream stat_stream(arg1,ios_base::in);

   // dummy vars for leading entries in stat that we don't care about
   //
   string pid, comm, state, ppid, pgrp, session, tty_nr;
   string tpgid, flags, minflt, cminflt, majflt, cmajflt;
   string utime, stime, cutime, cstime, priority, nice;
   string O, itrealvalue;

   // the two fields we want
   //
   unsigned long vsize;
   long rss;
   double starttime;

   stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
               >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
               >> utime >> stime >> cutime >> cstime >> priority >> nice
               >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

   stat_stream.close();
//   double CPU= ((atof(stime.c_str())+atof(utime.c_str()))/(atof(cutime.c_str())+atof(cstime.c_str())))*100; //(stime. + utime)/cutime*100;
//   qDebug()<<CPU;

   long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
   vm_usage     = (vsize/1024.0);
   resident_set = rss * page_size_kb;

   QMap<QString,QString> info;
   info.insert("PPID",QString::fromLocal8Bit(ppid.c_str()));
   info.insert("Priory",QString::fromLocal8Bit(priority.c_str()));
   info.insert("Start time",QString::number(((starttime/100)/60)));
   info.insert("Name",QString::fromLocal8Bit(comm.c_str()));

   return info;
}



#endif
