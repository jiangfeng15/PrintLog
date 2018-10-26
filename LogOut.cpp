#include "stdafx.h"
#include "LogOut.h"
#include <iostream>
using namespace std;

CLogOut::CLogOut()
{
	m_nWriteSize = 0;
	InitLogFile();
	InitializeCriticalSection(&m_criticalsec);
}


CLogOut::~CLogOut()
{
}
bool CLogOut::WriteToEdit(CEdit *pEdit, const char *lpLog)
{
	if (NULL == lpLog)
		return false;
	int nLen = strlen(lpLog);
	if (0 == nLen)
		return false;
	SYSTEMTIME st;
	::GetLocalTime(&st);
	char cTime[40] = { 0 };
	int nTime = 0;
	char *cEndLine = "\n";
	sprintf_s(cTime, "%d-%02d-%02d %02d:%02d:%02d: ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	nTime = strlen(cTime);

	char * cPrintLog = new char[nTime + nLen + 4];
	memset(cPrintLog, '\0', sizeof(char)*(nTime + nLen + 4));
	sprintf_s(cPrintLog, nTime + nLen + 4, "%s%s\r\n", cTime, lpLog);
	CString szPreStr(""), szNewStr("");
	int nCurLine = 0;
	pEdit->GetWindowText(szPreStr);
	szNewStr = szNewStr + cPrintLog + szPreStr;
	pEdit->SetWindowText(szNewStr);
	//nCurLine = pEdit->GetLineCount();
}

bool CLogOut::WriteLog(const wchar_t *lpwLog)
{
	char * pBuffer = WcharToChar(lpwLog);
	if (NULL == pBuffer)
		return false;
	bool bRet = WriteLog(pBuffer);
	delete pBuffer;
	return bRet;
}

bool CLogOut::WriteLog(const char *lpLog)
{
	if (NULL == lpLog)
		return false;
	int nLen = strlen(lpLog);
	if (0 == nLen)
		return false;
	EnterCriticalSection(&m_criticalsec);
	if (m_nWriteSize >= g_nMaxSize)
		InitLogFile();
	FILE * fp;
	_wfopen_s(&fp, m_wcLogPath, L"a+");
	char * cErrInfo = "打开日志文件失败！";
	if (NULL == fp)
	{
		fwrite(cErrInfo,1, strlen(cErrInfo), fp);
		fclose(fp);
	}
	SYSTEMTIME st;
	::GetLocalTime(&st);
	char cTime[40] = { 0 };
	char *cEndLine = "\n";
	sprintf_s(cTime, "%d-%02d-%02d %02d:%02d:%02d: ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	fwrite(cTime, 1, strlen(cTime), fp);
	fwrite(lpLog, 1, strlen(lpLog), fp);
	fwrite(cEndLine, 1, strlen(cEndLine), fp);
	m_nWriteSize = m_nWriteSize + nLen + strlen(cTime) + strlen(cEndLine);
	fclose(fp);
	LeaveCriticalSection(&m_criticalsec);
	return true;
}
char * CLogOut::WcharToChar(const wchar_t * lpwSource)
{
	if (NULL == lpwSource)
		return NULL;
	int nLen = wcslen(lpwSource);
	if (0 == nLen)
		return NULL;
	int nNeedSize = WideCharToMultiByte(CP_ACP, 0,lpwSource, nLen, NULL, 0, NULL, NULL);
	if (0 == nNeedSize)
		return NULL;
	char * pBuffer = new char[nNeedSize+1];
	if (NULL == pBuffer)
		return NULL;
	WideCharToMultiByte(CP_ACP, 0, lpwSource, nLen, pBuffer, nNeedSize, NULL, NULL);
	pBuffer[nNeedSize] = '\0';
	return pBuffer;
}
bool CLogOut::InitLogFile()
{
	wchar_t wcPath[MAX_PATH] = { '\0' };
	::GetModuleFileName(NULL, wcPath, MAX_PATH);
	for (int i = wcslen(wcPath) - 1; i >= 0; i--)
	{
		if (wcPath[i] == '\\')
		{
			break;
		}
		wcPath[i] = '\0';
	}
	wcscat_s(wcPath, L"Log");
	SHCreateDirectory(NULL, wcPath);
	SYSTEMTIME st;
	GetLocalTime(&st);
	swprintf_s(m_wcLogPath, _T("%s\\%d%02d%02d-%02d-%02d-%02d.log"), wcPath, st.wYear, st.wMonth, st.wDay, st.wHour,st.wMinute, st.wSecond);
	m_nWriteSize = 0;

	return true;
}