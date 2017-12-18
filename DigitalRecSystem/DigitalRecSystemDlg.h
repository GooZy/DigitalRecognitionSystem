
// DigitalRecSystemDlg.h : ͷ�ļ�
//
#include "Dib.h"
#include <iostream>
#pragma once


// CDigitalRecSystemDlg �Ի���
class CDigitalRecSystemDlg : public CDialogEx
{
// ����
public:
	CDigitalRecSystemDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_DIGITALRECSYSTEM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


private:
	CDC     m_dcMain;
	CDC     m_dcBk;
	CBitmap m_bmpMain;
	CBitmap m_bitmap;
	CBitmap m_bmpBk;
	CString strFilePath;
	CDib    m_dib;
	bool    bOpen;

public:
	CString m_display;

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnAbout();
	afx_msg void OnExit();
	afx_msg void OnOpenFile();
	afx_msg void OnSaveFile();
	afx_msg void OnReload();
	afx_msg void OnTogray();
	afx_msg void OnGrayToWhiteBlack();
	afx_msg void OnImageSharp();
	afx_msg void OnRemoveScatterNoise();
	afx_msg void OnFillnumber();
	afx_msg void OnOneKey();
	afx_msg void OnSlopeAdjust();
	afx_msg void OnNumberDivide();
	afx_msg void OnToSame();
	afx_msg void OnAutoAlign();
	afx_msg void OnOcrStar();
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonSaveImage();
	afx_msg void OnBnClickedButtonOneKey();
	afx_msg void OnBnClickedButtonOCR();
	afx_msg void OnBnClickedButtonReload();
	afx_msg void OnBnClickedButtonOnExit();
};
