#ifndef WIN_TASKMGR_H
#define WIN_TASKMGR_H
    #ifdef _WIN32
        #include "proclist.h"
        #include <windows.h>
        #include <tlhelp32.h>
        #include <psapi.h>
        #include "dll_func.h"
        #include "pdh.h"

 enum PLATFORM{    WINNT1,	WIN2K_XP1, WIN9X1, UNKNOWN1};


        class WIN_TASKMGR: public ProcList
        {

        private:
            HANDLE hProcessSnapShot;
            PROCESSENTRY32 ProcessEntry;
            QMap<QString,QStringList> proc_info;
            FILETIME prev_idle,prev_kernel,prev_user;
            int flag;
            QMap<QString,QString> status_info_map;


        public:
            WIN_TASKMGR();
            virtual QMap<QString,QStringList>  get_proc_info();


            virtual void kill_process(QString PID);

            virtual void create_process(QString path);

            virtual void create_proc_dump(QString PID);

            virtual void kill_proc_tree(QString PID);

            virtual QMap<QString,QString> status_info();

            virtual void set_proc_priory(QString PID,int priory);

            ~WIN_TASKMGR();

        private:
            void refresh_win_snapshot();
            DWORD get_memory_size(HANDLE hProc);
            QString get_proc_path(HANDLE hProc);
            ULONGLONG ft2ull(FILETIME &ft);







        };

    #endif
#endif // WIN_TASKMGR_H
