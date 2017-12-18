
// DigitalRecSystemDlg.cpp : ʵ���ļ�
//
 
#include "stdafx.h"
#include "DigitalRecSystem.h"
#include "DigitalRecSystemDlg.h"
#include "afxdialogex.h"
#include "Dib.h"
#include "AspriseOCR.h"
#pragma comment(lib, "AspriseOCR.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnSaveFile();
//	afx_msg void OnFillNumber();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_COMMAND(ID_SAVE_FILE, &CAboutDlg::OnSaveFile)
//	ON_COMMAND(ID_FILLNUMBER, &CAboutDlg::OnFillNumber)
END_MESSAGE_MAP()


// CDigitalRecSystemDlg �Ի���




CDigitalRecSystemDlg::CDigitalRecSystemDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDigitalRecSystemDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDigitalRecSystemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDigitalRecSystemDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_ABOUT, &CDigitalRecSystemDlg::OnAbout)
	ON_COMMAND(ID_EXIT, &CDigitalRecSystemDlg::OnExit)
	ON_COMMAND(ID_OPEN_FILE, &CDigitalRecSystemDlg::OnOpenFile)
	ON_COMMAND(ID_SAVE_FILE, &CDigitalRecSystemDlg::OnSaveFile)
	ON_COMMAND(ID_RELOAD, &CDigitalRecSystemDlg::OnReload)
	ON_COMMAND(ID_TOGRAY, &CDigitalRecSystemDlg::OnTogray)
	ON_COMMAND(ID_GRAYTOWB, &CDigitalRecSystemDlg::OnGrayToWhiteBlack)
	ON_COMMAND(ID_ImageSharp, &CDigitalRecSystemDlg::OnImageSharp)
	ON_COMMAND(ID_REMOVENOISE, &CDigitalRecSystemDlg::OnRemoveScatterNoise)
	ON_COMMAND(ID_FILLNUMBER, &CDigitalRecSystemDlg::OnFillnumber)
	ON_COMMAND(ID_ONEKEY, &CDigitalRecSystemDlg::OnOneKey)
	ON_COMMAND(ID_SLOPADJUST, &CDigitalRecSystemDlg::OnSlopeAdjust)
	ON_COMMAND(ID_NUMDIVIDE, &CDigitalRecSystemDlg::OnNumberDivide)
	ON_COMMAND(ID_TOSAME, &CDigitalRecSystemDlg::OnToSame)
	ON_COMMAND(ID_AUTOALIGN, &CDigitalRecSystemDlg::OnAutoAlign)
	ON_COMMAND(ID_OCRSTAR, &CDigitalRecSystemDlg::OnOcrStar)
	ON_BN_CLICKED(IDC_BUTTONOPEN, &CDigitalRecSystemDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTONSAVE, &CDigitalRecSystemDlg::OnBnClickedButtonSaveImage)
	ON_BN_CLICKED(IDC_BUTTONONEKEY, &CDigitalRecSystemDlg::OnBnClickedButtonOneKey)
	ON_BN_CLICKED(IDC_BUTTONOCR, &CDigitalRecSystemDlg::OnBnClickedButtonOCR)
	ON_BN_CLICKED(IDC_BUTTONRELOAD, &CDigitalRecSystemDlg::OnBnClickedButtonReload)
	ON_BN_CLICKED(IDC_BUTTONONEXIT, &CDigitalRecSystemDlg::OnBnClickedButtonOnExit)
END_MESSAGE_MAP()


// CDigitalRecSystemDlg ��Ϣ�������

BOOL CDigitalRecSystemDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	// ��ʼ������λ��
	this->MoveWindow(0,0,840,710,true);
	
	this->CenterWindow();

	//�Ƿ���ļ����Ƿ��һ��
	bOpen = false;
	m_dib.bToSame = false;

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CDigitalRecSystemDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CDigitalRecSystemDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CDigitalRecSystemDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDigitalRecSystemDlg::OnAbout()
{
	CAboutDlg DlgAbout;
	DlgAbout.DoModal();
}


void CDigitalRecSystemDlg::OnExit()
{
	exit(0);
}


void CDigitalRecSystemDlg::OnOpenFile()
{
	//����һ�����ļ��Ի��򣬲������������ļ�·��
	static TCHAR BASED_CODE szFilter[] = _T("24λλͼ�ļ�(*.bmp)|");
	CFileDialog dlg(TRUE,NULL,NULL,OFN_OVERWRITEPROMPT,szFilter,NULL);
    if(dlg.DoModal() == IDOK)
	{
	   strFilePath = dlg.GetPathName();
	}
	else return;

	//����λͼͼƬ���ж�
	if (!m_dib.LoadFromFile(strFilePath))
	{
		MessageBox(_T("����λͼ�ļ�"),_T("��ʾ"));
		return;
	}

	//�ж��Ƿ���24λλͼ�ļ�
	if (m_dib.GetBitCount() != 24)
	{
		MessageBox(_T("����24λλͼ�ļ�"),_T("��ʾ"));
		return;
	}

	//�Ѵ�
	bOpen = true;

	//�߽�
	CRect rect;
	GetClientRect(rect);

	//���õײ��߶�
	rect.bottom = rect.bottom * 3.0 / 4.0 - 23.0;

	//���Ʊ���
	CClientDC  dc(this);

	//���Ʊ���ɫ������֮ǰͼƬ
	CPen pen;
	pen.CreatePen (PS_SOLID,1,RGB(255,255,255));
	dc.SelectObject (&pen);
	dc.Rectangle (&rect);
	DeleteObject (pen);
	
	//��ʾλͼ
	m_dib.Draw(&dc, CPoint(0, 0), CSize(m_dib.GetWidth(), m_dib.GetHeight()));
}


void CDigitalRecSystemDlg::OnSaveFile()
{
	if (!bOpen)
	{
		MessageBox(_T("���ȴ�24λλͼ�ļ�"),_T("����"));
		return;
	}

	//����һ�����ļ��Ի��򣬲������������ļ�·��
	static TCHAR BASED_CODE szFilter[] = _T("24λλͼ�ļ�(*.bmp)|");
	CFileDialog dlg(FALSE,_T("bmp"),NULL,OFN_OVERWRITEPROMPT,szFilter,NULL);
    if(dlg.DoModal() == IDOK)
	{
	   strFilePath = dlg.GetPathName();
	}
	else return;

	//����λͼ�ļ�
	if (m_dib.SaveToFile(strFilePath))
	{
		MessageBox(_T("����λͼ�ļ��ɹ�"),_T("��ʾ"));
		return;
	}
}


void CDigitalRecSystemDlg::OnReload()
{
	if (!bOpen)
	{
		MessageBox(_T("���ȴ�λͼ�ļ�"),_T("����"));
		return;
	}

	//����λͼ
	m_dib.LoadFromFile(strFilePath);

	//�߽�
	CRect rect;
	GetClientRect(rect);

	//���Ʊ���
	CClientDC  dc(this);

	//���õײ��߶�
	rect.bottom = rect.bottom * 3.0 / 4.0 - 23.0;

	//���Ʊ���ɫ������֮ǰͼƬ
	CPen pen;
	pen.CreatePen (PS_SOLID,1,RGB(255,255,255));
	dc.SelectObject (&pen);
	dc.Rectangle (&rect);
	DeleteObject (pen);
	
	//��ʾλͼ
	m_dib.Draw(&dc, CPoint(0, 0), CSize(m_dib.GetWidth(), m_dib.GetHeight()));
}


void CDigitalRecSystemDlg::OnTogray()
{
	if (!bOpen)
	{
		MessageBox(_T("���ȴ�λͼ�ļ�"),_T("����"));
		return;
	}

	//���Ʊ���
	CClientDC  dc(this);

	//�ҶȻ�����
	if (!m_dib.RgbToGrade())
	{
		MessageBox(_T("�ҶȻ�ʧ��"), _T("����"));
		return;
	}

	m_dib.Draw(&dc, CPoint(0, 0), CSize(m_dib.GetWidth(), m_dib.GetHeight()));
}


void CDigitalRecSystemDlg::OnGrayToWhiteBlack()
{
	if (!bOpen)
	{
		MessageBox(_T("���ȴ�λͼ�ļ�"),_T("����"));
		return;
	}

	//���Ʊ���
	CClientDC  dc(this);

	//��ֵ������
	if (!m_dib.GrayToWhiteBlack())
	{
		MessageBox(_T("��ֵ��ʧ��"), _T("����"));
		return;
	}

	m_dib.Draw(&dc, CPoint(0, 0), CSize(m_dib.GetWidth(), m_dib.GetHeight()));
}


void CDigitalRecSystemDlg::OnImageSharp()
{	
	if (!bOpen)
	{
		MessageBox(_T("���ȴ�λͼ�ļ�"),_T("����"));
		return;
	}

	//���Ʊ���
	CClientDC  dc(this);

	//ͼ����
	if (!m_dib.ImageSharp())
	{
		MessageBox(_T("��ʧ��"), _T("����"));
		return;
	}

	m_dib.Draw(&dc, CPoint(0, 0), CSize(m_dib.GetWidth(), m_dib.GetHeight()));
}


void CDigitalRecSystemDlg::OnRemoveScatterNoise()
{
	if (!bOpen)
	{
		MessageBox(_T("���ȴ�λͼ�ļ�"),_T("����"));
		return;
	}

	//���Ʊ���
	CClientDC  dc(this);

	//ͼ��ȥ��
	if (!m_dib.RemoveScatterNoise())
	{
		MessageBox(_T("ȥ��ʧ��"), _T("����"));
		return;
	}

	m_dib.Draw(&dc, CPoint(0, 0), CSize(m_dib.GetWidth(), m_dib.GetHeight()));
}


void CDigitalRecSystemDlg::OnFillnumber()
{
	if (!bOpen)
	{
		MessageBox(_T("���ȴ�λͼ�ļ�"),_T("����"));
		return;
	}

	//���Ʊ���
	CClientDC  dc(this);

	//�������
	if (!m_dib.FillNumber())
	{
		MessageBox(_T("���ʧ��"), _T("����"));
		return;
	}
	else
	{
		for (int i = 0; i < 3; ++i)
			m_dib.FillNumber();
	}

	m_dib.Draw(&dc, CPoint(0, 0), CSize(m_dib.GetWidth(), m_dib.GetHeight()));
}


void CDigitalRecSystemDlg::OnSlopeAdjust()
{
	if (!bOpen)
	{
		MessageBox(_T("���ȴ�λͼ�ļ�"),_T("����"));
		return;
	}

	// ���Ʊ���
	CClientDC  dc(this);

	// б���Ż�
	if (!m_dib.SlopeAdjust())
	{
		MessageBox(_T("�Ż�ʧ��"), _T("����"));
		return;
	}

	m_dib.Draw(&dc, CPoint(0, 0), CSize(m_dib.GetWidth(), m_dib.GetHeight()));
}


void CDigitalRecSystemDlg::OnNumberDivide()
{
	if (!bOpen)
	{
		MessageBox(_T("���ȴ�λͼ�ļ�"),_T("����"));
		return;
	}

	// ���Ʊ���
	CClientDC  dc(this);

	// �ַ��и�
	if (!m_dib.NumberDivide())
	{
		MessageBox(_T("�и�ʧ��"), _T("����"));
		return;
	}

	m_dib.Draw(&dc, CPoint(0, 0), CSize(m_dib.GetWidth(), m_dib.GetHeight()));
	m_dib.DrawFrame(&dc, m_dib.m_charRect, 2, RGB(157,200,253));
}


void CDigitalRecSystemDlg::OnToSame()
{
	if (!bOpen)
	{
		MessageBox(_T("���ȴ�λͼ�ļ�"),_T("����"));
		return;
	}

	// ���Ʊ���
	CClientDC  dc(this);

	// �ַ���С��һ��
	if (!m_dib.ToSame(15, 30))
	{
		MessageBox(_T("��һ��ʧ��"), _T("����"));
		return;
	}

	m_dib.Draw(&dc, CPoint(0, 0), CSize(m_dib.GetWidth(), m_dib.GetHeight()));
	m_dib.DrawFrame(&dc, m_dib.m_charRect, 2, RGB(157,200,253));
}


void CDigitalRecSystemDlg::OnAutoAlign()
{
	if (!bOpen)
	{
		MessageBox(_T("���ȴ�λͼ�ļ�"),_T("����"));
		return;
	}

	// ���Ʊ���
	CClientDC  dc(this);

	// ��������
	if (!m_dib.AutoAlign())
	{
		MessageBox(_T("��������ʧ��"), _T("����"));
		return;
	}

	//�߽�
	CRect rect;
	GetClientRect(rect);

	//���õײ��߶�
	rect.bottom = rect.bottom * 3.0 / 4.0;

	//���Ʊ���ɫ������֮ǰͼƬ
	CPen pen;
	pen.CreatePen (PS_SOLID,1,RGB(255,255,255));
	dc.SelectObject (&pen);
	dc.Rectangle (&rect);
	DeleteObject (pen);

	m_dib.Draw(&dc, CPoint(0, 0), CSize(m_dib.GetWidth(), m_dib.GetHeight()));
	m_dib.DrawFrame(&dc, m_dib.m_charRect, 2, RGB(157,200,253));
}


void CDigitalRecSystemDlg::OnOneKey()
{
	if (!bOpen)
	{
		MessageBox(_T("���ȴ�λͼ�ļ�"),_T("����"));
		return;
	}

	// ���Ʊ���
	CClientDC  dc(this);

	// 1.�ҶȻ�����
	if (!m_dib.RgbToGrade())
	{
		MessageBox(_T("�ҶȻ�ʧ��"), _T("����"));
		return;
	}

	// 2.��ֵ������
	if (!m_dib.GrayToWhiteBlack())
	{
		MessageBox(_T("��ֵ��ʧ��"), _T("����"));
		return;
	}

	// 3.ͼ����
	if (!m_dib.ImageSharp())
	{
		MessageBox(_T("��ʧ��"), _T("����"));
		return;
	}

	// 4.ͼ��ȥ��
	if (!m_dib.RemoveScatterNoise())
	{
		MessageBox(_T("ȥ��ʧ��"), _T("����"));
		return;
	}

	// 5.�������
	if (!m_dib.FillNumber())
	{
		MessageBox(_T("���ʧ��"), _T("����"));
		return;
	}
	else
	{
		for (int i = 0; i < 3; ++i)
			m_dib.FillNumber();
	}

	// 6.б���Ż�
	if (!m_dib.SlopeAdjust())
	{
		MessageBox(_T("�Ż�ʧ��"), _T("����"));
		return;
	}

	// 7.�ַ��и�
	if (!m_dib.NumberDivide())
	{
		MessageBox(_T("�и�ʧ��"), _T("����"));
		return;
	}

	// 8.�ַ���С��һ��
	if (!m_dib.ToSame(15, 30))
	{
		MessageBox(_T("��һ��ʧ��"), _T("����"));
		return;
	}

	// 9.��������
	if (!m_dib.AutoAlign())
	{
		MessageBox(_T("��������ʧ��"), _T("����"));
		return;
	}

	//�߽�
	CRect rect;
	GetClientRect(rect);

	//���õײ��߶�
	rect.bottom = rect.bottom * 3.0 / 4.0 - 23.0;

	//���Ʊ���ɫ������֮ǰͼƬ
	CPen pen;
	pen.CreatePen (PS_SOLID,1,RGB(255,255,255));
	dc.SelectObject (&pen);
	dc.Rectangle (&rect);
	DeleteObject (pen);

	m_dib.Draw(&dc, CPoint(0, 0), CSize(m_dib.GetWidth(), m_dib.GetHeight()));
}


void CDigitalRecSystemDlg::OnOcrStar()
{
	if (!bOpen)
	{
		MessageBox(_T("���ȴ�λͼ�ļ�"),_T("����"));
		return;
	}

	//����λͼ�ļ�
	if (!m_dib.SaveToFile(_T("image.bmp")))
	{
		MessageBox(_T("����λͼ�ļ�ʧ��"),_T("��ʾ"));
		return;
	}

	//*********************************ty
	char *pathtxt ="image.bmp";//(LPSTR)(LPCTSTR)strFilePath;
    char *texttest = OCR(pathtxt,IMAGE_TYPE_AUTO_DETECT);
	CString texttxt(texttest);
    CDC* pDC=GetDC();
	CRect rct;
	rct.bottom = 100;
	rct.top = 0;
	rct.left = 0;
	rct.right = 100;
	
	try
	{
		CStdioFile filetxt;
		filetxt.Open(_T("ocrresult.txt"),CFile::modeCreate|CFile::modeWrite|CFile::typeText);
		filetxt.WriteString(texttxt);
		filetxt.Close();
	}
	catch(CFileException* e)
	{
		e->ReportError();
		e->Delete();
	}
	UpdateData(true);
	//��ʶ�������ֵ����������
	if (this->OpenClipboard())   //����ܴ򿪼�����
	{
		::EmptyClipboard();  //��ռ����壬ʹ�ô��ڳ�Ϊ�������ӵ����
		//���ı����õ�ϵͳ��������
		HGLOBAL hClip;
		hClip = ::GlobalAlloc(GMEM_MOVEABLE, (texttxt.GetLength() * 2) + 2); //�ж�Ҫ���ı����ݣ������ڴ�ʱ�����һ���ַ�
		TCHAR *pBuf;
		pBuf = (TCHAR *)::GlobalLock(hClip);//����������
		texttxt.Delete(texttxt.GetLength() - 2, 2);
		lstrcpy(pBuf, texttxt);//��CStringת��
		::GlobalUnlock(hClip);//�������������
		::SetClipboardData(CF_UNICODETEXT, hClip);//���ı����ݷ��͵�������  CF_UNICODETEXTΪUnicode����
		::CloseClipboard();//�رռ�����
	}
	UpdateData(false);
	MessageBox(_T("ʶ��������ǣ� ") + texttxt + _T("\nʶ�����Ѿ��ڼ�������,Ctrl+V����ʹ��"),_T("ʶ�����"),MB_OK);
	//**********************************ty
}


void CDigitalRecSystemDlg::OnBnClickedButtonOpen()
{
	CDigitalRecSystemDlg::OnOpenFile();
}


void CDigitalRecSystemDlg::OnBnClickedButtonSaveImage()
{
	CDigitalRecSystemDlg::OnSaveFile();
}


void CDigitalRecSystemDlg::OnBnClickedButtonOneKey()
{
	CDigitalRecSystemDlg::OnOneKey();
}


void CDigitalRecSystemDlg::OnBnClickedButtonOCR()
{
	CDigitalRecSystemDlg::OnOcrStar();
}


void CDigitalRecSystemDlg::OnBnClickedButtonReload()
{
	CDigitalRecSystemDlg::OnReload();
}


void CDigitalRecSystemDlg::OnBnClickedButtonOnExit()
{
	exit(0);
}
