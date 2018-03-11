#ifdef _WIN32
#include "win_taskmgr.h"
#include <QDebug>

#include <tchar.h>

#include <QThread>



///----------------------------------------------------------------
///        Constructor
/// ---------------------------------------------------------------
WIN_TASKMGR::WIN_TASKMGR()
{
    flag=0;
}

///----------------------------------------------------------------
///        Get processes info
/// ---------------------------------------------------------------
QMap<QString,QStringList>  WIN_TASKMGR::get_proc_info()
{
    // refresh snaphot
    refresh_win_snapshot();

    hProcessSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);

    // clear last info
    proc_info.clear();

    // vars for get process time
    FILETIME lpCreation, lpExit, lpKernel, lpUser;
    SYSTEMTIME stCreation, stExit, stKernel, stUser;

    // process params
    QStringList proc_names_list,proc_pid_list,proc_thread_count_list,
            proc_parents_id_list,proc_priory_list,proc_creation_time,proc_work_time,
            proc_memory,proc_path;

    do
    {
        if (!ProcessEntry.th32ProcessID) continue;

        // get process descriptor
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ProcessEntry.th32ProcessID);


        // get process times info
        if(GetProcessTimes(hProcess,&lpCreation, &lpExit, &lpKernel, &lpUser))
        {
              FileTimeToSystemTime(&lpCreation, &stCreation);
              FileTimeToSystemTime(&lpExit, &stExit);
              FileTimeToSystemTime(&lpUser, &stUser);
              FileTimeToSystemTime(&lpKernel, &stKernel);
        }


        // get proc memory info
        proc_memory.append(QString::number(get_memory_size(hProcess))+" кб");


        // get process priory
        switch (GetPriorityClass(hProcess))
        {
            case HIGH_PRIORITY_CLASS:
                proc_priory_list.append("Hight");
                break;
            case IDLE_PRIORITY_CLASS:
                proc_priory_list.append("Low");
                break;
            case NORMAL_PRIORITY_CLASS:
                proc_priory_list.append("Middle");
                break;
            case REALTIME_PRIORITY_CLASS:
                proc_priory_list.append("Real time");
                break;
            case 0:
                proc_priory_list.append("-");
                break;
            case 16384:
                proc_priory_list.append("Lower-middle");
                break;

            case 32768:
                proc_priory_list.append("Upper-middle");
                break;

        }


        // get and store process NAME
        QString proc_name=QString::fromWCharArray(ProcessEntry.szExeFile);
        proc_names_list.append(proc_name);


        // store process PID
        proc_pid_list.append(QString::number(ProcessEntry.th32ProcessID));

        //store process thread count
        proc_thread_count_list.append(QString::number(ProcessEntry.cntThreads));

        // store process parent process id
        proc_parents_id_list.append(QString::number(ProcessEntry.th32ParentProcessID));

        // store process creation time
        proc_creation_time.append(QString::number(stCreation.wMinute));

        // store process work time
        proc_work_time.append(QString::number(stUser.wMinute));


        // get proc path
        proc_path.append(get_proc_path(hProcess));

        //wchar_t* a="C:\\Windows\\System32\\calc.exe";
        //PrintVersionStringInfo("C:\\Windows\\System32\\calc.exe");

        CloseHandle(hProcess);

        // check the PROCESSENTRY32 for other members.

    }
    while( Process32Next( hProcessSnapShot, &ProcessEntry ));


    // fill QMap array
    proc_info.insert("Name",proc_names_list);
    proc_info.insert("PID",proc_pid_list);
    proc_info.insert("Thread count",proc_thread_count_list);
    proc_info.insert("Parent PID",proc_parents_id_list);
    proc_info.insert("Priory",proc_priory_list);
    proc_info.insert("Create time",proc_creation_time);
    proc_info.insert("Work time",proc_work_time);
    proc_info.insert("Memory",proc_memory);
    proc_info.insert("Path",proc_path);


    // Close the handle
    CloseHandle( hProcessSnapShot );

    return proc_info;
}

///----------------------------------------------------------------
///        Refresh Snapshot
/// ---------------------------------------------------------------
void WIN_TASKMGR::refresh_win_snapshot()
{
    // Get the process list snapshot.
    hProcessSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);

    ProcessEntry.dwSize = sizeof( ProcessEntry );


    // Get the first process info.
    BOOL Return = FALSE;
    Return = Process32First( hProcessSnapShot,&ProcessEntry );

    // Getting process info failed.
    if( !Return )
    {
        qDebug()<<"Getting process info failed";
    }
}

///----------------------------------------------------------------
///        Terminate process
/// ---------------------------------------------------------------
void WIN_TASKMGR::kill_process(QString PID)
{
    refresh_win_snapshot();

    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(pEntry);
    BOOL hRes = Process32First(hProcessSnapShot, &pEntry);
    while(hRes)
    {
        if (QString::number(pEntry.th32ProcessID)==PID)
        {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,(DWORD) pEntry.th32ProcessID);
            if (hProcess != NULL)
            {
                qDebug()<<"taskkill "<<pEntry.th32ProcessID;
                TerminateProcess(hProcess, 9);
                CloseHandle(hProcess);
                break;
            }
        }
        hRes = Process32Next(hProcessSnapShot, &pEntry);
    }
    CloseHandle(hProcessSnapShot);
    return;

//    system("taskkill /pid 6468");
}

///----------------------------------------------------------------
///        Create Dump
/// ---------------------------------------------------------------
void WIN_TASKMGR::create_proc_dump(QString PID)
{
    refresh_win_snapshot();

    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(pEntry);
    BOOL hRes = Process32First(hProcessSnapShot, &pEntry);

    while(hRes)
    {
        if (QString::number(pEntry.th32ProcessID)==PID)
        {
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pEntry.th32ProcessID);
            if (hProcess != NULL)
            {



                //структура с информацией о процессе
                MODULEINFO miInfo;

                //получаем базовый адрес процесса
                if (GetModuleInformation(hProcess, NULL, &miInfo, sizeof(miInfo)))
                {
                    qDebug()<<miInfo.EntryPoint;
                    CloseHandle(hProcess);
                    return;  // miInfo.EntryPoint;
                }
                else
                {
                    CloseHandle(hProcess);
                    return;  // NULL;
                }
            }
        }
        hRes = Process32Next(hProcessSnapShot, &pEntry);

    }
}


///----------------------------------------------------------------
///        Get memory size
/// ---------------------------------------------------------------
DWORD WIN_TASKMGR::get_memory_size(HANDLE hProc)
{
    //структура с информацией о процессе
    PROCESS_MEMORY_COUNTERS pmcInfo;

    //получаем информацию о процессе
    if (GetProcessMemoryInfo(hProc, &pmcInfo, sizeof(pmcInfo)))
        return (DWORD)pmcInfo.WorkingSetSize/1024;
    else return (DWORD)0;

}


///----------------------------------------------------------------
///        Create Process
/// ---------------------------------------------------------------
void WIN_TASKMGR::create_process(QString proc_name)
{
    STARTUPINFO cif;
    ZeroMemory(&cif,sizeof(STARTUPINFO));
    PROCESS_INFORMATION pi;

    wchar_t* pass;
    pass = (wchar_t*) malloc (sizeof(wchar_t)*proc_name.length()+1);
    proc_name.toWCharArray(pass);
    pass[proc_name.length()]=0; // Null terminate the string


    CreateProcess(pass,NULL,
        NULL,NULL,FALSE,NULL,NULL,NULL,&cif,&pi);
}

///----------------------------------------------------------------
///        Get proc path
/// ---------------------------------------------------------------
QString WIN_TASKMGR::get_proc_path(HANDLE hProcess)
{
    //get process path
    WCHAR filename[MAX_PATH] = {};
    GetModuleFileNameEx(hProcess, NULL, filename, MAX_PATH);
    return QString::fromWCharArray(filename);

}


///----------------------------------------------------------------
///        Destructor
/// ---------------------------------------------------------------
WIN_TASKMGR::~WIN_TASKMGR()
{

}


///----------------------------------------------------------------
///        Kill process tree
/// ---------------------------------------------------------------
void WIN_TASKMGR::kill_proc_tree(QString PID)
{
        DWORD pid=PID.toLong();
        PROCESSENTRY32 pe;

        memset(&pe, 0, sizeof(PROCESSENTRY32));
        pe.dwSize = sizeof(PROCESSENTRY32);

        HANDLE hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if (::Process32First(hSnap, &pe))
        {
            do // Recursion
            {
                if (pe.th32ParentProcessID == pid)
                    WIN_TASKMGR::kill_proc_tree(QString::number(pe.th32ProcessID));
            } while (::Process32Next(hSnap, &pe));
        }


        // kill the main process
        HANDLE hProc = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

        if (hProc)
        {
            ::TerminateProcess(hProc, 1);
            ::CloseHandle(hProc);
        }
}



///----------------------------------------------------------------
///        Status info
/// ---------------------------------------------------------------
QMap<QString,QString> WIN_TASKMGR::status_info()
{


    // get CPU load
    FILETIME idle;
    FILETIME kernel;
    FILETIME user;

    if(flag==0)
    {
        GetSystemTimes(&prev_idle, &prev_kernel, &prev_user);
        flag=1;
        return status_info_map;
    }


    if(flag==1)
    {
        status_info_map.clear();
        GetSystemTimes(&idle, &kernel, &user);
        ULONGLONG sys = (ft2ull(user) - ft2ull(prev_user)) +
            (ft2ull(kernel) - ft2ull(prev_kernel));

        status_info_map.insert("CPU",QString::number(int((sys - ft2ull(idle) + ft2ull(prev_idle)) * 100.0 / sys))+"%");
        prev_idle = idle;
        prev_kernel = kernel;
        prev_user = user;
        flag=0;
    }
    // -------------------------------------------------------

    return status_info_map;
}


ULONGLONG WIN_TASKMGR::ft2ull(FILETIME &ft) {
    ULARGE_INTEGER ul;
    ul.HighPart = ft.dwHighDateTime;
    ul.LowPart = ft.dwLowDateTime;
    return ul.QuadPart;
}


///----------------------------------------------------------------
///        Set proc priory
/// ---------------------------------------------------------------
void WIN_TASKMGR::set_proc_priory(QString PID,int Priory)
{
    int PRIORY;
    // get process priory
    qDebug()<<Priory;
    switch (Priory)
    {
        case 4:
            PRIORY=HIGH_PRIORITY_CLASS;
            break;
        case 0:
            PRIORY=IDLE_PRIORITY_CLASS;
            break;
        case 2:
            PRIORY=NORMAL_PRIORITY_CLASS;
            break;
        case 5:
            PRIORY=REALTIME_PRIORITY_CLASS;
            break;
//        case 0:
//            break;
        case 1:
            PRIORY=16384;
            break;

        case 3:
            PRIORY=32768;
            break;
    }


    refresh_win_snapshot();

    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(pEntry);
    BOOL hRes = Process32First(hProcessSnapShot, &pEntry);


    while(hRes)
    {
        if (QString::number(pEntry.th32ProcessID)==PID)
        {
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pEntry.th32ProcessID);
            if (hProcess != NULL)
            {

                SetPriorityClass(hProcess,PRIORY);
                CloseHandle(hProcess);
            }
        }
        hRes = Process32Next(hProcessSnapShot, &pEntry);

    }
}






/*void  PrintVersionStringInfo(const TCHAR *fileName)
{
    PLONG infoBuffer;  // буфер, куда будем читать информацию
    DWORD infoSize;    // и его размер

    struct LANGANDCODEPAGE { // структура для получения кодовых страниц по языкам трансляции ресурсов файла
        WORD wLanguage;
        WORD wCodePage;
    } *pLangCodePage;

    // имена параметров, значения которых будем запрашивать
    TCHAR *paramNames[] = {
            _T(L"FileDescription"),
            _T(L"CompanyName"),
            _T(L"FileVersion"),
            _T(L"InternalName"),
            _T(L"LegalCopyright"),
            _T(L"LegalTradeMarks"),
            _T(L"OriginalFilename"),
            _T(L"ProductName"),
            _T(L"ProductVersion"),
            _T(L"Comments"),
            _T(L"Author")
    };

    TCHAR paramNameBuf[256]; // здесь формируем имя параметра
    TCHAR *paramValue;       // здесь будет указатель на значение параметра, который нам вернет функция VerQueryValue
    UINT paramSz;            // здесь будет длина значения параметра, который нам вернет функция VerQueryValue

     wchar_t info[11];

    // получаем размер информации о версии файла
    infoSize = GetFileVersionInfoSize(fileName, NULL);
    if ( infoSize > 0 )
    {
        // выделяем память
        infoBuffer = (PLONG) malloc(infoSize);

        // получаем информацию
        if ( 0 != GetFileVersionInfo(fileName, NULL, infoSize, infoBuffer) )
        {

            UINT cpSz;
            // получаем список кодовых страниц
            if ( VerQueryValue(infoBuffer,                      // наш буфер, содержащий информацию
                               _T(L"\\VarFileInfo\\Translation"),// запрашиваем имеющиеся трансляции
                               (LPVOID*) &pLangCodePage,        // сюда функция вернет нам указатель на начало интересующих нас данных
                               &cpSz) )                         // а сюда - размер этих данных
            {
                // перебираем все кодовые страницы
                for (int cpIdx = 0; cpIdx < (int)(cpSz/sizeof(struct LANGANDCODEPAGE)); cpIdx++ )
                {
                    // перебираем имена параметров
                    for (int paramIdx = 0; paramIdx < sizeof(paramNames)/sizeof(char*); paramIdx++)
                    {
                        // формируем имя параметра ( \StringFileInfo\кодовая_страница\имя_параметра )
                        _stprintf(paramNameBuf, _T("L\\StringFileInfo\\%04x%04x\\%s"),
                                        pLangCodePage[cpIdx].wLanguage,  // ну, или можно сделать фильтр для
                                        pLangCodePage[cpIdx].wCodePage,  // какой-то отдельной кодовой страницы
                                        paramNames[paramIdx]);



                        VerQueryValue(infoBuffer, paramNameBuf, (LPVOID*)&paramValue, &paramSz);
                        //info[paramIdx]=
                        //return paramNames[paramIdx];
                    }
                }
            }
        }

        free(infoBuffer);
        return ;
    }

    return  ;
}*/



#endif




