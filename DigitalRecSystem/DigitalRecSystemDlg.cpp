
// DigitalRecSystemDlg.cpp : 实现文件
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


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CDigitalRecSystemDlg 对话框




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


// CDigitalRecSystemDlg 消息处理程序

BOOL CDigitalRecSystemDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	// 初始化窗口位置
	this->MoveWindow(0,0,840,710,true);
	
	this->CenterWindow();

	//是否打开文件、是否归一化
	bOpen = false;
	m_dib.bToSame = false;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDigitalRecSystemDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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
	//创建一个打开文件对话框，并返回完整的文件路径
	static TCHAR BASED_CODE szFilter[] = _T("24位位图文件(*.bmp)|");
	CFileDialog dlg(TRUE,NULL,NULL,OFN_OVERWRITEPROMPT,szFilter,NULL);
    if(dlg.DoModal() == IDOK)
	{
	   strFilePath = dlg.GetPathName();
	}
	else return;

	//加载位图图片并判断
	if (!m_dib.LoadFromFile(strFilePath))
	{
		MessageBox(_T("不是位图文件"),_T("提示"));
		return;
	}

	//判断是否是24位位图文件
	if (m_dib.GetBitCount() != 24)
	{
		MessageBox(_T("不是24位位图文件"),_T("提示"));
		return;
	}

	//已打开
	bOpen = true;

	//边界
	CRect rect;
	GetClientRect(rect);

	//设置底部高度
	rect.bottom = rect.bottom * 3.0 / 4.0 - 23.0;

	//绘制背景
	CClientDC  dc(this);

	//绘制背景色，覆盖之前图片
	CPen pen;
	pen.CreatePen (PS_SOLID,1,RGB(255,255,255));
	dc.SelectObject (&pen);
	dc.Rectangle (&rect);
	DeleteObject (pen);
	
	//显示位图
	m_dib.Draw(&dc, CPoint(0, 0), CSize(m_dib.GetWidth(), m_dib.GetHeight()));
}


void CDigitalRecSystemDlg::OnSaveFile()
{
	if (!bOpen)
	{
		MessageBox(_T("请先打开24位位图文件"),_T("错误"));
		return;
	}

	//创建一个打开文件对话框，并返回完整的文件路径
	static TCHAR BASED_CODE szFilter[] = _T("24位位图文件(*.bmp)|");
	CFileDialog dlg(FALSE,_T("bmp"),NULL,OFN_OVERWRITEPROMPT,szFilter,NULL);
    if(dlg.DoModal() == IDOK)
	{
	   strFilePath = dlg.GetPathName();
	}
	else return;

	//保存位图文件
	if (m_dib.SaveToFile(strFilePath))
	{
		MessageBox(_T("保存位图文件成功"),_T("提示"));
		return;
	}
}


void CDigitalRecSystemDlg::OnReload()
{
	if (!bOpen)
	{
		MessageBox(_T("请先打开位图文件"),_T("错误"));
		return;
	}

	//加载位图
	m_dib.LoadFromFile(strFilePath);

	//边界
	CRect rect;
	GetClientRect(rect);

	//绘制背景
	CClientDC  dc(this);

	//设置底部高度
	rect.bottom = rect.bottom * 3.0 / 4.0 - 23.0;

	//绘制背景色，覆盖之前图片
	CPen pen;
	pen.CreatePen (PS_SOLID,1,RGB(255,255,255));
	dc.SelectObject (&pen);
	dc.Rectangle (&rect);
	DeleteObject (pen);
	
	//显示位图
	m_dib.Draw(&dc, CPoint(0, 0), CSize(m_dib.GetWidth(), m_dib.GetHeight()));
}


void CDigitalRecSystemDlg::OnTogray()
{
	if (!bOpen)
	{
		MessageBox(_T("请先打开位图文件"),_T("错误"));
		return;
	}

	//绘制背景
	CClientDC  dc(this);

	//灰度化处理
	if (!m_dib.RgbToGrade())
	{
		MessageBox(_T("灰度化失败"), _T("错误"));
		return;
	}

	m_dib.Draw(&dc, CPoint(0, 0), CSize(m_dib.GetWidth(), m_dib.GetHeight()));
}


void CDigitalRecSystemDlg::OnGrayToWhiteBlack()
{
	if (!bOpen)
	{
		MessageBox(_T("请先打开位图文件"),_T("错误"));
		return;
	}

	//绘制背景
	CClientDC  dc(this);

	//二值化处理
	if (!m_dib.GrayToWhiteBlack())
	{
		MessageBox(_T("二值化失败"), _T("错误"));
		return;
	}

	m_dib.Draw(&dc, CPoint(0, 0), CSize(m_dib.GetWidth(), m_dib.GetHeight()));
}


void CDigitalRecSystemDlg::OnImageSharp()
{	
	if (!bOpen)
	{
		MessageBox(_T("请先打开位图文件"),_T("错误"));
		return;
	}

	//绘制背景
	CClientDC  dc(this);

	//图像锐化
	if (!m_dib.ImageSharp())
	{
		MessageBox(_T("锐化失败"), _T("错误"));
		return;
	}

	m_dib.Draw(&dc, CPoint(0, 0), CSize(m_dib.GetWidth(), m_dib.GetHeight()));
}


void CDigitalRecSystemDlg::OnRemoveScatterNoise()
{
	if (!bOpen)
	{
		MessageBox(_T("请先打开位图文件"),_T("错误"));
		return;
	}

	//绘制背景
	CClientDC  dc(this);

	//图像去噪
	if (!m_dib.RemoveScatterNoise())
	{
		MessageBox(_T("去噪失败"), _T("错误"));
		return;
	}

	m_dib.Draw(&dc, CPoint(0, 0), CSize(m_dib.GetWidth(), m_dib.GetHeight()));
}


void CDigitalRecSystemDlg::OnFillnumber()
{
	if (!bOpen)
	{
		MessageBox(_T("请先打开位图文件"),_T("错误"));
		return;
	}

	//绘制背景
	CClientDC  dc(this);

	//填充数字
	if (!m_dib.FillNumber())
	{
		MessageBox(_T("填充失败"), _T("错误"));
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
		MessageBox(_T("请先打开位图文件"),_T("错误"));
		return;
	}

	// 绘制背景
	CClientDC  dc(this);

	// 斜率优化
	if (!m_dib.SlopeAdjust())
	{
		MessageBox(_T("优化失败"), _T("错误"));
		return;
	}

	m_dib.Draw(&dc, CPoint(0, 0), CSize(m_dib.GetWidth(), m_dib.GetHeight()));
}


void CDigitalRecSystemDlg::OnNumberDivide()
{
	if (!bOpen)
	{
		MessageBox(_T("请先打开位图文件"),_T("错误"));
		return;
	}

	// 绘制背景
	CClientDC  dc(this);

	// 字符切割
	if (!m_dib.NumberDivide())
	{
		MessageBox(_T("切割失败"), _T("错误"));
		return;
	}

	m_dib.Draw(&dc, CPoint(0, 0), CSize(m_dib.GetWidth(), m_dib.GetHeight()));
	m_dib.DrawFrame(&dc, m_dib.m_charRect, 2, RGB(157,200,253));
}


void CDigitalRecSystemDlg::OnToSame()
{
	if (!bOpen)
	{
		MessageBox(_T("请先打开位图文件"),_T("错误"));
		return;
	}

	// 绘制背景
	CClientDC  dc(this);

	// 字符大小归一化
	if (!m_dib.ToSame(15, 30))
	{
		MessageBox(_T("归一化失败"), _T("错误"));
		return;
	}

	m_dib.Draw(&dc, CPoint(0, 0), CSize(m_dib.GetWidth(), m_dib.GetHeight()));
	m_dib.DrawFrame(&dc, m_dib.m_charRect, 2, RGB(157,200,253));
}


void CDigitalRecSystemDlg::OnAutoAlign()
{
	if (!bOpen)
	{
		MessageBox(_T("请先打开位图文件"),_T("错误"));
		return;
	}

	// 绘制背景
	CClientDC  dc(this);

	// 紧缩重排
	if (!m_dib.AutoAlign())
	{
		MessageBox(_T("紧缩重排失败"), _T("错误"));
		return;
	}

	//边界
	CRect rect;
	GetClientRect(rect);

	//设置底部高度
	rect.bottom = rect.bottom * 3.0 / 4.0;

	//绘制背景色，覆盖之前图片
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
		MessageBox(_T("请先打开位图文件"),_T("错误"));
		return;
	}

	// 绘制背景
	CClientDC  dc(this);

	// 1.灰度化处理
	if (!m_dib.RgbToGrade())
	{
		MessageBox(_T("灰度化失败"), _T("错误"));
		return;
	}

	// 2.二值化处理
	if (!m_dib.GrayToWhiteBlack())
	{
		MessageBox(_T("二值化失败"), _T("错误"));
		return;
	}

	// 3.图像锐化
	if (!m_dib.ImageSharp())
	{
		MessageBox(_T("锐化失败"), _T("错误"));
		return;
	}

	// 4.图像去噪
	if (!m_dib.RemoveScatterNoise())
	{
		MessageBox(_T("去噪失败"), _T("错误"));
		return;
	}

	// 5.填充数字
	if (!m_dib.FillNumber())
	{
		MessageBox(_T("填充失败"), _T("错误"));
		return;
	}
	else
	{
		for (int i = 0; i < 3; ++i)
			m_dib.FillNumber();
	}

	// 6.斜率优化
	if (!m_dib.SlopeAdjust())
	{
		MessageBox(_T("优化失败"), _T("错误"));
		return;
	}

	// 7.字符切割
	if (!m_dib.NumberDivide())
	{
		MessageBox(_T("切割失败"), _T("错误"));
		return;
	}

	// 8.字符大小归一化
	if (!m_dib.ToSame(15, 30))
	{
		MessageBox(_T("归一化失败"), _T("错误"));
		return;
	}

	// 9.紧缩重排
	if (!m_dib.AutoAlign())
	{
		MessageBox(_T("紧缩重排失败"), _T("错误"));
		return;
	}

	//边界
	CRect rect;
	GetClientRect(rect);

	//设置底部高度
	rect.bottom = rect.bottom * 3.0 / 4.0 - 23.0;

	//绘制背景色，覆盖之前图片
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
		MessageBox(_T("请先打开位图文件"),_T("错误"));
		return;
	}

	//保存位图文件
	if (!m_dib.SaveToFile(_T("image.bmp")))
	{
		MessageBox(_T("保存位图文件失败"),_T("提示"));
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
	//把识别后的数字导入剪贴板中
	if (this->OpenClipboard())   //如果能打开剪贴板
	{
		::EmptyClipboard();  //清空剪贴板，使该窗口成为剪贴板的拥有者
		//将文本放置到系统剪贴板中
		HGLOBAL hClip;
		hClip = ::GlobalAlloc(GMEM_MOVEABLE, (texttxt.GetLength() * 2) + 2); //判断要是文本数据，分配内存时多分配一个字符
		TCHAR *pBuf;
		pBuf = (TCHAR *)::GlobalLock(hClip);//锁定剪贴板
		texttxt.Delete(texttxt.GetLength() - 2, 2);
		lstrcpy(pBuf, texttxt);//把CString转换
		::GlobalUnlock(hClip);//解除锁定剪贴板
		::SetClipboardData(CF_UNICODETEXT, hClip);//把文本数据发送到剪贴板  CF_UNICODETEXT为Unicode编码
		::CloseClipboard();//关闭剪贴板
	}
	UpdateData(false);
	MessageBox(_T("识别的数字是： ") + texttxt + _T("\n识别结果已经在剪贴板中,Ctrl+V复制使用"),_T("识别完成"),MB_OK);
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
