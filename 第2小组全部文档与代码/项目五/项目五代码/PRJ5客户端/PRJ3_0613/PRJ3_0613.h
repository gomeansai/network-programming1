
// PRJ3_0613.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CPRJ3_0613App: 
// �йش����ʵ�֣������ PRJ3_0613.cpp
//

class CPRJ3_0613App : public CWinApp
{
public:
	CPRJ3_0613App();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CPRJ3_0613App theApp;