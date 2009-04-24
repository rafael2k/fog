// [Fog/Core Library - C++ API]
//
// [Licence] 
// MIT, See COPYING file in package

// [Precompiled headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/DirIterator.h>
#include <Fog/Core/Error.h>
#include <Fog/Core/FileUtil.h>
#include <Fog/Core/String.h>
#include <Fog/Core/StringUtil.h>
#include <Fog/Core/TextCodec.h>
#include <Fog/Core/UserInfo.h>

#if defined(FOG_OS_WINDOWS)
// windows.h is already included in Core/Build.h
#include <io.h>
#else
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#if defined(FOG_HAVE_UNISTD_H)
#include <unistd.h>
#endif
#endif

namespace Fog {

// ============================================================================
// [Fog::DirIterator::Entry]
// ============================================================================

DirIterator::Entry::Entry() 
{
}

DirIterator::Entry::Entry(const Entry& other) :
  _name(other._name),
  _type(other._type),
  _size(other._size)
{
#if defined(FOG_OS_WINDOWS)
  memcpy(&_winFindData, &other._winFindData, sizeof(WIN32_FIND_DATAW));
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  memcpy(&_statInfo, &other._statInfo, sizeof(struct stat));
#endif // FOG_OS_POSIX
}

DirIterator::Entry::~Entry()
{
}

DirIterator::Entry& DirIterator::Entry::operator=(const Entry& other)
{
  _name = other._name;
  _type = other._type;
  _size = other._size;

#if defined(FOG_OS_WINDOWS)
  memcpy(&_winFindData, &other._winFindData, sizeof(WIN32_FIND_DATAW));
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  memcpy(&_statInfo, &other._statInfo, sizeof(struct stat));
#endif // FOG_OS_POSIX

  return *this;
}

// ============================================================================
// [Fog::DirIterator]
// ============================================================================

DirIterator::DirIterator() :
  _handle(NULL)
#if defined(FOG_OS_WINDOWS)
  , _position(-1)
  , _fileInEntry(false)
#endif // FOG_OS_WINDOWS
  , _skipDots(true)
{
#if defined(FOG_OS_WINDOWS)
  memset(&_winFindData, 0, sizeof(WIN32_FIND_DATAW));
#endif // FOG_OS_WINDOWS
}

DirIterator::DirIterator(const String32& path) :
  _handle(NULL)
#if defined(FOG_OS_WINDOWS)
  , _position(-1)
  , _fileInEntry(false)
#endif // FOG_OS_WINDOWS
  , _skipDots(true)
{
#if defined(FOG_OS_WINDOWS)
  memset(&_winFindData, 0, sizeof(WIN32_FIND_DATAW));
#endif // FOG_OS_WINDOWS

  open(path);
}

DirIterator::~DirIterator()
{
  if (_handle) close();
}

#if defined(FOG_OS_WINDOWS)
err_t DirIterator::open(const String32& path)
{
  if (_handle) close();

  TemporaryString32<TemporaryLength> pathAbs;
  TemporaryString16<TemporaryLength> t;

  err_t err;

  if ((err = FileUtil::toAbsolutePath(pathAbs, String32(), path)) ||
      (err = t.set(pathAbs)) ||
      (err = t.append(StubAscii8("\\*"))) ||
      (err = t.slashesToWin()))
  {
    return err;
  }

  if ((_handle = (void*)FindFirstFileW(t.cStrW(), &_winFindData)) != (void*)INVALID_HANDLE_VALUE)
  {
    _path = pathAbs;
    _position = 0;
    _fileInEntry = true;

    return Error::Ok;
  }
  else 
  {
    _handle = NULL;
    return GetLastError();
  }
}

void DirIterator::close()
{
  if (!_handle) return;

  ::FindClose((HANDLE)(_handle));
  _handle = NULL;
  _position = -1;
  _fileInEntry = false;
  _path.clear();
  memset(&_winFindData, 0, sizeof(WIN32_FIND_DATAW));
}

bool DirIterator::read(Entry& to)
{
  if (!_handle) return false;

  if (!_fileInEntry)
  {
    // try to read next file entry
__readNext:
    if (!::FindNextFileW(_handle, &to._winFindData)) return false;
  }
  else 
  {
    memcpy(&to._winFindData, &_winFindData, sizeof(WIN32_FIND_DATAW));
    _fileInEntry = false;
  }

  _position++;

  // we have valid file entry in to._winFindData
  if (to._winFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
  {
    to._type = Entry::Directory;

    // Skip "." and ".."
    if (_skipDots)
    {
      const WCHAR* cfn = to._winFindData.cFileName;
      if (cfn[0] == TEXT('.') && 
        ((cfn[1] == 0) || (cfn[1] == TEXT('.') && cfn[2] == 0) ))
      {
        goto __readNext;
      }
    }
  }
  else
  {
    to._type = Entry::File;
  }

  to._size =
    ((uint64_t)(to._winFindData.nFileSizeHigh) << 32) |
    ((uint64_t)(to._winFindData.nFileSizeLow));
  to._name.set(StubUtf16(to._winFindData.cFileName));

  return true;
}

err_t DirIterator::rewind()
{
  if (!_handle) return Error::InvalidHandle;

  TemporaryString16<TemporaryLength> t;

  err_t err;

  if ((err = t.set(_path)) ||
      (err = t.append(StubAscii8("\\*"))) ||
      (err = t.slashesToWin()))
  {
    return err;
  }

  HANDLE h = FindFirstFileW(t.cStrW(), &_winFindData);
  if (h == INVALID_HANDLE_VALUE)
  {
    return GetLastError();
  }

  ::FindClose((HANDLE)_handle);
  _handle = (void*)h;
  _position = 0;
  _fileInEntry = true;

  return Error::Ok;
}

int64_t DirIterator::tell()
{
  return _position;
}
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
err_t DirIterator::open(const String32& path)
{
  close();

  TemporaryString32<TemporaryLength> pathAbs;
  TemporaryString8<TemporaryLength> t;

  FileSystem::toAbsolutePath(pathAbs, path);
  t.set(pathAbs, TextCodec::local8());

  if ((_handle = (void*)::opendir(t.cStr())) != NULL)
  {
    _path = pathAbs;
    _pathCache = t;
    return Error::Ok;
  }
  else
  {
    return errno;
  }
}

void DirIterator::close()
{
  if (!_handle) return;

  ::closedir((DIR*)(_handle));
  _handle = NULL;
  _path.clear();
}

bool DirIterator::read(Entry& to)
{
  if (!_handle) return false;

  struct dirent *de;
  while ((de = ::readdir((DIR*)(_handle))) != NULL)
  {
    const char* name = de->d_name;

    // skip "." and ".."
    if (name[0] == '.' && skipDots())
    {
      if (name[1] == '\0' || (name[1] == '.' && name[2] == '\0')) continue;
    }

    // get entry name length
    sysuint_t nameLength = strlen(name);

    // translate entry name to unicode
    to._name.set(name, nameLength, TextCodec::local8());

    // build path for stat() call and maximize speed of building string,
    // code below means something like:

    // _pathCache.append('/');
    // _pathCache.append(name, name_length);
    // _pathCache.xNullTerminate();

    char* namep = _pathCache._grow(nameLength + 1);
    *namep++ = '/';
    memcpy(namep, name, nameLength + 1); // copy with null terminator

    // We will not update _pathCache length, because we don't need it. It's
    // used just now to call stat() and every read call will rewrite it

    uint type = 0;

    if (::stat(_pathCache.cData(), &to._statInfo) != 0)
    {
      // This is situation that's bad symbolic link (I was experienced
      // this) and there is no reason to write an error message...
    }
    else
    {
      // S_ISXXX are posix macros to get easy file type...
      if (S_ISREG(to._statInfo.st_mode)) type |= Entry::File;
      if (S_ISDIR(to._statInfo.st_mode)) type |= Entry::Directory;
#if defined(S_ISCHR)
      if (S_ISCHR(to._statInfo.st_mode)) type |= Entry::CharacterDevice;
#endif
#if defined(S_ISBLK)
      if (S_ISBLK(to._statInfo.st_mode)) type |= Entry::BlockDevice;
#endif
#if defined(S_ISFIFO)
      if (S_ISFIFO(to._statInfo.st_mode)) type |= Entry::Fifo;
#endif
#if defined(S_ISLNK)
      if (S_ISLNK(to._statInfo.st_mode)) type |= Entry::Link;
#endif
#if defined(S_ISSOCK)
      if (S_ISSOCK(to._statInfo.st_mode)) type |= Entry::Socket;
#endif
    }

    to._type = type;
    if (type == Entry::File)
      to._size = to._statInfo.st_size;
    else
      to._size = 0;
    return true;
  }

  return false;
}

err_t DirIterator::rewind()
{
  if (!_handle) return Error::InvalidHandle;

  ::rewinddir((DIR*)(_handle));
  return Error::Ok;
}

int64_t DirIterator::tell()
{
  if (_handle)
    return (int64_t)::telldir((DIR*)(_handle));
  else
    return -1;
}
#endif // FOG_OS_POSIX

} // Fog namespace