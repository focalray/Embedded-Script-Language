
///******************* LEGACY CODE: DO NOT EDIT ********************///


/*
	DataStream, 2000-2012
	Jinhong Kim
	Redperf@gmail.com
*/


#include "DataStream.h"
#include <windows.h>
#include <stdio.h>



static DI16 __strlen(char *str)
{
	DI16 len = 0;
	while (*str++) len++;
	return len;
}


DataStream::DataStream()
{
	hFile			= NULL;
	err				= 0;
	file_size		= 0;
	buf_ptr			= NULL;
	buf_cur_ptr		= NULL;
	buf_last_ptr	= NULL;
	bDMA			= FALSE;

	bOpenToRead			= TRUE;
	bOpenToReadStream	= FALSE;
}

DataStream::DataStream(char *fn)
{
	hFile			= NULL;
	err				= 0;
	file_size		= 0;
	buf_ptr			= NULL;
	buf_cur_ptr		= NULL;
	buf_last_ptr	= NULL;
	bDMA			= FALSE;

	bOpenToRead			= TRUE;
	bOpenToReadStream	= FALSE;

	Init(fn);
}

DataStream::~DataStream()
{
	Close();
}

void DataStream::Close()
{
	if ( buf_ptr && !bDMA )
		//free(buf_ptr);
		VirtualFree(buf_ptr, 0, MEM_RELEASE);

	if (hFile)
		CloseHandle(hFile);

	hFile			= NULL;
	err				= 0;
	file_size		= 0;
	buf_ptr			= NULL;
	buf_cur_ptr		= NULL;
	buf_last_ptr	= NULL;
	bDMA			= FALSE;
	bOpenToRead		= TRUE;
}

void DataStream::Init(char *fname)
{
	DI16 len = __strlen(fname);
	CopyMemory((void*)file_name, (void*)fname, len+1); 
}

void DataStream::InitDirectMemoryAccess( char* buf, DWORD speclen )
{
	buf_ptr			= (DI8*)buf;
	buf_cur_ptr		= buf_ptr;
	bDMA			= TRUE;
	err				= 0;

	if ( 0 == speclen )
		buf_last_ptr	= 0;
	else
		buf_last_ptr	= buf_ptr + speclen;
}

DWORD DataStream::MemoryToFile( DWORD len )
{
	if ( NULL == bDMA || NULL == hFile )
		return -1;

	DWORD written;
	WriteFile( hFile, (void*)buf_ptr, len, &written, NULL);

	return written;
}

BOOL DataStream::OpenToReadStream()
{
	hFile = CreateFileA(file_name,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hFile = NULL;
		return FALSE;
	}

	file_size = GetFileSize(hFile, NULL);

	buf_ptr				= NULL;
	buf_cur_ptr			= NULL;
	buf_last_ptr		= NULL;

	bOpenToRead			= FALSE;
	bOpenToReadStream	= TRUE;

	return TRUE;
}

BOOL DataStream::OpenToRead()
{
	hFile = CreateFileA(file_name,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hFile = NULL;
		return FALSE;
	}

	file_size = GetFileSize(hFile, NULL);

	DWORD	Get_size;
//	buf_ptr = (DI8*)malloc(file_size);
	buf_ptr = (DI8*)VirtualAlloc(NULL, file_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	buf_cur_ptr = buf_ptr;
	buf_last_ptr = buf_ptr + file_size - 1;

	ReadFile(hFile, (void*)buf_ptr, file_size, &Get_size, NULL);

	CloseHandle(hFile);
	hFile = NULL;

	bOpenToRead		= TRUE;

	return TRUE;
}


BOOL DataStream::OpenToWrite()
{
	hFile = CreateFileA(file_name,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hFile = NULL;
		return FALSE;
	}

	bOpenToRead		= FALSE;

	return TRUE;
}


BOOL DataStream::CreateToWrite()
{
	hFile = CreateFileA(file_name,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hFile = NULL;
		return FALSE;
	}

	bOpenToRead		= FALSE;

	return TRUE;
}

void DataStream::Seek( int pos )
{
	if ( bOpenToRead )
	{
		buf_cur_ptr = buf_ptr + pos;
		return;
	}

	if (bDMA)
	{
		buf_cur_ptr = buf_ptr + pos;
	}
	else
	{
		SetFilePointer(hFile, pos, NULL, FILE_BEGIN);
	}
}

void DataStream::Skip( int pos )
{
	if ( bOpenToRead )
	{
		buf_cur_ptr += pos;
		return;
	}

	if (bDMA)
	{
		buf_cur_ptr += pos;
	}
	else
	{
		SetFilePointer(hFile, pos, NULL, FILE_CURRENT);
	}
}

void DataStream::SkipBack( int pos )
{
	if ( bOpenToRead )
	{
		buf_cur_ptr -= pos;
		return;
	}

	if (bDMA)
	{
		buf_cur_ptr -= pos;
	}
	else
	{
		SetFilePointer(hFile, -(LONG)pos, NULL, FILE_CURRENT);
	}
}

void DataStream::Current( int* pos )
{
	if ( bOpenToRead )
	{
		*pos = (int)(buf_cur_ptr - buf_ptr);
		return;
	}

	if (bDMA)
	{
		*pos = (int)(buf_cur_ptr - buf_ptr);
	}
	else
	{
		*pos = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
	}
}

void DataStream::Rewind()
{
	if ( bOpenToRead )
	{
		buf_cur_ptr = buf_ptr;
		return;
	}

	if (bDMA)
	{
		buf_cur_ptr = buf_ptr;
	}
	else
	{
		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	}
}

DI16 DataStream::Err()
{
	return err;
}

DI32 DataStream::GetSize()
{
	return file_size;
}

BOOL DataStream::IsEnd()
{
	if ( bOpenToReadStream )
	{
		return ERROR_HANDLE_EOF == GetLastError();
	}

	if ( bOpenToRead && buf_cur_ptr >= buf_last_ptr )
	{
		return TRUE;
	}

	if (buf_cur_ptr >= buf_last_ptr && bDMA)
	{
		return TRUE;
	}

	return FALSE;
}

DWORD DataStream::GetWrittenBytes()
{
	return (DWORD)(buf_cur_ptr - buf_ptr);
}

