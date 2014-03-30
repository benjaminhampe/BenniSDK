/** FileSystem Extensions

2012 by BenjaminHampe@gmx.de

*/

#ifndef __IRR_FILESYSTEM_EXTENSIONS_H__
#define __IRR_FILESYSTEM_EXTENSIONS_H__

#include <IrrCompileConfig.h>
#include <irrTypes.h>
#include <irrString.h>
#include <irrArray.h>
#include <path.h>
#include <IFileSystem.h>

//! new define, will the functions use Windows Shell Operations, if _IRR_WINDOWS_API_ is defined
#define _IRR_USE_WINDOWS_SHELL_FOR_FILE_OPERATIONS_

//! new define, will the functions use system() commands?
#define _IRR_USE_SYSTEM_FOR_FILE_OPERATIONS_

#if defined (_IRR_WINDOWS_API_)
    #include <windows.h>
	#include <direct.h> // for _chdir
	#include <io.h> // for _access
	#include <tchar.h>
	#if !defined ( _WIN32_WCE )

	#endif
#else
	#if (defined(_IRR_POSIX_API_) || defined(_IRR_OSX_PLATFORM_))
		#define _FILE_OFFSET_BITS 64 // before including the system headers That will more or less define fseek to be actually fseek64
		#include <unistd.h>
		#include <cstdio>
		#include <cstdlib>
		#include <cstring>
		#include <climits>
		#include <errno.h>
		#include <sys/types.h>
		#include <sys/stat.h>
		#include <sys/statfs.h> // for getDriveList()
		#include <dirent.h>

		//! different filesystem type-values
        //#include <linux/affs_fs.h>
        //#include <linux/efs_fs.h>
        //#include <linux/ext_fs.h>
        //#include <linux/ext2_fs.h>
        //#include <linux/hpfs_fs.h>
        //#include <linux/iso_fs.h>
        //#include <linux/minix_fs.h>
        //#include <linux/msdos_fs.h>
        //#include <linux/ncp_fs.h>
        //#include <linux/nfs_fs.h>
        //#include <linux/proc_fs.h>
        //#include <linux/smb_fs.h>
        //#include <linux/sysv_fs.h>
        //#include <linux/ufs_fs.h>
        //#include <linux/xfs_fs.h>
        //#include <linux/xia_fs.h>

        #define AFFS_SUPER_MAGIC 0xADFF
        #define EFS_SUPER_MAGIC 0x00414A53
        #define EXT_SUPER_MAGIC 0x137D
        #define EXT2_OLD_SUPER_MAGIC 0xEF51
        #define EXT2_SUPER_MAGIC 0xEF53
        #define HPFS_SUPER_MAGIC 0xF995E849
        #define ISOFS_SUPER_MAGIC 0x9660
        #define MINIX_SUPER_MAGIC 0x137F /* orig. minix */
        #define MINIX_SUPER_MAGIC2 0x138F /* 30 char minix */
        #define MINIX2_SUPER_MAGIC 0x2468 /* minix V2 */
        #define MINIX2_SUPER_MAGIC2 0x2478 /* minix V2, 30 char names */
        #define MSDOS_SUPER_MAGIC 0x4d44
        #define NCP_SUPER_MAGIC 0x564c
        #define NFS_SUPER_MAGIC 0x6969
        #define PROC_SUPER_MAGIC 0x9fa0
        #define SMB_SUPER_MAGIC 0x517B
        #define XENIX_SUPER_MAGIC 0x012FF7B4
        #define SYSV4_SUPER_MAGIC 0x012FF7B5
        #define SYSV2_SUPER_MAGIC 0x012FF7B6
        #define COH_SUPER_MAGIC 0x012FF7B7
        #define UFS_MAGIC 0x00011954
        #define XFS_SUPER_MAGIC 0x58465342
        #define _XIAFS_SUPER_MAGIC 0x012FD16D


	#endif
#endif

//! use shell or system(), system() has priority and will #undef shell (advapi32)
#if defined(_IRR_USE_SYSTEM_FOR_FILE_OPERATIONS_)
	#undef _IRR_USE_WINDOWS_SHELL_FOR_FILE_OPERATIONS_
#endif

#if defined(_IRR_WINDOWS_API_) || defined(_MSC_VER)
	#if defined(_IRR_USE_WINDOWS_SHELL_FOR_FILE_OPERATIONS_)
        #pragma comment(lib, "advapi32.lib") //! for shell operations
	#endif
#endif

namespace irr{
namespace io{

//! emulate IFIleSystem class, so we dont need to change functions
//! BIG-BUT: Windows already defined a enum member FileSystemType,
//! in a really creepy way! See "winnt.h" typedef enum _CM_SERVICE_NODE_TYPE

enum E_NUMBER_FORMAT
{
	ENF_BYTES=0,
	ENF_KILO=1,
	ENF_MEGA=2,
	ENF_GIGA=3,
	ENF_TERA=4,
	ENF_PETA=5,
	ENF_EXA=6,
	ENF_COUNT=7
};


core::stringc formatNumber(u32 value, E_NUMBER_FORMAT srcType = ENF_KILO);

//! for getDriveList()
enum E_LOGICAL_DRIVE_TYPE
{
	ELDT_UNKNOWN = 0,
	ELDT_NOT_EXIST,
	ELDT_REMOVABLE,
	ELDT_FIXED,
	ELDT_REMOTE,
	ELDT_CDROM,
	ELDT_RAMDISK
};

//! new names
const c8* const E_LOGICAL_DRIVE_TYPE_NAMES[] = {
	"ELDT_UNKNOWN",
	"ELDT_NOT_EXIST",
	"ELDT_REMOVABLE",
	"ELDT_FIXED",
	"ELDT_REMOTE",
	"ELDT_CDROM",
	"ELDT_RAMDISK"
};

#if IRRLICHT_VERSION_MINOR < 8
	typedef unsigned long long int u64;
	typedef signed long long int s64;
#endif

//! holding information about available physical drives
struct SDriveInfo
{
	path Name;
	E_LOGICAL_DRIVE_TYPE Type;
	u64 AvailSpace; // in KiB
	u64 TotalSpace; // in KiB
	u32 OptimalBlockSize; // in Bytes
};

/************************************************************************************

	FUNCTION PROTOTYPES/DECLARATIONS

************************************************************************************/

//! beta state
core::stringc 			getErrorText();

void 					getDriveList();
u32 					getDriveCount();
path 					getDriveName( u32 i );
u64 					getDriveAvailSpace( u32 i );
u64 					getDriveTotalSpace( u32 i );
u32 					getDriveOptimalBlockSize( u32 i );
E_LOGICAL_DRIVE_TYPE 	getDriveType( u32 i );

path 					getExeFile();
path 					getExeDirectory();

bool 					existFile(const path& fileName);
bool 					existDirectory(const path& fileName);

path 					getFileDirectory(const path& fileName);
path 					getFileName( const path& fileName );
path 					getFileBaseName( const path& fileName );
path 					getFileExtension( const path& fileName );
u64						getFileSize(const path& fileName);
u64						getDirectorySize(const path& dirName, s32 level = 0);

path 					createUniqueFileName( const path& fileName );
path 					createUniqueDirectoryName( const path& baseName );
bool 					createTestFile(const path& fileName);
bool 					createDirectory(const path& fileName, s32 mode = 0777);

//! returns humanReadable file-size, source value in Bytes as multiple of 1000
core::stringc 			formatFileSizeK(u64 bytes);

//! returns humanReadable file-size, source value in Bytes as multiple of 1024 (adds 'i' to all units)
core::stringc 			formatFileSize(u64 bytes);

//! alpha state
bool 					removeFile(const path& fileName);
bool 					removeDirectory(const path& dirName);
bool 					copyFile(const path& srcName, const path& dstName);
bool 					copyDirectory(const path& srcName, const path& dstName);
bool 					moveFile(const path& srcName, const path& dstName);
bool 					moveDirectory(const path& srcName, const path& dstName);
bool 					renameFile(const path& srcName, const path& dstName);
bool 					renameDirectory(const path& srcName, const path& dstName);
u32						countFiles(const path& srcName, s32 level = 0);
u32						countDirectories(const path& srcName, s32 level = 0);


//! not touched yet
path 					getWorkingDirectory();
bool 					setWorkingDirectory(const path& fileName);
path 					getAbsolutePath(const path& fileName);
path 					getRelativePath(const path& fileFrom, const path& fileTo = getExeDirectory());

//! not touched yet

void 					printDir( const path& srcPath );

core::array<path>		getFileAndDirectoryNames(const path& pathName, s32 level = 0);
core::array<path>		getFileNames(const path& pathName, s32 level = 0);
core::array<path>		getDirectoryNames(const path& pathName, s32 level = 0);

enum E_FILE_ATTRIBUTES{
	EFA_READ = 1,
	EFA_WRITE = 2,
	EFA_EXECUTE = 4,
	EFA_HIDDEN = 8,
	EFA_NORMAL = EFA_READ | EFA_WRITE | EFA_EXECUTE
};

E_FILE_ATTRIBUTES		getFileAttributes(const path& fileName);
E_FILE_ATTRIBUTES		getDirectoryAttributes(const path& fileName);

u32						getFileDate(const path& fileName);
u32						getDirectoryDate(const path& fileName);

u32						getFileCreationDate(const path& fileName);
u32						getDirectoryCreationDate(const path& fileName);

core::stringc			getFileMIME(const path& fileName);

bool					sortByName( core::array<path>& files, bool bSortDirectoriesFirst);
bool					sortBySize( core::array<path>& files, bool bSortDirectoriesFirst);
bool					sortByType( core::array<path>& files, bool bSortDirectoriesFirst);
bool					sortByDate( core::array<path>& files, bool bSortDirectoriesFirst);

core::array<path>		listFiles(const path& pathName, s32 level = 0);
core::array<path>		listDirectories(const path& pathName, s32 level = 0);
core::array<path>		listFilesAndDirectories(const path& pathName, s32 level = 0);

//gui::IGUIImageList* createImageList_for_FileBrowser( gui::IGUIEnvironment* guienv );
//bool populateTreeViewWithFiles( gui::IGUITreeView* tv, IFileSystem* fs, const io::path& rootDir, s32 sublevels = 0);

} // END NAMESPACE IO
} // END NAMESPACE IRR

#endif // __IRR_FILESYSTEM_EXTENSIONS_H__
