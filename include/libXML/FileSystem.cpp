#include "FileSystem.h"

#include <IrrCompileConfig.h>
#include <irrTypes.h>
#include <irrString.h>
#include <irrArray.h>
#include <path.h>
#include <coreutil.h>

#if IRRLICHT_VERSION_MINOR < 8
	#if defined(_IRR_WCHAR_FILESYSTEM)
		typedef wchar_t fschar_t;
		#define _IRR_TEXT(X) L##X
	#else
		typedef char fschar_t;
		#define _IRR_TEXT(X) X
	#endif
#endif

namespace irr{
namespace io{

#undef FILESYSTEM_NATIVE
#define FILESYSTEM_NATIVE 0
u32 FSType = FILESYSTEM_NATIVE; // later FSType has to be replaced with FileSystemType
core::array<SDriveInfo> DriveList;
path WorkingDirectory;

//! helper, convert last error to string
core::stringc getErrorText()
{
	core::stringc errorText;

	#if defined(_IRR_WINDOWS_API_)
    c8* lpMsgBuf = 0; // will be allocated by WinAPI
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        lpMsgBuf,
        0,
        NULL);

    errorText = lpMsgBuf;
    LocalFree(lpMsgBuf);
    #elif defined(_IRR_POSIX_API_)
    errorText = strerror(errno);
	#else
	errorText = "unknown";
    #endif
    return errorText;
}



//! Update list of available physical drives
void getDriveList()
{
	DriveList.clear();

#if defined( _IRR_WINDOWS_API_ )
	const s32 SIZE = 255;
	#if defined (_IRR_WCHAR_FILESYSTEM)

	wchar_t* buffer = new wchar_t[SIZE]; // LPTSTR
	u32 chars = GetLogicalDriveStringsW(SIZE, buffer); // Win32 API ANSI
	core::stringw result(L"");
	for ( s32 i=0; i < (s32)chars; ++i)
	{
		result += buffer[i];
	}
	delete buffer;
	core::array<core::stringw> container;
	result.split(container, L"\0", 1, true, false);

	#else

	c8* buffer = new c8[SIZE]; // LPTSTR
	u32 chars = GetLogicalDriveStringsA(SIZE, buffer); // Win32 API ANSI
	core::stringc result("");
	for ( s32 i=0; i<(s32)chars; ++i)
	{
		result += buffer[i];
	}
	delete buffer;
	core::array<core::stringc> container;
	result.split(container, "\0", 1, true, false);

	#endif

	for (u32 i=0; i<container.size(); ++i)
	{
		SDriveInfo drive;
		drive.Name = container[i];
		drive.Type = ELDT_NOT_EXIST;
	#if defined (_IRR_WCHAR_FILESYSTEM)
		drive.Type = (E_LOGICAL_DRIVE_TYPE)GetDriveTypeW(drive.Name.c_str());
	#else
		drive.Type = (E_LOGICAL_DRIVE_TYPE)GetDriveTypeA(drive.Name.c_str());
	#endif
		drive.Name.make_lower();
		drive.AvailSpace = 0;
		drive.TotalSpace = 0;
		drive.OptimalBlockSize = 1024;
		DWORD dwSectorsPerCluster = 0;
		DWORD dwBytesPerSector = 0;
		DWORD dwNumberOfFreeClusters = 0;
		DWORD dwTotalNumberOfClusters = 0;
		//! jumps over a: and b:
		//! to prevent popup messagebox for non inserted floppy disks
		if (!((drive.Name == path("a:\\")) || (drive.Name == path("b:\\"))))
		{
	#if defined (_IRR_WCHAR_FILESYSTEM)
			s32 result = GetDiskFreeSpaceW( drive.Name.c_str(), &dwSectorsPerCluster, &dwBytesPerSector, &dwNumberOfFreeClusters, &dwTotalNumberOfClusters);
	#else
			s32 result = GetDiskFreeSpaceA( drive.Name.c_str(), &dwSectorsPerCluster, &dwBytesPerSector, &dwNumberOfFreeClusters, &dwTotalNumberOfClusters);
	#endif
			if (result)
			{

			}
		}
		drive.OptimalBlockSize = dwSectorsPerCluster*dwBytesPerSector;
		drive.AvailSpace = (u64)drive.OptimalBlockSize*(u64)dwNumberOfFreeClusters;
		drive.TotalSpace = (u64)drive.OptimalBlockSize*(u64)dwTotalNumberOfClusters;

	#if defined (_IRR_WCHAR_FILESYSTEM)
		drive.Name.replace(L'\\',L'/');
	#else
		drive.Name.replace('\\','/');
	#endif
		DriveList.push_back(drive);
	}
//! Linux
#elif defined (_IRR_POSIX_API_)
//
//    s32 status;

    for (u32 i=0; i<3; i++)
    {
        SDriveInfo drive;
        drive.Name = _IRR_TEXT("");
        drive.Type = ELDT_UNKNOWN;
        drive.OptimalBlockSize = 1024;
        drive.AvailSpace = 0;
        drive.TotalSpace = 0;

        if (i==0)
        {
            drive.Name = _IRR_TEXT("/");
            DriveList.push_back(drive);
        }
        else if (i==1)
        {
            FILE* f=popen("dmesg | egrep -e \"^.{0,}(s|h){1}d{1}[a-z]{1}[0-9]{1,}.{0,}$\" | sort", "r");
            if (!f)
            {
                perror("popen");
                puts("ERROR::getDriveList(), could not open pipe from dmesg.\n");
                return;
            }
            c8 chars[1024];
            memset(chars,0,1024);
            core::stringc name = "/dev/";
            core::stringc tmp("");
            while(fgets(chars, 1024, f))
            {
                tmp=name;
                tmp+=chars;
                tmp.replace('\n','\0');
                tmp.replace('\r','\0');
                memset(chars, 0, 1024);
                drive.Name = tmp;
                DriveList.push_back(drive);
            }
            pclose(f);
        }
        else if (i==2)
        {
            FILE* f=popen("ls -1 /media | sort", "r");
            if (!f)
            {
                puts("ERROR::getDriveList(), could not open pipe from ls.\n");
                return;
            }
            c8 chars[1024];
            memset(chars,0,1024);
            core::stringc name = "/media/";
            core::stringc tmp("");
            while(fgets(chars, 1024, f))
            {
                tmp=name;
                tmp+=chars;
                tmp.replace('\n','\0');
                tmp.replace('\r','\0');
                memset(chars, 0, 1024);

                drive.Name = tmp;
                DriveList.push_back(drive);
            }

            pclose(f);
        }
    }

    //! now we have a list of potential FileSystems
	for (u32 i=0; i<DriveList.size(); ++i)
	{
	    SDriveInfo& drive = DriveList[i];

        #ifdef __USE_LARGEFILE64
        struct statfs64 info;
        if (statfs64(drive.Name.c_str(), &info ) != -1)
        #else
        struct statfs info;
        if (statfs(drive.Name.c_str(), &info ) != -1)
        #endif
        {
            drive.OptimalBlockSize = info.f_bsize;
            drive.AvailSpace = (((u64)info.f_bsize) * (u64)info.f_bavail); // in Bytes
            drive.TotalSpace = (((u64)info.f_bsize) * (u64)info.f_blocks); // in Bytes

//            std::cout << " Name = "<<drive.Name.c_str()<<" with Type = ";
//            switch ((u32)info.f_type)
//            {
//                case AFFS_SUPER_MAGIC: std::cout<<"AFFS_SUPER_MAGIC"; break;
//                case EFS_SUPER_MAGIC: std::cout<<"EFS_SUPER_MAGIC"; break;
//                case EXT_SUPER_MAGIC: std::cout<<"EXT_SUPER_MAGIC"; break;
//                case EXT2_OLD_SUPER_MAGIC: std::cout<<"EXT2_OLD_SUPER_MAGIC"; break;
//                case EXT2_SUPER_MAGIC: std::cout<<"EXT2_SUPER_MAGIC"; break;
//                case HPFS_SUPER_MAGIC: std::cout<<"HPFS_SUPER_MAGIC"; break;
//                case ISOFS_SUPER_MAGIC: std::cout<<"ISOFS_SUPER_MAGIC"; break;
//                case MINIX_SUPER_MAGIC: std::cout<<"MINIX_SUPER_MAGIC"; break;
//                case MINIX_SUPER_MAGIC2: std::cout<<"MINIX_SUPER_MAGIC2"; break;
//                case MINIX2_SUPER_MAGIC: std::cout<<"MINIX2_SUPER_MAGIC"; break;
//                case MINIX2_SUPER_MAGIC2: std::cout<<"MINIX2_SUPER_MAGIC2"; break;
//                case MSDOS_SUPER_MAGIC: std::cout<<"MSDOS_SUPER_MAGIC"; break;
//                case NCP_SUPER_MAGIC: std::cout<<"NCP_SUPER_MAGIC"; break;
//                case NFS_SUPER_MAGIC: std::cout<<"NFS_SUPER_MAGIC"; break;
//                case PROC_SUPER_MAGIC: std::cout<<"PROC_SUPER_MAGIC"; break;
//                case SMB_SUPER_MAGIC: std::cout<<"SMB_SUPER_MAGIC"; break;
//                case XENIX_SUPER_MAGIC: std::cout<<"XENIX_SUPER_MAGIC"; break;
//                case SYSV4_SUPER_MAGIC: std::cout<<"SYSV4_SUPER_MAGIC"; break;
//                case SYSV2_SUPER_MAGIC: std::cout<<"SYSV2_SUPER_MAGIC"; break;
//                case COH_SUPER_MAGIC: std::cout<<"COH_SUPER_MAGIC"; break;
//                case UFS_MAGIC: std::cout<<"UFS_MAGIC"; break;
//                case XFS_SUPER_MAGIC: std::cout<<"XFS_SUPER_MAGIC"; break;
//                case _XIAFS_SUPER_MAGIC: std::cout<<"_XIAFS_SUPER_MAGIC"; break;
//                default: std::cout<<"unknown"; break;
//            }
//            std::cout << std::endl;
////                    std::cout << "\t\tAvailSpace = "<<drive.AvailSpace<<std::endl;
////                    std::cout << "\t\tTotalSpace = "<<drive.TotalSpace<<std::endl;
////                    std::cout << "\t\tOptimalBlockSize = "<<drive.OptimalBlockSize<<std::endl<<std::endl;
//        }
//        else
//        {
//            std::cout << "ERROR :: statfs() returned with ERROR!"<<std::endl;
        }
    }
#else
	#warning WARNING getDriveList() not implemented.
	SDriveInfo drive;
	#if defined (_IRR_WCHAR_FILESYSTEM)
	drive.Name = L"";
	#else
	drive.Name = "";
	#endif
	drive.Type = ELDT_NOT_EXIST;
	drive.AvailSpace = 0;
	drive.TotalSpace = 1;
	drive.OptimalBlockSize = 1024;
	DriveList.push_back( drive );
#endif
}

//! Return count of available physical drives
u32 getDriveCount()
{
	return DriveList.size();
}

//! Return the unique name of the physical drive
path getDriveName( u32 i )
{
	if (i<DriveList.size())
		return DriveList[i].Name;
	else
		return "no valid index";
}

//! Return the type of the physical drive
E_LOGICAL_DRIVE_TYPE getDriveType( u32 i )
{
	if (i<DriveList.size())
		return DriveList[i].Type;
	else
		return ELDT_NOT_EXIST;
}

//! Return the amount of free space on physical drive in Bytes
u64 getDriveAvailSpace( u32 i )
{
	if (i<DriveList.size())
		return DriveList[i].AvailSpace;
	else
		return 0;
}

//! Return the amount of total space on physical drive in Bytes
u64 getDriveTotalSpace( u32 i )
{
	if (i<DriveList.size())
		return DriveList[i].TotalSpace;
	else
		return 0;
}

//! Return the optimal BlockSize for copy-ops on physical drive in Bytes
u32 getDriveOptimalBlockSize( u32 i )
{
	if (i<DriveList.size())
		return DriveList[i].OptimalBlockSize;
	else
		return 1024;
}



//! Return the full filename of the application executeable.
path getExeFile()
{
//    Mac OS X: _NSGetExecutablePath() (man 3 dyld)
//    Linux:	readlink /proc/self/exe
//    Solaris: 	getexecname()
//    FreeBSD: 	sysctl CTL_KERN KERN_PROC KERN_PROC_PATHNAME -1
//    BSD with 	procfs: readlink /proc/curproc/file
//    Windows: 	GetModuleFileName() with hModule = NULL
    path tmp;

//! WinAPI
#if defined(_IRR_WINDOWS_API_)
	#if defined (_IRR_WCHAR_FILESYSTEM)
    wchar_t buf[ MAX_PATH ]; buf[0] = 0;
    if (!GetModuleFileNameW( NULL, (LPWSTR)&buf, MAX_PATH))
	#else
    c8 buf[ MAX_PATH ]; buf[0] = 0;
    if (!GetModuleFileNameA( NULL, (LPSTR)&buf, MAX_PATH))
	#endif
	{
		printf("%s\n",getErrorText().c_str());
		return tmp;
	}
    tmp = buf;
	#if defined (_IRR_WCHAR_FILESYSTEM)
    tmp.replace(L'\\', L'/');
    #else
    tmp.replace('\\', '/');
	#endif
//! POSIX
#elif defined (_IRR_POSIX_API_)
	c8 buf[1024];
	memset(buf, 0, sizeof(buf));
	if (readlink("/proc/self/exe", buf, sizeof(buf)-1))
	{
		perror("readlink");
		return tmp;
	}
	tmp = buf;
#else
	#warning getExeName() not implemented!
#endif
    return tmp;
}

//! Return the full path of the application executeable.
path getExeDirectory()
{
	return getFileDirectory( getExeFile() );
}

//! determines if a file exists and would be able to be opened.
bool existFile(const path& fileName)
{
#if defined(_IRR_WINDOWS_API_)
	path tmp = fileName;
    #if defined(_IRR_WCHAR_FILESYSTEM)
	tmp.replace(L'/', L'\\');
	HANDLE hFile = CreateFileW(tmp.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    #else
	tmp.replace('/', '\\');
	HANDLE hFile = CreateFileA(tmp.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    #endif
	if (hFile == INVALID_HANDLE_VALUE)
    {
         _IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX
		return false;
    }
	else
	{
		CloseHandle(hFile);
        _IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX
		return true;
	}
#else
    #if defined(_IRR_WCHAR_FILESYSTEM)
	if (waccess(fileName.c_str(), 0) == 0)
    #else
    if (access(fileName.c_str(), 0) == 0)
    #endif
    {
        struct stat status;
        stat( fileName.c_str(), &status );

        if (!( status.st_mode & S_IFDIR ))
        {
            _IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX
            return true;
        }
    }

    _IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX
	return false;
#endif
}

//! determines if a path exists
bool existDirectory(const io::path& fileName)
{
    // is there something to do with virtual filesystems
    bool returnValue = false;

    if (FSType == FILESYSTEM_NATIVE)
    {
//! Windows
#if defined(_IRR_WINDOWS_API_) || defined(_MSC_VER)
		path tmp = fileName;
    #if defined(_IRR_WCHAR_FILESYSTEM)
		tmp.replace(L'/', L'\\');
        u32 dwAttrib = GetFileAttributesW(tmp.c_str());
    #else
		tmp.replace('/', '\\');
        u32 dwAttrib = GetFileAttributesA(tmp.c_str());
    #endif
        _IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX
        return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
//! Linux
#elif defined(_IRR_POSIX_API_) || defined(_IRR_OSX_PLATFORM_)
        core::stringc tmp;
    #if defined(_IRR_WCHAR_FILESYSTEM)
        size_t n = wcstombs( NULL, fileName.c_str(), fileName.size());
        char* c = new char[n+1];
        memset(c,0,n+1);
        wcstombs( c, fileName.c_str(), n+1);
        tmp = c;
        delete [] c;
        tmp.replace(L'\\', L'/');
    #else
		tmp = fileName;
        tmp.replace('\\', '/');
    #endif
        if ( access( tmp.c_str(), 0 ) != -1 )
        {
            struct stat status;
            stat( tmp.c_str(), &status );

            if ( status.st_mode & S_IFDIR )
            {
                returnValue = true;
            }
        }
#endif
    }
    else
    {
        // do something for virtual FS
    }
    _IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX
    return returnValue;
}


path getWorkingDirectory()
{
	return WorkingDirectory;
}

bool setWorkingDirectory(const path& fileName)
{
	//! extract path drom filename
	path dir = getFileDirectory(fileName);

	//! create if not exist
	if ( !existDirectory(dir) )
		createDirectory(dir);

	//! set new working directory only if exist
	if (existDirectory(dir))
	{
		WorkingDirectory = dir;
		return true;
	}
	return false;
}

path getAbsolutePath(const path& fileName)
{
	path tmp = getExeDirectory();
	return tmp;
}

path getRelativePath(const path& fileFrom, const path& fileTo)
{
	path tmp = getExeDirectory();
	return tmp;
}


bool createTestFile(const path& fileName)
{
	path f = fileName;

#if defined(_IRR_WINDOWS_API_)
	#if defined(_IRR_WCHAR_FILESYSTEM)
	f.replace(L'/',L'\\');
	#else
	f.replace('/','\\');
	#endif

	for (u32 i=0; i<6; i++)
	{

		path c; // command c
		if (i==0) c = _IRR_TEXT("echo /? > ");
		if (i==1) c = _IRR_TEXT("copy /? >> ");
		if (i==2) c = _IRR_TEXT("del /? >> ");
		if (i==3) c = _IRR_TEXT("mkdir /? >> ");
		if (i==4) c = _IRR_TEXT("rmdir /? >> ");
		if (i==5) c = _IRR_TEXT("xcopy /? >> ");
		c += f;
	#if defined(_IRR_WCHAR_FILESYSTEM)
		s32 result = _wsystem( c.c_str() );
	#else
		s32 result = system( c.c_str() );
	#endif
		if (result!=0)
		{
			printf( "ERROR %s.\n", getErrorText().c_str());
		}
	}

#elif defined(_IRR_POSIX_API_)

	#if defined(_IRR_WCHAR_FILESYSTEM)
	f.replace(L'\\',L'/');
	#else
	f.replace('\\','/');
	#endif

	for (u32 i=0; i<8; i++)
	{
		path c; // command c
		if (i==0) c = _IRR_TEXT("echo --help > ");
		if (i==1) c = _IRR_TEXT("cp --help >> ");
		if (i==2) c = _IRR_TEXT("rm --help >> ");
		if (i==3) c = _IRR_TEXT("dd --help >> ");
		if (i==4) c = _IRR_TEXT("df --help >> ");
		if (i==5) c = _IRR_TEXT("stat --help >> ");
		if (i==6) c = _IRR_TEXT("mkdir --help >> ");
		if (i==7) c = _IRR_TEXT("rmdir --help >> ");
		c += f;
		s32 result = system( c.c_str() ); // exec command on bash
//		if (result!=0)
//		{
//			printf( "ERROR %s.\n", getErrorText().c_str());
//		}
	}

#else
	#warning no such thing
#endif
	return existFile(f);
}


//! returns the directory part of a filename, i.e. all until the first
path getFileDirectory(const path& fileName)
{
	path tmp = fileName;
	#if defined( _IRR_WINDOWS_API_)
		#if defined (_IRR_WCHAR_FILESYSTEM)
		tmp.replace(L'\\', L'/');
		#else
		tmp.replace('\\', '/');
		#endif
	#endif

	#if defined (_IRR_WCHAR_FILESYSTEM)
	s32 pos = tmp.findLast(L'/');
	#else
	s32 pos = tmp.findLast('/');
	#endif

	if (pos!=-1)
	{
		tmp = tmp.subString(0,(u32)pos);
	}
	else
	{
		tmp = _IRR_TEXT(".");
	}

	return tmp;
}


s32 createDirectoryOnce( const path& fileName, s32 mode)
{
#if defined (_IRR_WINDOWS_API_)
	return 0;
#else
    s32 status = 0;
    struct stat st;

	#if defined(_IRR_WCHAR_FILESYSTEM)
    if (_wstat(fileName.c_str(), &st) != 0)
	#else
	if (stat(fileName.c_str(), &st) != 0)
	#endif
    {
        //! Directory does not exist
		#if defined(_IRR_WCHAR_FILESYSTEM)
        //if (_wmkdir(filename.c_str(), (u16)mode) != 0)
        if (_wmkdir(fileName.c_str(), mode) != 0)
        #else
        // if (mkdir(filename.c_str(), (u16)mode) != 0)
        if (mkdir(fileName.c_str(), mode) != 0)
        #endif
            status = -1;
    }
    else if (!S_ISDIR(st.st_mode))
    {
        errno = ENOTDIR;
        status = -1;
    }

    return status;
#endif
}

//! creates a directory on native FileSystem or a virtual one, depending on FSType
bool createDirectory(const path& fileName, s32 mode)
{
	if (existDirectory(fileName))
	{
		_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX
		return false;
	}

	if (FSType == FILESYSTEM_NATIVE)
	{
//! WinAPI or cmd.exe
#if defined (_IRR_WINDOWS_API_)
		path tmp = fileName;
	#if defined(_IRR_WCHAR_FILESYSTEM)
		tmp.replace(L'/', L'\\');
	#else
		tmp.replace('/', '\\');
	#endif

	//! cmd.exe
	#if defined (_IRR_USE_SYSTEM_FOR_FILE_OPERATIONS_)
		path command = _IRR_TEXT("mkdir ");
		command += tmp;
		#if defined(_IRR_WCHAR_FILESYSTEM)
		_wsystem(command.c_str());
		#else
		system(command.c_str());
		#endif
	//! WinAPI
	#else
		#warning createDirectory() not fully implemented (no recursion)
		// something recursive
		BOOL result = FALSE;
		#if defined(_IRR_WCHAR_FILESYSTEM)
		result = CreateDirectoryW( fileName.c_str(), NULL);
		#else
		result = CreateDirectoryA( fileName.c_str(), NULL);
		#endif
		// end something recursive
	#endif
//! POSIX
#elif defined(_IRR_POSIX_API_)
	#warning createDirectory() POSIX Test.
    path command("mkdir -p ");
    command+=fileName;
    system( command.c_str() );


//		s32 status = 0;
//	#if defined(_IRR_WCHAR_FILESYSTEM)
//		wchar_t *pp, *sp;
//		wchar_t *copypath = _wstrdup(fileName.c_str());
//	#else
//		char *pp, *sp;
//		char *copypath = strdup(fileName.c_str());
//	#endif
//		pp = copypath;
//
//	#if defined(_IRR_WCHAR_FILESYSTEM)
//		while (status == 0 && (sp = _wstrchr(pp, L'/')) != 0)
//	#else
//		while (status == 0 && (sp = strchr(pp, '/')) != 0)
//    #endif
//		{
//			if (sp != pp)
//			{
//				/* Neither root nor double slash in path */
//				*sp = '\0';
//
//				status = createDirectoryOnce(copypath, mode);
//
//				*sp = '/';
//			}
//			pp = sp + 1;
//		}
//		if (status == 0)
//			status = createDirectoryOnce(fileName.c_str(), mode);
//		free(copypath);
#else
	#warning createDirectory() not implemented.
#endif
	}
	else
	{
		// Do something for Virtual FS
	}
    _IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX
    return existDirectory(fileName);
}

//! returns humanReadable file-size, source value in Bytes as multiple of 1000
core::stringc formatFileSizeK(u64 bytes)
{
	if (bytes == 0)
	{
		return core::stringc("0 Bytes");
	}

	core::stringc result("");

	if (bytes < 1000)
	{
		result += (u32)bytes;
		result += " Bytes";
		return result;
	}

	u8 potenz = 0;
	f64 value = bytes;
	while (value > 1000.0)
	{
		value /= 1000.0;
		potenz++;
	}

	char buf[32];
	snprintf(buf, 32, "%.2f", value);
	result+=buf;
	result+=" ";

	switch (potenz)
	{
		case 0: result+="Bytes"; break;
		case 1: result+="KB"; break;
		case 2: result+="MB"; break;
		case 3: result+="GB"; break;
		case 4: result+="TB"; break;
		case 5: result+="PB"; break;
		case 6: result+="EB"; break;
		default: result+="10^"; result+=3*potenz; break;
	}
	return result;
}

//! returns humanReadable file-size, source value in Bytes as multiple of 1024 (adds 'i' to all units)
core::stringc formatFileSize(u64 bytes)
{
	if (bytes == 0)
	{
		return core::stringc("0 Bytes");
	}

	core::stringc result("");

	if (bytes < 1024)
	{
		result += (u32)bytes;
		result += " Bytes";
		return result;
	}

	u8 potenz = 0;
	f64 value = bytes;
	while (value > 1024.0)
	{
		value /= 1024.0;
		potenz++;
	}

	char buf[32];
	snprintf(buf, 32, "%.2f", value);
	result+=buf;
	result+=" ";

	switch (potenz)
	{
		case 0: result+="Bytes"; break;
		case 1: result+="KiB"; break;
		case 2: result+="MiB"; break;
		case 3: result+="GiB"; break;
		case 4: result+="TiB"; break;
		case 5: result+="PiB"; break;
		case 6: result+="EiB"; break;
		default: result+="2^"; result+=10*potenz; break;
	}
	return result;
}

//! returns the file-size in Bytes
u64	getFileSize(const path& fileName)
{
    if (!existFile(fileName))
        return 0L;

	//! Posix variant
	FILE* f=fopen(fileName.c_str(), "r");
	if (!f)	return 0L;
	s64 siz(0);

#if defined (__GNUC__)
	#if !defined(_IRR_WINDOWS_)
	fseeko64(f, 0L, SEEK_END);
	siz = ftello64(f);
    #else
        #warning not implemented
    #endif
#endif

	fclose(f);
	return (u64)siz;
}

//! returns the directory-size in Bytes
u64	getDirectorySize(const path& dirName, s32 level)
{
	u64 uResult = 0L;

    if (!existDirectory(dirName))
        return uResult;

//#if defined(_IRR_WCHAR_FILESYSTEM)
//	printf("getDirectorySize(%ls)\n", fileName.c_str());
//#else
//	printf("getDirectorySize(%s)\n", fileName.c_str());
//#endif

#if defined(_IRR_WINDOWS_API_)
	path tmp = dirName;
	#if defined(_IRR_WCHAR_FILESYSTEM)
		tmp.replace(L'/', L'\\');
	#else
		tmp.replace('/', '\\');
	#endif
    path searchPattern = tmp + _IRR_TEXT("\\*.*");
	WIN32_FIND_DATAA data;
    HANDLE h = FindFirstFileA(searchPattern.c_str(), &data);
    if(h != INVALID_HANDLE_VALUE)
    {
        do
		{
            if( (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
            {
                // make sure we skip "." and "..".  Have to use strcmp here because
                if( strcmp(data.cFileName,".") != 0 &&strcmp(data.cFileName,"..") != 0)
                {
                    searchPattern = tmp + "\\" + data.cFileName;
                    uResult += getDirectorySize(searchPattern);
                }
            }
            else
            {
                LARGE_INTEGER sz;
                sz.LowPart = data.nFileSizeLow;
                sz.HighPart = data.nFileSizeHigh;
                uResult += sz.QuadPart;
            }
        }
        while( FindNextFileA(h,&data) != 0);
        FindClose(h);
    }
#else
    char chars[1024];
    memset(chars, 0, 1024);
    path c("du -s -P --block-size=1 ");
    if (level < 0) c += " ";
    if (level == 0) c += "-S ";
    c += dirName;
    FILE *f;
    if (( f = popen(c.c_str(), "r")) == NULL)
    {
        perror("popen");
        exit(1);
    }
    core::stringc tmp = "";
    while(fgets(chars, 1024, f))
    {
        tmp+=chars;
        memset(chars, 0, 1024);
    }
    pclose(f);
    u32 i = 0;
    u32 iStart = 0;
    u32 iEnd = 0;
    bool isInteger = false;

    while (i < tmp.size())
	{
		if (tmp[i] >= '0' && tmp[i] <= '9' )
		{
			isInteger = true;
			iStart = i;
		}
		else
		{
			if (isInteger)
			{
				iEnd = i;
				break;
			}
		}
		i++;
	}

	if (iEnd > iStart)
	{
		uResult = (u64)atoll(tmp.subString(iStart,iEnd-iStart).c_str());
	}
#endif
	return uResult;
}

//! creates a file on native FileSystem or a virtual one, depending on FSType
bool createFile(const path& fileName, s32 mode)
{
//    HANDLE WINAPI CreateFile(
//  __in      LPCTSTR lpFileName,
//  __in      DWORD dwDesiredAccess,
//  __in      DWORD dwShareMode,
//  __in_opt  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
//  __in      DWORD dwCreationDisposition,
//  __in      DWORD dwFlagsAndAttributes,
//  __in_opt  HANDLE hTemplateFile);

    // TouchFile();
    _IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX
    return false;
}

//! deletes a file from native FileSystem or a virtual one, depending on FSType
bool removeFile(const path& fileName)
{
    // is there something to do with virtual filesystems
    bool returnValue = false;

    if (FSType == FILESYSTEM_NATIVE)
    {
//! Windows
#if defined(_IRR_WINDOWS_API_)
    #if defined(_IRR_WCHAR_FILESYSTEM)
        if ( DeleteFileW( fileName.c_str() ))
    #else
        if ( DeleteFileA( fileName.c_str() ))
    #endif
        {
            returnValue = true;
        }
        else
        {
            printf("%s\n",getErrorText().c_str());
        }
//! Linux
#elif defined(_IRR_POSIX_API_)
    #if defined(_IRR_WCHAR_FILESYSTEM)
        if ( _wremove( fileName.c_str() ) == 0 )
    #else
        if ( remove( fileName.c_str() ) == 0 )
    #endif
        {
            returnValue = true;
        }
        else
        {
			printf("%s\n",getErrorText().c_str());
        }
#else
	#warning removeFile() not implemented.
#endif
    }
    else
	{
		// do something for virtual FS
	}
    _IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX
    return returnValue;
}

//! deletes a directory from native FileSystem or a virtual one, depending on FSType
bool removeDirectory(const path& dirName)
{
	if (!existDirectory(dirName))
	{
		_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX
		return false;
	}

    if (FSType == FILESYSTEM_NATIVE)
    {
//! Windows
#if defined(_IRR_WINDOWS_API_)
	//! make path Windows conform
		path tmp = dirName;
	#if defined(_IRR_WCHAR_FILESYSTEM)
		tmp.replace(L'/', L'\\');
	#else
		tmp.replace('/', '\\');
	#endif

	//! cmd.exe
	#if defined(_IRR_USE_SYSTEM_FOR_FILE_OPERATIONS_)
		path command = _IRR_TEXT("rmdir /S /Q ");
		command += tmp;
		#if defined(_IRR_WCHAR_FILESYSTEM)
		_wsystem(command.c_str());
		#else
		system(command.c_str());
		#endif
	//! shell (Advanced API)
	#elif defined(_IRR_USE_WINDOWS_SHELL_FOR_FILE_OPERATIONS_)
		#if defined(_IRR_WCHAR_FILESYSTEM)
		SHFILEOPSTRUCTW file_op = {
		#else
		SHFILEOPSTRUCT file_op = {
		#endif
			NULL,
			FO_DELETE,
			tmp.c_str(),
			_IRR_TEXT(""),
			FOF_NOCONFIRMATION |
			FOF_NOERRORUI |
			FOF_SILENT,
			false,
			0,
			_IRR_TEXT("")
		};
        #if defined(_IRR_WCHAR_FILESYSTEM)
		SHFileOperationW(&file_op);
		#else
		SHFileOperationA(&file_op);
		#endif
	//! WinAPI
	#else
		#warning removeDirectory() not fully implemented
		#if defined(_IRR_WCHAR_FILESYSTEM)
		if ( RemoveDirectoryW( tmp.c_str() ) == FALSE )
		#else
		if ( RemoveDirectoryA( tmp.c_str() ) == FALSE )
		#endif
		{
			printf("%s\n",getErrorText().c_str());
		}
	#endif
//! POSIX
#elif defined(_IRR_POSIX_API_)
	//! bash
	#if defined(_IRR_USE_SYSTEM_FOR_FILE_OPERATIONS_)
		path command = _IRR_TEXT("rmdir ");
		command += dirName;
		#if defined(_IRR_WCHAR_FILESYSTEM)
		s32 status = _wsystem(command.c_str());
		#else
		s32 status = system(command.c_str());
		#endif
		if (status!=0)
    //! native API
	#else
		#warning removeDirectory() not fully implemented
		#if defined(_IRR_WCHAR_FILESYSTEM)
		if ( _wrmdir( dirName.c_str() ) != 0 )
		#else
		if ( rmdir( dirName.c_str() ) != 0 )
		#endif
	#endif
		{
			printf("%s\n",getErrorText().c_str());
		}
#else
	#warning removeDirectory() not implemented.
#endif
	}
	else
	{
        // Do something for virtual FS
    }

	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX
    return !existDirectory(dirName);
}

//! copy the contents of a directory to a new location
bool copyFile(const path& srcName, const path& dstName)
{
    // is there something to do with virtual filesystems
    bool returnValue = false;

    if (FSType == FILESYSTEM_NATIVE)
    {
//! Windows
#if defined(_IRR_WINDOWS_API_)
    path srcTmp = srcName;
    path dstTmp = dstName;
    #if defined(_IRR_WCHAR_FILESYSTEM)
    srcTmp.replace(L'/', L'\\');
    dstTmp.replace(L'/', L'\\');
    #else
    srcTmp.replace('/', '\\');
    dstTmp.replace('/', '\\');
    #endif

    //! cmd.exe
	#if defined(_IRR_USE_SYSTEM_FOR_FILE_OPERATIONS_)
        path command = _IRR_TEXT("copy ");
        command += srcTmp;
        command += _IRR_TEXT(" ");
        command += dstTmp;
        #if defined(_IRR_WCHAR_FILESYSTEM)
        s32 status = _wsystem(command.c_str());
        #else
        s32 status = system(command.c_str());
        #endif
        if (status==0)
    #else
        #warning copyFile() not fully implemented!
        BOOL bFailIfExist = FALSE;
        #if defined(_IRR_WCHAR_FILESYSTEM)
        if ( CopyFileW( srcTmp.c_str(), dstTmp.c_str(), bFailIfExist ) == 0 )
        #else
        if ( CopyFileA( srcTmp.c_str(), dstTmp.c_str(), bFailIfExist ) == 0 )
        #endif
    #endif
        {
            returnValue = true;
        }
        else
        {
            printf("%s\n",getErrorText().c_str());
        }
//! Linux
#elif defined(_IRR_POSIX_API_)
	//! bash
	#if defined(_IRR_USE_SYSTEM_FOR_FILE_OPERATIONS_)
        path command = _IRR_TEXT("/bin/cp -f -R ");
        command += srcName;
        command += _IRR_TEXT(" ");
        command += dstName;

        //! use popen
        #if _POSIX_C_SOURCE >= 2 || _XOPEN_SOURCE || _BSD_SOURCE || _SVID_SOURCE
        FILE *f = popen(command.c_str(), "r");
        if (!f)
        {
            puts("Failed to run system command.\n");
            _IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX
            return false;
        }
        returnValue = true;
        pclose(f);
        //!
        #else
        s32 status = system()
        #endif
    //! native API
    #else
        #if defined(_IRR_WCHAR_FILESYSTEM)
        if ( _cp( srcName.c_str(), dstName.c_str() ) == 0 )
        #else
        if ( cp( srcName.c_str(), dstName.c_str() ) == 0 )
        #endif
        {
            returnValue = true;
        }
        else
        {
            printf("%s\n",getErrorText().c_str());
        }
    #endif
#else
	#warning copyFile() not implemented.
#endif
    }
    else
    {
        // do something for virtual FS i dont care
    }
    // do something for virtual FS i dont care
    _IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX
    return returnValue;
}

//! copy the contents of a directory to a new location
bool copyDirectory(const path& srcName, const path& dstName)
{
    // is there something to do with virtual filesystems
    bool returnValue = false;

    if (FSType == FILESYSTEM_NATIVE)
    {
//! Windows
#if defined(_IRR_WINDOWS_API_)
		path srcTmp = srcName;
		path dstTmp = dstName;
	#if defined(_IRR_WCHAR_FILESYSTEM)
		srcTmp.replace(L'/', L'\\');
		dstTmp.replace(L'/', L'\\');
	#else
		srcTmp.replace('/', '\\');
		dstTmp.replace('/', '\\');
	#endif
	#if defined (_IRR_USE_SYSTEM_FOR_FILE_OPERATIONS_)
		path command = _IRR_TEXT("copy /Y ");
		command += srcTmp;
		command += _IRR_TEXT(" ");
		command += dstTmp;
		#if defined(_IRR_WCHAR_FILESYSTEM)
		s32 result = _wsystem(command.c_str());
		#else
		s32 result = system(command.c_str());
		#endif
	#else
		HANDLE hFind;
		#if defined(_IRR_WCHAR_FILESYSTEM)
		WIN32_FIND_DATAW FindFileData;
		wchar_t l_szTmp[1025] = {0};
		wchar_t l_szSrcPath[1025] = {0};
		wchar_t l_szDesPath[1025] = {0};
		wchar_t l_szNewSrcPath[1025] = {0};
		wchar_t l_szNewDesPath[1025] = {0};
		memcpy(l_szTmp,srcTmp.c_str(),sizeof(wchar_t)*1024);
		memcpy(l_szSrcPath,srcTmp.c_str(),sizeof(wchar_t)*1024);
		memcpy(l_szDesPath,dstTmp.c_str(),sizeof(wchar_t)*1024);
		wcscat(l_szTmp,L"*");
		hFind = FindFirstFileW(l_szTmp, &FindFileData);
		if(hFind == NULL) return false;
		do
		{
			if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if(wcscmp(FindFileData.cFileName,L"."))
				{
					if(wcscmp(FindFileData.cFileName,L".."))
					{
						wprintf(L"The Directory found is %ls ", FindFileData.cFileName);
						wsprintf(l_szNewDesPath,L"%ls%ls\"",l_szDesPath,FindFileData.cFileName);
						wsprintf(l_szNewSrcPath,L"%ls%ls\"",l_szSrcPath,FindFileData.cFileName);
						CreateDirectoryW(l_szNewDesPath,NULL);
						copyDirectory(path(l_szNewSrcPath),path(l_szNewDesPath));
					}
				}
			}
			else
			{
				wprintf (L"The File found is %ls ", FindFileData.cFileName);
				wchar_t l_szSrcFile[1025] = {0};
				wchar_t l_szDesFile[1025] = {0};
				wsprintf(l_szDesFile,L"%ls%ls",l_szDesPath,FindFileData.cFileName);
				wsprintf(l_szSrcFile,L"%ls%ls",l_szSrcPath,FindFileData.cFileName);
				BOOL l_bRet = CopyFileW(l_szSrcFile,l_szDesFile,TRUE);
			}
		}
		while(FindNextFileW(hFind, &FindFileData));
		FindClose(hFind);
		return true;

		#else
		WIN32_FIND_DATAA FindFileData;
		c8 l_szTmp[1025] = {0};
		c8 l_szSrcPath[1025] = {0};
		c8 l_szDesPath[1025] = {0};
		c8 l_szNewSrcPath[1025] = {0};
		c8 l_szNewDesPath[1025] = {0};
		memcpy(l_szTmp,srcTmp.c_str(),1024);
		memcpy(l_szSrcPath,srcTmp.c_str(),1024);
		memcpy(l_szDesPath,dstTmp.c_str(),1024);
		strcat(l_szTmp,"*");
		hFind = FindFirstFileA(l_szTmp, &FindFileData);
		if(hFind == NULL) return false;
		do
		{
			if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if(strcmp(FindFileData.cFileName,"."))
				{
					if(strcmp(FindFileData.cFileName,".."))
					{
						printf ("The Directory found is %s ", FindFileData.cFileName);
						sprintf(l_szNewDesPath,"%s%s\"",l_szDesPath,FindFileData.cFileName);
						sprintf(l_szNewSrcPath,"%s%s\"",l_szSrcPath,FindFileData.cFileName);
						CreateDirectoryA(l_szNewDesPath,NULL);
						copyDirectory(path(l_szNewSrcPath),path(l_szNewDesPath));
					}
				}
			}
			else
			{
				printf ("The File found is %s ", FindFileData.cFileName);
				char l_szSrcFile[1025] = {0};
				char l_szDesFile[1025] = {0};
				sprintf(l_szDesFile,"%s%s",l_szDesPath,FindFileData.cFileName);
				sprintf(l_szSrcFile,"%s%s",l_szSrcPath,FindFileData.cFileName);
				BOOL l_bRet = CopyFileA(l_szSrcFile,l_szDesFile,TRUE);
			}
		}
		while(FindNextFileA(hFind, &FindFileData));
		FindClose(hFind);
		returnValue = true;
		#endif
	#endif
//! POSIX
#elif defined (_IRR_POSIX_API_)
	#if defined (_IRR_USE_SYSTEM_FOR_FILE_OPERATIONS_)
		path command = _IRR_TEXT("cp -f -R");
		command += srcName;
		command += _IRR_TEXT(" ");
		command += dstName;
		#if defined(_IRR_WCHAR_FILESYSTEM)
		s32 length = wcstombs( NULL, command.c_str(), 0);
		if (!length)
        {
            puts("ERROR in removeDirectory() could not retrieve needed mbstring-size");
            _IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX
            return false;
        }
        c8* buffer = new c8[length+1];
        if (!buffer)
        {
            printf("ERROR in removeDirectory() could not allocate buffer with size (%i)",length);
            _IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX
            return false;
        }
        memset((void*)buffer, 0, length+1);
        length = wcstombs( buffer, command.c_str(), length+1);
        core::stringc tmpCStr = buffer;
        delete [] buffer;
		s32 result = system(tmpCStr.c_str());
		#else
		s32 result = system(command.c_str());
		#endif
        if (result!=0)
        {
            printf("%s\n",getErrorText().c_str());
            _IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX
            return false;
        }
        returnValue = true;
	#else
        #warning copyDirectory() not implemented.
    #endif
#else
	#warning copyDirectory() not implemented.
#endif
    }
    else
    {
        // do something for virtual FS i dont care
    }
    // do something for virtual FS i dont care
    _IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX
    return returnValue;
}

//! moves one or more files to a new location
bool moveFile(const path& srcName, const path& dstName)
{
    // is there something to do with virtual filesystems
    bool returnValue = false;

    if (FSType == FILESYSTEM_NATIVE)
    {
//! Windows
#if defined(_IRR_WINDOWS_API_)
    #ifndef _MSC_VER
        #warning moveFile() not implemented.
    #endif
//    #if defined(_IRR_WCHAR_FILESYSTEM)
//        if ( RemoveDirectoryW( filename.c_str() ) == 0 )
//    #else
//        if ( RemoveDirectoryA( filename.c_str() ) == 0 )
//    #endif
//        {
//            returnValue = true;
//        }
//        else
//        {
//            os::Printer::log( getErrorText().c_str() );
//        }

//! Linux
#elif defined(_IRR_POSIX_API_)
    #ifndef _MSC_VER
	    #warning moveFile() not implemented.
    #endif
//    #if defined(_IRR_WCHAR_FILESYSTEM)
//        if ( _wrmdir( filename.c_str() ) == 0 )
//    #else
//        if ( rmdir( filename.c_str() ) == 0 )
//    #endif
//        {
//            returnValue = true;
//        }
//        else
//        {
//            perror( "ERROR in removeDirectory() const" );
//        }
#else
	#warning moveFile() not implemented.
#endif
    }
    else
    {
        // do something for virtual FS i dont care
    }
    // do something for virtual FS i dont care
    _IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX
    return returnValue;
}

//! copy the contents of a directory to a new location
bool moveDirectory(const path& srcName, const path& dstName)
{
    // is there something to do with virtual filesystems
    bool returnValue = false;

    if (FSType == FILESYSTEM_NATIVE)
    {
//! Windows
#if defined(_IRR_WINDOWS_API_)
    #ifndef _MSC_VER
	    #warning moveDirectory() not implemented.
    #endif
//    #if defined(_IRR_WCHAR_FILESYSTEM)
//        if ( RemoveDirectoryW( filename.c_str() ) == 0 )
//    #else
//        if ( RemoveDirectoryA( filename.c_str() ) == 0 )
//    #endif
//        {
//            returnValue = true;
//        }
//        else
//        {
//            os::Printer::log( getErrorText().c_str() );
//        }

//! Linux
#elif defined (_IRR_POSIX_API_)
	#warning moveDirectory() not implemented.
//    #if defined(_IRR_WCHAR_FILESYSTEM)
//        if ( _wrmdir( filename.c_str() ) == 0 )
//    #else
//        if ( rmdir( filename.c_str() ) == 0 )
//    #endif
//        {
//            returnValue = true;
//        }
//        else
//        {
//            perror( "ERROR in removeDirectory() const" );
//        }
#else
	#warning moveDirectory() not implemented.
#endif
    }
    else
    {
        // do something for virtual FS i dont care
    }
    // do something for virtual FS i dont care
    _IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX
    return returnValue;
}

//! get the filename without path
path getFileName( const path& fileName )
{
	path tmp = fileName;

#if defined(_IRR_WINDOWS_API_)
	#if defined (_IRR_WCHAR_FILESYSTEM)
	tmp.replace(L'\\', L'/');
	#else
	tmp.replace('\\', '/');
	#endif
#endif

	s32 pos = tmp.findLast ( '/' );
	if ( pos < 0 )
		return tmp;

	return tmp.subString( pos+1, tmp.size() - (pos+1) );
}

//! get the filename without path and extension
path getFileBaseName ( const path& fileName )
{
	path tmp = getFileName(fileName);
	s32 pos = fileName.findLast ( '.' );
	if ( pos < 0 )
		return tmp;

	return tmp.subString ( 0, pos );
}


//! get the filename extension from a file path
path getFileExtension ( const path& fileName )
{
	path dst("");
	s32 pos = fileName.findLast ( '.' );
	if ( pos < 0 )
		return dst;

	dst = fileName.subString ( pos+1, fileName.size ()-pos-1 );
	return dst;
}

path createUniqueFileName( const path& fileName )
{
	if (!existFile(fileName))
		return fileName;

	path fileBase;
	path fileExt;

	u32 siz = fileName.size();
	s32 pos = fileName.findLast('.');
	if (pos >= 0)
	{
		fileBase = fileName.subString( 0,(u32)pos );
		fileExt = ".";
		fileExt += fileName.subString( pos+1, siz-(pos+1) );
	}
	else
	{
		fileBase = fileName;
		fileExt = ".unknown";
	}

	u32 i(0);
	path result(fileBase + fileExt);

	while (existFile(result))
	{
		result = fileBase;
		result += "_";
		if (i<1000) result+="0";
		if (i<100) result+="0";
		if (i<10) result+="0";
		result += i;
		result += fileExt;
		i++;
	}
//
//	FILE* f; // Zeiger auf Datenstrom der Datei
//	f = fopen( result.c_str(),"w"); // Datei neu erzeugen
//	if (!f)
//	{
//		printf("ERROR: could not create or get write access to file %s", result.c_str());
//		return result;
//	}
//
//	fclose(f); // Datei schließen
	return result;
}

path createUniqueDirectoryName( const path& baseName )
{
	const path dirName = baseName;

	if (!existDirectory(dirName))
		return dirName;

	u32 i(0);
	path result = dirName;
	while (existDirectory(result))
	{
		result = dirName;
		result += "_";
		if (i<1000) result+="0";
		if (i<100) result+="0";
		if (i<10) result+="0";
		result += i;
		i++;
	}
	return result;
}

bool renameFile(const path& srcName, const path& dstName)
{
    #ifndef _MSC_VER
		#warning renameFile() not implemented yet!
    #endif


	return false;
}
bool renameDirectory(const path& srcName, const path& dstName)
{
    #ifndef _MSC_VER
	    #warning renameDirectory() not implemented yet!
    #endif


	return false;
}
u32	countFiles(const path& srcName, s32 level)
{
	u32 uResult = 0;

    if (!existDirectory(srcName))
        return uResult;

#if defined(_IRR_WINDOWS_API_)
	path tmp = srcName;
	#if defined(_IRR_WCHAR_FILESYSTEM)
		tmp.replace(L'/', L'\\');
	#else
		tmp.replace('/', '\\');
	#endif
    path searchPattern = tmp + _IRR_TEXT("\\*.*");
	WIN32_FIND_DATAA data;
    HANDLE h = FindFirstFileA(searchPattern.c_str(), &data);
    if(h != INVALID_HANDLE_VALUE)
    {
        do
		{
            if( (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
            {
                // make sure we skip "." and "..".  Have to use strcmp here because
                if( strcmp(data.cFileName,".") != 0 &&strcmp(data.cFileName,"..") != 0)
                {
                    searchPattern = tmp + "\\" + data.cFileName;
                    uResult += countFiles(searchPattern, level-1);
                }
            }
            else
            {
                uResult++;
            }
        }
        while( FindNextFileA(h,&data) != 0);

        FindClose(h);
    }
#elif defined(_IRR_POSIX_API_)
	#warning countFiles() not implemented.
#else
	#warning countFiles() not implemented.
#endif
	return uResult;
}
u32	countDirectories(const path& srcName, s32 level)
{
	u32 uResult = 0;

    if (!existDirectory(srcName))
        return uResult;

#if defined(_IRR_WINDOWS_API_)
	path tmp = srcName;
	#if defined(_IRR_WCHAR_FILESYSTEM)
		tmp.replace(L'/', L'\\');
	#else
		tmp.replace('/', '\\');
	#endif
    path searchPattern = tmp + _IRR_TEXT("\\*.*");
	WIN32_FIND_DATAA data;
    HANDLE h = FindFirstFileA(searchPattern.c_str(), &data);
    if(h != INVALID_HANDLE_VALUE)
    {
        do
		{
            if( (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
            {
                // make sure we skip "." and "..".  Have to use strcmp here because
                if( strcmp(data.cFileName,".") != 0 &&strcmp(data.cFileName,"..") != 0)
                {
                    searchPattern = tmp + "\\" + data.cFileName;
                    uResult++;
                    uResult+=countDirectories(searchPattern, level-1);
                }
            }
        }
        while( FindNextFileA(h,&data) != 0);

        FindClose(h);
    }
#elif defined(_IRR_POSIX_API_)
	#warning countDirectories() not implemented.
#else
	#warning countDirectories() not implemented.
#endif
	return uResult;

}

//gui::IGUIImageList* createImageList_for_FileBrowser( gui::IGUIEnvironment* guienv )
//{
//	printf( "createImageList_for_FileBrowser()\n");
//
//	if (!guienv)
//		return 0;
//
//	video::IVideoDriver* driver = guienv->getVideoDriver();
//
//	bool bMipmaps = driver->getTextureCreationFlag( video::ETCF_CREATE_MIP_MAPS);
//	driver->setTextureCreationFlag( video::ETCF_CREATE_MIP_MAPS, false);
//	video::ITexture* tbImages = driver->getTexture("../media/toolbar/toolbar.png");
//	driver->setTextureCreationFlag( video::ETCF_CREATE_MIP_MAPS, bMipmaps);
//
//	if (tbImages)
//	{
//		gui::IGUIImageList* imageList =	guienv->createImageList( tbImages, core::dimension2di(24,24), true);
//		if (imageList)
//		{
//			printf( "imageList has %i images á (%i x %i) pixel.\n", imageList->getImageCount(), imageList->getImageSize().Width, imageList->getImageSize().Height);
//			return imageList;
//		}
//	}
//
//	return 0;
//}
//
//bool populateTreeViewWithFiles( gui::IGUITreeView* tv, IFileSystem* fs, const io::path& rootDir, s32 sublevels)
//{
//	printf("populateTreeViewWithFiles()\n");
//
//	if (!tv)
//		return false;
//
//	if (!fs)
//		return false;
//
//	io::path oldDir = fs->getWorkingDirectory();
//
//	if (!fs->changeWorkingDirectoryTo( rootDir ))
//	{
//		printf("working dir not changed.\n");
//	}
//
//	io::IFileList* files = fs->createFileList();
//	if (!files)
//	{
//		fs->changeWorkingDirectoryTo( oldDir );
//		return false;
//	}
//
//	gui::IGUITreeViewNode* rootNode = tv->getRoot();
//	if (!rootNode)
//	{
//		files->drop();
//		fs->changeWorkingDirectoryTo( oldDir );
//		return false;
//	}
//	rootNode->clearChildren();
//
//	//! get image-list
//	gui::IGUIImageList* imgList = tv->getImageList();
//	u32 imgCount = 0;
//	if (imgList)
//	{
//		imgCount = imgList->getImageCount();
//	}
//
//	for (u32 i=0; i<files->getFileCount(); ++i)
//	{
//		if (files->isDirectory(i))
//		{
//			s32 imgIndex = -1;
//			if (imgList && imgCount>2)
//				imgIndex = 2;
//			rootNode->addChildBack( core::stringw(files->getFileName(i)).c_str(), L"", imgIndex, -1, 0, 0);
//		}
//
//	}
//	for (u32 i=0; i<files->getFileCount(); ++i)
//	{
//		if (!files->isDirectory(i))
//		{
//			s32 imgIndex = -1;
//			if (imgList && imgCount>7)
//				imgIndex = 7;
//			rootNode->addChildBack( core::stringw(files->getFileName(i)).c_str(), L"", imgIndex, -1, 0, 0);
//		}
//
//	}
//
//	files->drop();
//	fs->changeWorkingDirectoryTo( oldDir );
//	return true;
//}

core::stringc formatNumber(u32 value, E_NUMBER_FORMAT srcType)
{
	core::stringc txt("");
	u32 c = 0;

	while (value>=1024)
	{
		value>>=10;
		c++;
	};

	txt+=(s32)value;
	txt+=" ";

	u32 dstType = (u32)srcType + c;

	if (dstType > (u32)ENF_COUNT)
		dstType = (u32)ENF_COUNT;

	switch ((E_NUMBER_FORMAT)dstType)
	{
		case ENF_BYTES: txt+="Bytes"; break;
		case ENF_KILO: txt+="KiB"; break;
		case ENF_MEGA: txt+="MiB"; break;
		case ENF_GIGA: txt+="GiB"; break;
		case ENF_TERA: txt+="TiB"; break;
		case ENF_PETA: txt+="PiB"; break;
		case ENF_EXA: txt+="EiB"; break;
		case ENF_COUNT: txt+="2^"; txt+=(s32)((u32)srcType+c); break;
		default: break;
	}

	txt+=" = 2^"; txt+=(s32)((u32)srcType+c);

	return txt;
}

} // END NAMESPACE IO
} // END NAMESPACE IRR
