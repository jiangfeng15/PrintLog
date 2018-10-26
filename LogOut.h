#pragma once
//文件最大4M  
static const int g_nMaxSize = 1024 * 1024 * 4;
class CLogOut
{
public:
	CLogOut();
	~CLogOut();
	bool WriteLog(const char *lpLog);
	bool WriteLog(const wchar_t *lpwLog);
	//显示在CEdit控件中
	bool WriteToEdit(CEdit *pEdit, const char *lpLog);
protected:
	bool InitLogFile();
	char * WcharToChar(const wchar_t * lpwSource);
public:
	wchar_t m_wcLogPath[MAX_PATH];
	int     m_nWriteSize;
	CRITICAL_SECTION m_criticalsec;

};

