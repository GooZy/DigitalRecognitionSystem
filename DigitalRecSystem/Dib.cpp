//======================================================================
// 文件： Dib.cpp
// 内容： 设备无关位图类-原文件
// 功能： （1）位图的加载与保存；
//        （2）位图信息的获取；
//        （3）位图数据的获取；
//        （3）位图的显示；
//        （4）位图的转换；
//        （5）位图相关判断；
// 作者： 李平科
// 更新： 郭子尧
// 联系： lipingke@126.com
// 日期： 2009-7-26
//======================================================================

#include "StdAfx.h"
#include "Dib.h"

//=======================================================
// 函数功能： 构造函数，初始化数据成员
// 输入参数： 无
// 返回值：   无
//=======================================================
CDib::CDib(void)
{
    // 数据成员初始化
	StrCpyW(m_fileName, _T(""));
    m_lpBmpFileHeader = NULL;
    m_lpDib = NULL;   
    m_lpBmpInfo = NULL;
    m_lpBmpInfoHeader = NULL;
    m_lpRgbQuad = NULL;
    m_lpData = NULL;
    m_hPalette = NULL;
    m_bHasRgbQuad = FALSE;
    m_bValid = FALSE;
}

//=======================================================
// 函数功能： 析构函数，释放内存空间
// 输入参数： 无
// 返回值：   无
//=======================================================
CDib::~CDib(void)
{
    // 清理空间
    Empty();
}

//=======================================================
// 函数功能： 从文件加载位图
// 输入参数： LPCTSTR lpszPath-待加载位图文件路径
// 返回值：   BOOL-TRUE 成功；FALSE 失败
//=======================================================
BOOL CDib::LoadFromFile(LPCTSTR lpszPath)
{
	bToSame = false;

    // 记录位图文件名
	StrCpyW(m_fileName, lpszPath);

    // 以读模式打开位图文件
    CFile dibFile;
    if(!dibFile.Open(m_fileName, CFile::modeRead | CFile::shareDenyWrite))
    {
        return FALSE;
    }

    // 清理空间
    Empty(); 
    
    // 为位图文件头分配空间，并初始化为0
    m_lpBmpFileHeader = (LPBITMAPFILEHEADER)new BYTE[sizeof(BITMAPFILEHEADER)];
    memset(m_lpBmpFileHeader, 0, sizeof(BITMAPFILEHEADER)); 

    // 读取位图文件头
    int nCount = dibFile.Read((void *)m_lpBmpFileHeader, sizeof(BITMAPFILEHEADER));
    if(nCount != sizeof(BITMAPFILEHEADER)) 
    {
        return FALSE;
    } 

    // 判断此文件是不是位图文件（“0x4d42”代表“BM”）
    if(m_lpBmpFileHeader->bfType == 0x4d42)
    {
        // 是位图文件

        // 计算除位图文件头的空间大小，分配空间并初始化为0
        DWORD dwDibSize = dibFile.GetLength() - sizeof(BITMAPFILEHEADER);
        m_lpDib = new BYTE[dwDibSize];
        memset(m_lpDib, 0, dwDibSize);

        // 读取除位图文件头的所有数据
        dibFile.Read(m_lpDib, dwDibSize);

        // 关闭位图文件
        dibFile.Close();

        // 设置位图信息指针
        m_lpBmpInfo = (LPBITMAPINFO)m_lpDib;

        // 设置位图信息头指针
        m_lpBmpInfoHeader = (LPBITMAPINFOHEADER)m_lpDib;

        // 设置位图颜色表指针
        m_lpRgbQuad = (LPRGBQUAD)(m_lpDib + m_lpBmpInfoHeader->biSize);

        // 如果位图没有设置位图使用的颜色数，设置它
        if(m_lpBmpInfoHeader->biClrUsed == 0)
        {
            m_lpBmpInfoHeader->biClrUsed = GetNumOfColor();
        }

        // 计算颜色表长度
        DWORD dwRgbQuadLength = CalcRgbQuadLength();

        // 设置位图数据指针
        m_lpData = m_lpDib + m_lpBmpInfoHeader->biSize + dwRgbQuadLength;

        // 判断是否有颜色表
        if(m_lpRgbQuad == (LPRGBQUAD)m_lpData)
        {
            m_lpRgbQuad = NULL;    // 将位图颜色表指针置空
            m_bHasRgbQuad = FALSE; // 无颜色表
        }
        else
        {
            m_bHasRgbQuad = TRUE;  // 有颜色表
            MakePalette();         // 根据颜色表生成调色板
        }        

        // 设置位图大小（因为很多位图文件都不设置此项）
        m_lpBmpInfoHeader->biSizeImage = GetSize();

        // 位图有效
        m_bValid = TRUE;

        return TRUE;
    }
    else
    {
        // 不是位图文件
        m_bValid = FALSE;

        return FALSE;
    }     

}

//=======================================================
// 函数功能： 将位图保存到文件
// 输入参数： LPCTSTR lpszPath-位图文件保存路径
// 返回值：   BOOL-TRUE 成功；FALSE 失败
//=======================================================
BOOL CDib::SaveToFile(LPCTSTR lpszPath)
{
    // 以写模式打开文件
	CFile dibFile;
	if(!dibFile.Open(lpszPath, CFile::modeCreate | CFile::modeReadWrite 
		| CFile::shareExclusive))
    {
        return FALSE;
    }

    // 记录位图文件名
	StrCpyW(m_fileName, lpszPath);

    // 将文件头结构写进文件
    dibFile.Write(m_lpBmpFileHeader, sizeof(BITMAPFILEHEADER));

    // 将文件信息头结构写进文件
    dibFile.Write(m_lpBmpInfoHeader, sizeof(BITMAPINFOHEADER));

    // 计算颜色表长度
    DWORD dwRgbQuadlength = CalcRgbQuadLength();

    // 如果有颜色表的话，将颜色表写进文件
    if(dwRgbQuadlength != 0)
    {
        dibFile.Write(m_lpRgbQuad, dwRgbQuadlength);
    }                                                        

    // 将位图数据写进文件
    DWORD dwDataSize = GetLineByte() * GetHeight();
    dibFile.Write(m_lpData, dwDataSize);

    // 关闭文件
    dibFile.Close();
		
    return TRUE;
}

//=======================================================
// 函数功能： 获取位图文件名
// 输入参数： 无
// 返回值：   LPCTSTR-位图文件名
//=======================================================
LPCTSTR CDib::GetFileName()
{
    return m_fileName;
}

//=======================================================
// 函数功能： 获取位图宽度
// 输入参数： 无
// 返回值：   LONG-位图宽度
//=======================================================
LONG CDib::GetWidth()
{
    return m_lpBmpInfoHeader->biWidth;
}

//=======================================================
// 函数功能： 获取位图高度
// 输入参数： 无
// 返回值：   LONG-位图高度
//=======================================================
LONG CDib::GetHeight()
{
    return m_lpBmpInfoHeader->biHeight;
}

//=======================================================
// 函数功能： 获取位图的宽度和高度
// 输入参数： 无
// 返回值：   CSize-位图的宽度和高度
//=======================================================
CSize CDib::GetDimension()
{
    return CSize(GetWidth(), GetHeight());
}

//=======================================================
// 函数功能： 获取位图大小
// 输入参数： 无
// 返回值：   DWORD-位图大小
//=======================================================
DWORD CDib::GetSize()
{
    if(m_lpBmpInfoHeader->biSizeImage != 0)
    {
        return m_lpBmpInfoHeader->biSizeImage;
    }
    else
    {       
        return GetWidth() * GetHeight();
    }
}

//=======================================================
// 函数功能： 获取单个像素所占位数
// 输入参数： 无
// 返回值：   WORD-单个像素所占位数
//=======================================================
WORD CDib::GetBitCount()
{
    return m_lpBmpInfoHeader->biBitCount;
}       

//=======================================================
// 函数功能： 获取每行像素所占字节数
// 输入参数： 无
// 返回值：   UINT-每行像素所占字节数
//=======================================================
UINT CDib::GetLineByte()
{ 
    return (GetWidth() * GetBitCount() / 8 + 3) / 4 * 4;;
}

//=======================================================
// 函数功能： 获取位图颜色数
// 输入参数： 无
// 返回值：   DWORD-位图颜色数
//=======================================================
DWORD CDib::GetNumOfColor()
{
    UINT dwNumOfColor;     

    if ((m_lpBmpInfoHeader->biClrUsed == 0) 
        && (m_lpBmpInfoHeader->biBitCount < 9))
	{
		switch (m_lpBmpInfoHeader->biBitCount)
		{
		    case 1: dwNumOfColor = 2; break;
		    case 4: dwNumOfColor = 16; break;
		    case 8: dwNumOfColor = 256;
		}
	}
    else
    {
        dwNumOfColor = m_lpBmpInfoHeader->biClrUsed;
    }  		

    return dwNumOfColor; 
}

//=======================================================
// 函数功能： 计算位图颜色表长度
// 输入参数： 无
// 返回值：   DWORD-位图颜色表长度
//=======================================================
DWORD CDib::CalcRgbQuadLength()
{
    DWORD dwNumOfColor = GetNumOfColor();
    if(dwNumOfColor > 256)
    {
        dwNumOfColor = 0;
    }
    return  dwNumOfColor * sizeof(RGBQUAD);
}

//=======================================================
// 函数功能： 获取位图颜色表
// 输入参数： 无
// 返回值：   LPRGBQUAD-位图颜色表指针
//=======================================================
LPRGBQUAD CDib::GetRgbQuad()
{
    return m_lpRgbQuad;
}

//=======================================================
// 函数功能： 获取位图数据
// 输入参数： 无
// 返回值：   LPBYTE-位图数据指针
//=======================================================
LPBYTE CDib::GetData()
{
    return m_lpData;
}

//=======================================================
// 函数功能： 根据颜色表生成调色板
// 输入参数： 无
// 返回值：   BOOL-TRUE 成功；FALSE 失败
//=======================================================
BOOL CDib::MakePalette()
{
    // 计算颜色表长度
    DWORD dwRgbQuadLength = CalcRgbQuadLength();

    // 如果颜色表长度为0，则不生成逻辑调色板
	if(dwRgbQuadLength == 0) 
    {
        return FALSE;
    }

	//删除旧的调色板对象
	if(m_hPalette != NULL) 
    {
        DeleteObject(m_hPalette);
        m_hPalette = NULL;
    }

	// 申请缓冲区，初始化为0
    DWORD dwNumOfColor = GetNumOfColor();
    DWORD dwSize = 2 * sizeof(WORD) + dwNumOfColor * sizeof(PALETTEENTRY);
	LPLOGPALETTE lpLogPalette = (LPLOGPALETTE) new BYTE[dwSize];
    memset(lpLogPalette, 0, dwSize);

    // 生成逻辑调色板
	lpLogPalette->palVersion = 0x300;
	lpLogPalette->palNumEntries = dwNumOfColor;
	LPRGBQUAD lpRgbQuad = (LPRGBQUAD) m_lpRgbQuad;
	for(int i = 0; i < dwNumOfColor; i++) 
    {
		lpLogPalette->palPalEntry[i].peRed = lpRgbQuad->rgbRed;
		lpLogPalette->palPalEntry[i].peGreen = lpRgbQuad->rgbGreen;
		lpLogPalette->palPalEntry[i].peBlue = lpRgbQuad->rgbBlue;
		lpLogPalette->palPalEntry[i].peFlags = 0;
		lpRgbQuad++;
	}

	// 创建逻辑调色板
	m_hPalette = CreatePalette(lpLogPalette);

	// 释放缓冲区
	delete [] lpLogPalette;

    return TRUE;
}

//=======================================================
// 函数功能： 显示位图
// 输入参数：
//            CDC *pDC-设备环境指针
//            CPoint origin-显示矩形区域的左上角
//            CSize size-显示矩形区域的尺寸
// 返回值：
//            BOOL-TRUE 成功；FALSE 失败
//=======================================================
BOOL CDib::Draw(CDC *pDC, CPoint origin, CSize size)
{
    // 位图无效，无法绘制，返回错误
    if(!IsValid())
    {
        return FALSE;
    }

    // 旧的调色板句柄
	HPALETTE hOldPalette = NULL;

	// 如果位图指针为空，则返回FALSE
	if(m_lpDib == NULL) 
    {
        return FALSE;
    }

	// 如果位图有调色板，则选进设备环境中
	if(m_hPalette != NULL) 
    {
		hOldPalette = SelectPalette(pDC->GetSafeHdc(), m_hPalette, TRUE);
	}

	// 设置位图伸缩模式
	pDC->SetStretchBltMode(COLORONCOLOR);

	// 将位图在pDC所指向的设备上进行显示
	StretchDIBits(pDC->GetSafeHdc(), origin.x, origin.y, size.cx, size.cy,
		0, 0, GetWidth(), GetHeight(), m_lpData, m_lpBmpInfo, DIB_RGB_COLORS, SRCCOPY);

	// 恢复旧的调色板
	if(hOldPalette != NULL)
    {
        SelectPalette(pDC->GetSafeHdc(), hOldPalette, TRUE);
    }

    return TRUE;
}

//=======================================================
// 函数功能： 24位彩色位图转8位灰度位图
// 输入参数： 无
// 返回值：   BOOL-TRUE 成功；FALSE 失败
//=======================================================
BOOL CDib::RgbToGrade()
{
    // 位图无效，失败返回
    if(!IsValid())
    {
        return FALSE;
    }

    // 不是24位位图，失败返回
    if(GetBitCount() != 24)
    {
        return FALSE;
    }

    // 是压缩位图，失败返回
    if(m_lpBmpInfoHeader->biCompression != BI_RGB)
    {
        return FALSE;
    }

    // 如果不是灰度位图，才需要转换
    if(!IsGrade())
    {
        // 获取原位图信息
        LONG lHeight = GetHeight();
        LONG lWidth = GetWidth();
        UINT uLineByte = GetLineByte();

        // 计算灰度位图数据所需空间
        UINT uGradeBmpLineByte = (lWidth + 3) / 4 * 4;
        DWORD dwGradeBmpDataSize = uGradeBmpLineByte * lHeight; 

        // 计算灰度位图所需空间
        DWORD dwGradeBmpSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256 + dwGradeBmpDataSize;

        // 设置灰度位图文件头
        LPBITMAPFILEHEADER lpGradeBmpFileHeader = (LPBITMAPFILEHEADER)new BYTE[sizeof(BITMAPFILEHEADER)];
        memset(lpGradeBmpFileHeader, 0, sizeof(BITMAPFILEHEADER));
        lpGradeBmpFileHeader->bfType = 0x4d42;
        lpGradeBmpFileHeader->bfSize = sizeof(BITMAPFILEHEADER) + dwGradeBmpSize;
        lpGradeBmpFileHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
                                          + sizeof(RGBQUAD) * 256;
        lpGradeBmpFileHeader->bfReserved1 = 0;
        lpGradeBmpFileHeader->bfReserved2 = 0;            

        // 为灰度位图分配空间，并初始化为0
        LPBYTE lpGradeBmp = (LPBYTE)new BYTE[dwGradeBmpSize];
        memset(lpGradeBmp, 0, dwGradeBmpSize);

        // 设置灰度位图信息头
        LPBITMAPINFOHEADER lpGradeBmpInfoHeader = (LPBITMAPINFOHEADER)(lpGradeBmp);
        lpGradeBmpInfoHeader->biBitCount = 8;
        lpGradeBmpInfoHeader->biClrImportant = 0;
        lpGradeBmpInfoHeader->biClrUsed = 256;
        lpGradeBmpInfoHeader->biCompression = BI_RGB;
        lpGradeBmpInfoHeader->biHeight = lHeight;
        lpGradeBmpInfoHeader->biPlanes = 1;
        lpGradeBmpInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
        lpGradeBmpInfoHeader->biSizeImage = dwGradeBmpDataSize;
        lpGradeBmpInfoHeader->biWidth = lWidth;
        lpGradeBmpInfoHeader->biXPelsPerMeter = m_lpBmpInfoHeader->biXPelsPerMeter;
        lpGradeBmpInfoHeader->biYPelsPerMeter = m_lpBmpInfoHeader->biYPelsPerMeter;

        // 设置灰度位图颜色表
        LPRGBQUAD lpGradeBmpRgbQuad = (LPRGBQUAD)(lpGradeBmp + sizeof(BITMAPINFOHEADER));

        // 初始化8位灰度图的调色板信息
        LPRGBQUAD lpRgbQuad;               
        for(int k = 0; k < 256; k++)
        {
            lpRgbQuad = (LPRGBQUAD)(lpGradeBmpRgbQuad + k);
            lpRgbQuad->rgbBlue = k; 
            lpRgbQuad->rgbGreen = k;
            lpRgbQuad->rgbRed = k;
            lpRgbQuad->rgbReserved = 0;
        }

        // 灰度位图数据处理
        BYTE r, g, b; 
        LPBYTE lpGradeBmpData = (LPBYTE)(lpGradeBmp + sizeof(BITMAPINFOHEADER) 
                                         + sizeof(RGBQUAD) * 256);
        // 进行颜色转换
        for(int i = 0; i < lHeight; i++)
        {
            for(int j = 0; j < lWidth; j++)
            {
                b = m_lpData[i * uLineByte + 3 * j];
                g = m_lpData[i * uLineByte + 3 * j + 1];
                r = m_lpData[i * uLineByte + 3 * j + 2];
                lpGradeBmpData[i * uGradeBmpLineByte + j] = (BYTE)(0.299 * r + 0.587 * g + 0.114 * b); 
            }
        }

        // 释放原有位图空间
        Empty(FALSE);

        // 重新设定原位图指针指向
        m_lpBmpFileHeader = lpGradeBmpFileHeader;
        m_lpDib = lpGradeBmp;
        m_lpBmpInfo = (LPBITMAPINFO)(lpGradeBmp);
        m_lpBmpInfoHeader = lpGradeBmpInfoHeader;
        m_lpRgbQuad = lpGradeBmpRgbQuad;
        m_lpData = lpGradeBmpData;

        // 设置颜色表标志
        m_bHasRgbQuad = TRUE;
        // 设置位图有效标志
        m_bValid = TRUE;
        // 生成调色板
        MakePalette();
    }

    return TRUE;   
}

//=======================================================
// 函数功能： 8位灰度位图二值化
// 输入参数： 无
// 返回值：   BOOL-TRUE 成功；FALSE 失败
//=======================================================
BOOL CDib::GrayToWhiteBlack()
{
	// 位图无效，失败返回
    if(!IsValid())
    {
        return FALSE;
    }

    // 是压缩位图，失败返回
    if(m_lpBmpInfoHeader->biCompression != BI_RGB)
    {
        return FALSE;
    }

    // 如果是灰度位图，才进行转换
    if(IsGrade())
    {
		 // 获取原位图信息
		 LONG lHeight = GetHeight();
		 LONG lWidth = GetWidth();
		 UINT uLineByte = GetLineByte();

		 // 计算灰度位图数据所需空间
		 UINT uGradeBmpLineByte = (lWidth + 3) / 4 * 4;
		 DWORD dwGradeBmpDataSize = uGradeBmpLineByte * lHeight; 

		 // 计算灰度位图所需空间
		 DWORD dwGradeBmpSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256 + dwGradeBmpDataSize;

		 // 设置灰度位图文件头
		 LPBITMAPFILEHEADER lpGradeBmpFileHeader = (LPBITMAPFILEHEADER)new BYTE[sizeof(BITMAPFILEHEADER)];
		 memset(lpGradeBmpFileHeader, 0, sizeof(BITMAPFILEHEADER));
		 lpGradeBmpFileHeader->bfType = 0x4d42;
		 lpGradeBmpFileHeader->bfSize = sizeof(BITMAPFILEHEADER) + dwGradeBmpSize;
		 lpGradeBmpFileHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
										   + sizeof(RGBQUAD) * 256;
		 lpGradeBmpFileHeader->bfReserved1 = 0;
		 lpGradeBmpFileHeader->bfReserved2 = 0;            

		 // 为灰度位图分配空间，并初始化为0
		 LPBYTE lpGradeBmp = (LPBYTE)new BYTE[dwGradeBmpSize];
		 memset(lpGradeBmp, 0, dwGradeBmpSize);

		 // 设置灰度位图信息头
		 LPBITMAPINFOHEADER lpGradeBmpInfoHeader = (LPBITMAPINFOHEADER)(lpGradeBmp);
		 lpGradeBmpInfoHeader->biBitCount = 8;
		 lpGradeBmpInfoHeader->biClrImportant = 0;
		 lpGradeBmpInfoHeader->biClrUsed = 256;
		 lpGradeBmpInfoHeader->biCompression = BI_RGB;
		 lpGradeBmpInfoHeader->biHeight = lHeight;
		 lpGradeBmpInfoHeader->biPlanes = 1;
		 lpGradeBmpInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
		 lpGradeBmpInfoHeader->biSizeImage = dwGradeBmpDataSize;
		 lpGradeBmpInfoHeader->biWidth = lWidth;
		 lpGradeBmpInfoHeader->biXPelsPerMeter = m_lpBmpInfoHeader->biXPelsPerMeter;
		 lpGradeBmpInfoHeader->biYPelsPerMeter = m_lpBmpInfoHeader->biYPelsPerMeter;

		 // 设置灰度位图颜色表
		 LPRGBQUAD lpGradeBmpRgbQuad = (LPRGBQUAD)(lpGradeBmp + sizeof(BITMAPINFOHEADER));

		 // 初始化8位灰度图的调色板信息
		 LPRGBQUAD lpRgbQuad;               
		 for(int k = 0; k < 256; k++)
		 {
			 lpRgbQuad = (LPRGBQUAD)(lpGradeBmpRgbQuad + k);
			 lpRgbQuad->rgbBlue = k; 
			 lpRgbQuad->rgbGreen = k;
			 lpRgbQuad->rgbRed = k;
			 lpRgbQuad->rgbReserved = 0;
		 }

		 // 灰度位图数据处理
		 BYTE wb; 
		 LPBYTE lpGradeBmpData = (LPBYTE)(lpGradeBmp + sizeof(BITMAPINFOHEADER) 
										  + sizeof(RGBQUAD) * 256);

		 // 选择设定颜色
		 int selectColor;

		 // 根据颜色数选择颜色
		 int black = 0, white = 0;
		 for(int i = 0; i < lHeight; i++)
		 {
			 for(int j = 0; j < lWidth; j++)
			 {
				wb = m_lpData[i * uLineByte + j];
				if(wb > 220) ++black;
				else ++white;
			 }
		 }

		 // 选择颜色
		 if (black > white) selectColor = 255;
		 else selectColor = 0;

		 // 进行颜色转换
		 for(int i = 0; i < lHeight; i++)
		 {
			 for(int j = 0; j < lWidth; j++)
			 {
				wb = m_lpData[i * uLineByte + j];
				if(wb > 220) wb = selectColor;
				else wb = 255 - selectColor;
				lpGradeBmpData[i * uGradeBmpLineByte + j] = (BYTE)wb; 
			 }
		 }

		 // 释放原有位图空间
		 Empty(FALSE);

		 // 重新设定原位图指针指向
		 m_lpBmpFileHeader = lpGradeBmpFileHeader;
		 m_lpDib = lpGradeBmp;
		 m_lpBmpInfo = (LPBITMAPINFO)(lpGradeBmp);
		 m_lpBmpInfoHeader = lpGradeBmpInfoHeader;
		 m_lpRgbQuad = lpGradeBmpRgbQuad;
		 m_lpData = lpGradeBmpData;

		 // 设置颜色表标志
		 m_bHasRgbQuad = TRUE;  
		 // 设置位图有效标志
		 m_bValid = TRUE;
		 // 生成调色板
		 MakePalette();
		 return TRUE;
	}
	return FALSE;
}

//=======================================================
// 函数功能： 图像锐化
// 输入参数： 无
// 返回值：   BOOL-TRUE 成功；FALSE 失败
//=======================================================
BOOL CDib::ImageSharp()
{
	// 位图无效，失败返回
    if(!IsValid())
    {
        return FALSE;
    }

    // 是压缩位图，失败返回
    if(m_lpBmpInfoHeader->biCompression != BI_RGB)
    {
        return FALSE;
    }

    // 如果是灰度位图，才进行转换
    if(IsGrade())
    {
		 // 获取原位图信息
		 LONG lHeight = GetHeight();
		 LONG lWidth = GetWidth();
		 UINT uLineByte = GetLineByte();

		 //中间变量
		 BYTE bTemp, bThre = 2;
		 BYTE *pSrc, *pSrc1, *pSrc2;

		// 进行颜色转换
		for(int i = 0; i < lHeight; i++)
		{
			 for(int j = 0; j < lWidth; j++)
			 {
				 // 指向DIB第i行，第j个象素的指针
				pSrc = &m_lpData[uLineByte * (lHeight - 1 - i) + j];
			
				// 指向DIB第i+1行，第j个象素的指针
				pSrc1 = &m_lpData[uLineByte * (lHeight - 2 - i) + j];
			
				// 指向DIB第i行，第j+1个象素的指针
				pSrc2 = &m_lpData[uLineByte * (lHeight - 1 - i) + j + 1];
			

				//计算梯度值
				bTemp = abs((*pSrc)-(*pSrc1)) + abs((*pSrc)-(*pSrc2));
			
				// 判断是否小于阈值
				if (bTemp < 255)
				{  

					// 判断是否大于阈值，对于小于情况，灰度值不变。
				    if (bTemp >= bThre)
				    {

						// 直接赋值为bTemp
						*pSrc = bTemp;

				    }

				}
			    else
				{
					// 直接赋值为255
					*pSrc = 255;
				}
			}
		}

		//最后还要处理一下图像中最下面那行
		for(int j = 0; j < lWidth; j++)
		{   
	
		    //将此位置的象素设置为255，即白点
			m_lpData[j] = 255;

		}
		return TRUE;
	}
	return FALSE;
}

//=======================================================
// 函数功能： 判断离散点
// 输入参数： 无
// 返回值：   BOOL-TRUE 属于离散点；FALSE 不属于离散点 
//=======================================================
BOOL CDib::DeleteScaterJudge(LPBYTE lpmark, int x, int y, CPoint mark[], int thres)
{
	// 获取原位图信息
    LONG lHeight = GetHeight();
    LONG lWidth = GetWidth();
    UINT uLineByte = GetLineByte();
	
	//如果连续长度满足要求，说明不是离散点，返回
	if(m_lConsec >= thres)
		return TRUE;
	
	//长度加一
	m_lConsec++;

	//设定访问标志
	lpmark[lWidth * y + x] = true;
	
	//保存访问点坐标
	mark[m_lConsec - 1].x = x;
	mark[m_lConsec - 1].y = y;

	//象素的灰度值
	LONG gray;
  
	//方向数组省时省力
	int dir[8][2] = {0, 1,  1, 0,  1, 1,  -1, -1,  -1, 1,  1, -1,  -1, 0,  0, -1};

    //如果连续长度满足要求，说明不是离散点，返回
	if(m_lConsec >= thres)
		return TRUE;
	else
	{	
		//如果是黑色点，则调用函数自身进行递归	
		for(int i = 0; i < 8; i++) 
		{
			int nx = x + dir[i][0], ny = y + dir[i][1];
			//传递灰度值
			gray = m_lpData[uLineByte * ny + nx];

			if(gray == 255) continue;
			//如果点在图像内、颜色为黑色并且没有被访问过
			if(ny >= 0 && lpmark[ny * lWidth + nx] == false)

			//进行递归处理		
			DeleteScaterJudge(lpmark, nx, ny, mark, thres);

			//如果连续长度满足要求，说明不是离散点，返回
			if(m_lConsec >= thres)
				return TRUE;
		}
	}
	//如果递归结束，返回false，说明是离散点
	return FALSE;
}

//=======================================================
// 函数功能： 图像去噪
// 输入参数： 无
// 返回值：   BOOL-TRUE 成功；FALSE 失败
//=======================================================
BOOL CDib::RemoveScatterNoise()
{
	//方向
	int dir[8][2] = {-1, -1, 0, 1, 1, 1, 1, 0, 1, -1, 0, -1, -1, -1, -1, 0};

	// 位图无效，失败返回
    if(!IsValid())
    {
        return FALSE;
    }

    // 是压缩位图，失败返回
    if(m_lpBmpInfoHeader->biCompression != BI_RGB)
    {
        return FALSE;
    }

    // 如果是灰度位图，才进行去噪
    if(IsGrade())
    {
        // 获取原位图信息
        LONG lHeight = GetHeight();
        LONG lWidth = GetWidth();
        UINT uLineByte = GetLineByte();

		// 设定阈值
		CONST LONG thres = 15;

		// 初始化连续数
		m_lConsec = 0;
 
		//开辟一块用来存放标志的内存数组
		LPBYTE lpmark = new BYTE[lHeight * lWidth];

		//开辟一块用来保存离散判定结果的内存数组
		bool *lpTemp = new bool[lHeight * lWidth];

		for (int i = 0; i < lHeight * lWidth; i++)
			//将所有的标志位设置为非
			lpmark[i] = false;

		//用来存放离散点的坐标的数组
		CPoint mark[40];
   	
		//扫描整个图像

		for(int i = 0; i < lHeight; i++)
		{  
			for(int j = 0; j < lWidth; j++)
			{	
				//先把标志位置false
				for(int k = 0; k < m_lConsec; k++)
					lpmark[mark[k].y * lWidth + mark[k].x] = false;

				//连续数置0
				m_lConsec = 0;

				//进行离散性判断
				lpTemp[i * lWidth + j] = DeleteScaterJudge(lpmark, j, i, mark, thres);
			}
		}

		//扫描整个图像，把离散点填充成白色
		for(int i = 0; i < lHeight; i++)
		{
			for(int j = 0; j < lWidth; j++)
			{       
				// 查看标志位,如果为非则将此点设为白点
				if(lpTemp[i * lWidth + j] == false)
				{	
					// 将第i行第j个象素设为白色
					m_lpData[uLineByte * i + j] = 255;
				}
			}
		}
		delete lpmark;
		delete[] lpTemp;
		return TRUE;
	}
	return FALSE;
}

//=======================================================
// 函数功能： 填充数字
// 输入参数： 无
// 返回值：   BOOL-TRUE 成功；FALSE 失败
//=======================================================
BOOL CDib::FillNumber()
{
	//方向
	int dir[8][2] = {-1, -1, 0, 1, 1, 1, 1, 0, 1, -1, 0, -1, -1, -1, -1, 0};

	// 位图无效，失败返回
    if(!IsValid())
    {
        return FALSE;
    }

    // 是压缩位图，失败返回
    if(m_lpBmpInfoHeader->biCompression != BI_RGB)
    {
        return FALSE;
    }

    // 如果是灰度位图，才进行修补
    if(IsGrade())
    {
		 // 获取原位图信息
		 LONG lHeight = GetHeight();
		 LONG lWidth = GetWidth();
		 UINT uLineByte = GetLineByte();

		 //记录需要填充的点
		 bool *needRepair = new bool[uLineByte * (lHeight + lWidth)];

		 for(int i = 0; i < lHeight; i++)
		{
			 for(int j = 0; j < lWidth; j++)
			 {
				 needRepair[i * uLineByte + j] = false; 
			 }
		 }

		// 进行填充处理
		for(int i = 0; i < lHeight; i++)
		{
			 for(int j = 0; j < lWidth; j++)
			 {
				 if (m_lpData[i * uLineByte + j] != 255) continue; 

				 int findBlack = 0; //统计周围黑点

				 //访问周围八个方向，统计黑点个数
				 for (int k = 0; k < 8; ++k)
				 {
					 int nx = i + dir[k][0], ny = j + dir[k][1];
					 if (0 <= nx && nx < lHeight && 0 <= ny && ny < lWidth)
					 {
						if (m_lpData[nx * uLineByte + ny] == 0) ++findBlack;
					 }
				 }

				 //如果找到多于4个黑点
				 if (findBlack > 4)
				 {
					 needRepair[i * uLineByte + j] = true;
				 }
			 }
		}

		//进行修补
		for(int i = 0; i < lHeight; i++)
		{
			 for(int j = 0; j < lWidth; j++)
			 {
				 if (needRepair[i * uLineByte + j] == true)
				 {
					 m_lpData[i * uLineByte + j] = 0;
				 }
			 }
		 }
		
		delete []needRepair;

		return TRUE;
	}
	return FALSE;
}

//=======================================================
// 函数功能： 斜率调整
// 输入参数： 无
// 返回值：   BOOL-TRUE 成功；FALSE 失败
//=======================================================
BOOL CDib::SlopeAdjust()
{
	// 位图无效，失败返回
    if(!IsValid())
    {
        return FALSE;
    }

    // 是压缩位图，失败返回
    if(m_lpBmpInfoHeader->biCompression != BI_RGB)
    {
        return FALSE;
    }

    // 如果是灰度位图，才进行修正
    if(IsGrade())
    {
		 // 获取原位图信息
		 LONG lHeight = GetHeight();
		 LONG lWidth = GetWidth();
		 UINT uLineByte = GetLineByte();

		 // 图像左右半边的平均高度
		 double dLeftAver = 0.0;
		 double dRightAver = 0.0;

		 // 图像的倾斜度
		 double dSlope;

		 //循环变量
		 LONG i, j;

		 // 统计循环变量
		 LONG lCounts = 0;

		 // 扫描左半边的图像，求黑色像素的平均高度
		 for (i = 0; i < lHeight; ++i)
		 {
			 for (j = 0; j < lWidth / 2; ++j)
			 {
				 // 如果是黑点
				 if (m_lpData[uLineByte *  i + j] == 0)
				 {
					 // 对其高度进行统计叠加
					 lCounts += lWidth / 2 - j;
					 dLeftAver += i * (lWidth / 2 - j);
				 }
			 }
		 }

		 // 计算平均高度
		 dLeftAver /= lCounts;

		 // 将统计循环变量重新赋值
		 lCounts =0;

		// 扫描右半边的图像，求黑色象素的平均高度
		for (i = 0; i < lHeight; i++)
		{
			for (j = lWidth / 2; j < lWidth; j++)
			{
				// 如果是黑点
				if (m_lpData[uLineByte *  i + j] == 0)
				{
					// 对其高度进行统计叠加
					lCounts += lWidth / 2 - j;
					dRightAver += i * (lWidth / 2 - j);
				}
			}
		}

		// 计算右半边的平均高度
		dRightAver /= lCounts;
	
		// 计算斜率
		dSlope = (dLeftAver - dRightAver) / (lWidth/2);

		// 计算灰度位图数据所需空间
		 UINT uGradeBmpLineByte = (lWidth + 3) / 4 * 4;
		 DWORD dwGradeBmpDataSize = uGradeBmpLineByte * lHeight; 

		 // 计算灰度位图所需空间
		 DWORD dwGradeBmpSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256 + dwGradeBmpDataSize;

		 // 设置灰度位图文件头
		 LPBITMAPFILEHEADER lpGradeBmpFileHeader = (LPBITMAPFILEHEADER)new BYTE[sizeof(BITMAPFILEHEADER)];
		 memset(lpGradeBmpFileHeader, 0, sizeof(BITMAPFILEHEADER));
		 lpGradeBmpFileHeader->bfType = 0x4d42;
		 lpGradeBmpFileHeader->bfSize = sizeof(BITMAPFILEHEADER) + dwGradeBmpSize;
		 lpGradeBmpFileHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
										   + sizeof(RGBQUAD) * 256;
		 lpGradeBmpFileHeader->bfReserved1 = 0;
		 lpGradeBmpFileHeader->bfReserved2 = 0;            

		 // 为灰度位图分配空间，并初始化为0
		 LPBYTE lpGradeBmp = (LPBYTE)new BYTE[dwGradeBmpSize];
		 memset(lpGradeBmp, 0, dwGradeBmpSize);

		 // 设置灰度位图信息头
		 LPBITMAPINFOHEADER lpGradeBmpInfoHeader = (LPBITMAPINFOHEADER)(lpGradeBmp);
		 lpGradeBmpInfoHeader->biBitCount = 8;
		 lpGradeBmpInfoHeader->biClrImportant = 0;
		 lpGradeBmpInfoHeader->biClrUsed = 256;
		 lpGradeBmpInfoHeader->biCompression = BI_RGB;
		 lpGradeBmpInfoHeader->biHeight = lHeight;
		 lpGradeBmpInfoHeader->biPlanes = 1;
		 lpGradeBmpInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
		 lpGradeBmpInfoHeader->biSizeImage = dwGradeBmpDataSize;
		 lpGradeBmpInfoHeader->biWidth = lWidth;
		 lpGradeBmpInfoHeader->biXPelsPerMeter = m_lpBmpInfoHeader->biXPelsPerMeter;
		 lpGradeBmpInfoHeader->biYPelsPerMeter = m_lpBmpInfoHeader->biYPelsPerMeter;

		 // 设置灰度位图颜色表
		 LPRGBQUAD lpGradeBmpRgbQuad = (LPRGBQUAD)(lpGradeBmp + sizeof(BITMAPINFOHEADER));

		 // 初始化8位灰度图的调色板信息
		 LPRGBQUAD lpRgbQuad;               
		 for(int k = 0; k < 256; k++)
		 {
			 lpRgbQuad = (LPRGBQUAD)(lpGradeBmpRgbQuad + k);
			 lpRgbQuad->rgbBlue = k; 
			 lpRgbQuad->rgbGreen = k;
			 lpRgbQuad->rgbRed = k;
			 lpRgbQuad->rgbReserved = 0;
		 }

		 // 灰度位图数据处理
		 LPBYTE lpGradeBmpData = (LPBYTE)(lpGradeBmp + sizeof(BITMAPINFOHEADER) 
										  + sizeof(RGBQUAD) * 256);
		 // 象素点的灰度值
		 int gray;
    
		 // 位置映射值
		 int i_src;

		 // 根据斜率，把当前新图像的点映射到源图像的点
		 for (i = 0; i < lHeight; i++)
		 {
   			for (j = 0; j < lWidth; j++)
			{
				i_src=int(i - (j - lWidth / 2) * dSlope);

				//如果点在图像外，象素置白色
				if (i_src <0 || i_src >=lHeight )
					gray = 255;
				else
				{	
					//否则到源图像中找点，取得象素值
					gray = m_lpData[uLineByte *  i_src + j];
				}
				//把新图像的点用得到的象素值填充
				lpGradeBmpData[uLineByte * i + j] = gray;
			}
		 }

		 // 释放原有位图空间
        Empty(FALSE);

        // 重新设定原位图指针指向
        m_lpBmpFileHeader = lpGradeBmpFileHeader;
        m_lpDib = lpGradeBmp;
        m_lpBmpInfo = (LPBITMAPINFO)(lpGradeBmp);
        m_lpBmpInfoHeader = lpGradeBmpInfoHeader;
        m_lpRgbQuad = lpGradeBmpRgbQuad;
        m_lpData = lpGradeBmpData;

        // 设置颜色表标志
        m_bHasRgbQuad = TRUE;

        // 设置位图有效标志
        m_bValid = TRUE;

        // 生成调色板
        MakePalette();

		return TRUE;
	}
	return FALSE;
}

//=======================================================
// 函数功能： 字符分割
// 输入参数： 无
// 返回值：   BOOL-TRUE 成功；FALSE 失败
//=======================================================
BOOL CDib::NumberDivide()
{
	// 位图无效，失败返回
    if(!IsValid())
    {
        return FALSE;
    }

    // 是压缩位图，失败返回
    if(m_lpBmpInfoHeader->biCompression != BI_RGB)
    {
        return FALSE;
    }

    // 如果是灰度位图，才进行分割
    if(IsGrade())
    {
		 //清空用来保存每个字符区域的链表
		CRectLink charRect1,charRect2;
		charRect1.clear();
		charRect2.clear();

		// 获取原位图信息
		 LONG lHeight = GetHeight();
		 LONG lWidth = GetWidth();
		 UINT uLineByte = GetLineByte();

		//定义上下边界两个变量
		int top, bottom;

		//设置循环变量
		int i,j;

		//用来统计图像中字符个数的计数器
		digitCount = 0;


		//从上往下扫描，找到上边界
		for (i = 0; i < lHeight; i++)
		{
  			for (j = 0; j < lWidth; j++)
			{
				//看是否为黑点
				if (m_lpData[uLineByte * i + j] == 0)
				{   
				   //若为黑点，把此点作为字符大致的最高点
					top = i;

					//对i强行赋值以中断循环
					i = lHeight;

					//跳出循环
					break;
				}
			}
		}


		//从下往上扫描，找下边界
		for (i = lHeight - 1; i >= 0; i--)
		{
			for (j = 0; j < lWidth; j++)
			{
				//判断是否为黑点
				if (m_lpData[uLineByte * i + j] == 0)
				{
					//若为黑点，把此点作为字符大致的最低点
					bottom = i;

					//对i强行赋值以中断循环
					i = -1;

					//跳出循环
					break;
				}
			}
	
		}

		//lab 用作是否进入一个字符分割的标志
		bool lab = false;

		//表明扫描一列中是否发现黑色点
		bool black = false;

		//存放位置信息的结构体
		CRect rect;

		//计数器置零
		digitCount=0;
   
		for (i = 0; i < lWidth; i++)
		{
			//开始扫描一列
			black = false;

			for (j = 0; j < lHeight; j++)
			{	
				//判断是否为黑点
				if (m_lpData[uLineByte * j + i] == 0)
				{
					//如果发现黑点，设置标志位
					black=true;

					//如果还没有进入一个字符的分割
					if(lab==false)
					{	
						//设置左侧边界
						rect.left = i;

						//字符分割开始
						lab = true;
					}

					//如果字符分割已经开始了就跳出循环
					else break;
				}		
			}

			//如果已经扫到了最右边那列，说明整副图像扫描完毕。退出
			if(i == (lWidth - 1))
			   break;

			//如果到此black仍为false，说明扫描了一列，都没有发现黑点。表明当前字符分割结束
			if( lab == true && black == false)
			{   
			   //将位置信息存入结构体中

			   //设置右边界
				rect.right =i;

				//设置上边界
				rect.top =top;

				//设置下边界
				rect.bottom =bottom;

				//将框外括一个象素，以免压到字符
				rect.InflateRect(1,1);

				//将这个结构体插入存放位置信息的链表1的后面
				charRect1.push_back(rect);

				//设置标志位，开始下一次的字符分割
				lab = false;

				//字符个数统计计数器加1
				digitCount++;
			}
		}

	   //再将矩形轮廓矩形的top和bottom精确化

		//将链表1赋值给链表2
		charRect2 = charRect1;

		//将链表2的内容清空
		charRect2.clear ();

		//建立一个新的存放位置信息的结构体
		CRect rectnew;

		//对于链表1从头至尾逐个进行扫描
		while(!charRect1.empty())
		{    
			//从链表1头上得到一个矩形
			rect = charRect1.front();

			//从链表1头上面删掉一个
			charRect1.pop_front();

			//计算更加精确的矩形区域

			//获得精确的左边界
			rectnew.left = rect.left - 1 ;

			//获得精确的右边界
			rectnew.right = rect.right + 1 ;

			//通过获得的精确左右边界对上下边境重新进行精确定位

			// 由下而上扫描计算上边界
			for(i = rect.top; i < rect.bottom; i++)
			{   
				for(j = rect.left; j < rect.right; j++)
				{
					//如果这个象素是黑点
					if (m_lpData[uLineByte * i + j] == 0)
					{	
						//设置上边界
						rectnew.top = i - 1;
					
						//对i进行强制定义以跳出循环
						i = rect.bottom;

						//跳出循环
						break;
					}	
				}
			}

			//由下而上扫描计算下边界
			for(i = rect.bottom - 1; i >= rect.top; i--)
			{
				for(j = rect.left; j < rect.right; j++)
				{
					//该点如果为黑点
					if (m_lpData[uLineByte * i + j] == 0)
					{	
						//设置下边界
						rectnew.bottom = i + 1;

						//对i进行强制定义以跳出循环
						i = -1;

						//跳出循环
						break;
					}	
				}
			}

			//将得到的新的准确的位置信息从后面插到链表2的尾上
			charRect2.push_back(rectnew);
		}

		//将链表2 传递给链表1
		charRect1 = charRect2;

		//将链表1返回
		m_charRect = charRect1;

		return TRUE;
	}
	return FALSE;
}

//=======================================================
// 函数功能： 绘制矩形框
// 输入参数： 无
// 返回值：   BOOL-TRUE 成功；FALSE 失败
//=======================================================
void CDib::DrawFrame(CDC* pDC, CRectLink tempRect, unsigned int linewidth,COLORREF color)
{
	CPen pen;
	pen.CreatePen (PS_SOLID,linewidth,color);
	pDC->SelectObject(&pen);
	::SelectObject(*pDC,GetStockObject(NULL_BRUSH));
	CRect rect,rect2;
	while(!tempRect.empty())
	{
		//从表头上得到一个矩形
		rect2 = rect = tempRect.front();
		//从链表头上面删掉一个
		tempRect.pop_front();
		//注意，这里原先的rect是相对于图像原点(左下角)的，
		//而在屏幕上绘图时，要转换以客户区为原点的坐标
		rect.top = GetHeight() - rect2.bottom;
		rect.bottom =GetHeight() - rect2.top;
		pDC->Rectangle (&rect);
	}
}

//=======================================================
// 函数功能： 归一化处理
// 输入参数： 无
// 返回值：   BOOL-TRUE 成功；FALSE 失败
//=======================================================
BOOL CDib::ToSame(double tarWidth, double tarHeight)
{
	// 位图无效，失败返回
    if(!IsValid())
    {
        return FALSE;
    }

    // 是压缩位图，失败返回
    if(m_lpBmpInfoHeader->biCompression != BI_RGB)
    {
        return FALSE;
    }

    // 如果是灰度位图，才进行归一化
    if(IsGrade())
    {
		//进行了归一化
		bToSame = true;

		// 获取原位图信息
		LONG lHeight = GetHeight();
		LONG lWidth = GetWidth();
		UINT uLineByte = GetLineByte();

		// 循环变量
		int	i, j;

		//宽度、高度方向上的缩放因子
		double wscale, hscale;

		// 计算灰度位图数据所需空间
		 UINT uGradeBmpLineByte = (lWidth + 3) / 4 * 4;
		 DWORD dwGradeBmpDataSize = uGradeBmpLineByte * lHeight; 

		 // 计算灰度位图所需空间
		 DWORD dwGradeBmpSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256 + dwGradeBmpDataSize;

		 // 设置灰度位图文件头
		 LPBITMAPFILEHEADER lpGradeBmpFileHeader = (LPBITMAPFILEHEADER)new BYTE[sizeof(BITMAPFILEHEADER)];
		 memset(lpGradeBmpFileHeader, 0, sizeof(BITMAPFILEHEADER));
		 lpGradeBmpFileHeader->bfType = 0x4d42;
		 lpGradeBmpFileHeader->bfSize = sizeof(BITMAPFILEHEADER) + dwGradeBmpSize;
		 lpGradeBmpFileHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
										   + sizeof(RGBQUAD) * 256;
		 lpGradeBmpFileHeader->bfReserved1 = 0;
		 lpGradeBmpFileHeader->bfReserved2 = 0;            

		 // 为灰度位图分配空间，并初始化为0
		 LPBYTE lpGradeBmp = (LPBYTE)new BYTE[dwGradeBmpSize];
		 memset(lpGradeBmp, 0, dwGradeBmpSize);

		 // 设置灰度位图信息头
		 LPBITMAPINFOHEADER lpGradeBmpInfoHeader = (LPBITMAPINFOHEADER)(lpGradeBmp);
		 lpGradeBmpInfoHeader->biBitCount = 8;
		 lpGradeBmpInfoHeader->biClrImportant = 0;
		 lpGradeBmpInfoHeader->biClrUsed = 256;
		 lpGradeBmpInfoHeader->biCompression = BI_RGB;
		 lpGradeBmpInfoHeader->biHeight = lHeight;
		 lpGradeBmpInfoHeader->biPlanes = 1;
		 lpGradeBmpInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
		 lpGradeBmpInfoHeader->biSizeImage = dwGradeBmpDataSize;
		 lpGradeBmpInfoHeader->biWidth = lWidth;
		 lpGradeBmpInfoHeader->biXPelsPerMeter = m_lpBmpInfoHeader->biXPelsPerMeter;
		 lpGradeBmpInfoHeader->biYPelsPerMeter = m_lpBmpInfoHeader->biYPelsPerMeter;

		 // 设置灰度位图颜色表
		 LPRGBQUAD lpGradeBmpRgbQuad = (LPRGBQUAD)(lpGradeBmp + sizeof(BITMAPINFOHEADER));

		 // 初始化8位灰度图的调色板信息
		 LPRGBQUAD lpRgbQuad;               
		 for(int k = 0; k < 256; k++)
		 {
			 lpRgbQuad = (LPRGBQUAD)(lpGradeBmpRgbQuad + k);
			 lpRgbQuad->rgbBlue = k; 
			 lpRgbQuad->rgbGreen = k;
			 lpRgbQuad->rgbRed = k;
			 lpRgbQuad->rgbReserved = 0;
		 }

		 // 灰度位图数据处理
		 LPBYTE lpGradeBmpData = (LPBYTE)(lpGradeBmp + sizeof(BITMAPINFOHEADER) 
										  + sizeof(RGBQUAD) * 256);

		 //初始化为白色
		 for(i = 0; i < lHeight; i++)
		{   
			for(j = 0; j < lWidth; j++)
			{
				lpGradeBmpData[uLineByte * i + j] = 255;	
			}
		}

		//进行映射操作的坐标变量
		int i_src,j_src;

		//存放字符位置信息的结构体
		CRect rect;
		CRect rectnew;

		//先清空一个新的矩形区域链表以便存储标准化后的矩形区域链表
		m_charRectCopy.clear();

		//从头到尾逐个扫描各个结点
		while(!m_charRect.empty())
		{
			//从表头上得到一个矩形
			rect = m_charRect.front();

			//从链表头上面删掉一个
			m_charRect.pop_front();

			if(rect.Height() < 3 || rect.Width() < 6) continue;

			//计算缩放因子

			//横坐标方向的缩放因子
			wscale = tarWidth / rect.Width ();

			//纵坐标方向的缩放因子
			hscale = tarHeight / rect.Height ();

			//计算标准化矩形

			//上边界
			rectnew.top = rect.top ;

			//下边界
			rectnew.bottom = rect.top + tarHeight;

			//左边界
			rectnew.left = rect.left ;

			//右边界
			rectnew.right = rectnew.left + tarWidth;

			//将原矩形框内的象素映射到新的矩形框内
			for(i = rectnew.top; i < rectnew.bottom; i++)
			{
				for(j = rectnew.left; j < rectnew.right; j++)
				{   

					//计算映射坐标
					i_src = rectnew.top + int((i - rectnew.top) / hscale);
					j_src = rectnew.left + int((j - rectnew.left) / wscale);

					//将相对应的象素点进行映射操作
					lpGradeBmpData[uLineByte * i + j] = m_lpData[uLineByte * i_src + j_src];
				}
			}
			//将标准化后的矩形区域插入新的链表
			m_charRectCopy.push_back(rectnew);
		}

		//存储标准化后新的rect区域
		m_charRect = m_charRectCopy;

		// 释放原有位图空间
        Empty(FALSE);

        // 重新设定原位图指针指向
        m_lpBmpFileHeader = lpGradeBmpFileHeader;
        m_lpDib = lpGradeBmp;
        m_lpBmpInfo = (LPBITMAPINFO)(lpGradeBmp);
        m_lpBmpInfoHeader = lpGradeBmpInfoHeader;
        m_lpRgbQuad = lpGradeBmpRgbQuad;
        m_lpData = lpGradeBmpData;

        // 设置颜色表标志
        m_bHasRgbQuad = TRUE;
        // 设置位图有效标志
        m_bValid = TRUE;
        // 生成调色板
        MakePalette();

		return TRUE;
	}
	return FALSE;
}

//=======================================================
// 函数功能： 紧缩重排
// 输入参数： 无
// 返回值：   BOOL-TRUE 成功；FALSE 失败
//=======================================================
BOOL CDib::AutoAlign()
{
	// 位图无效，失败返回
    if(!IsValid())
    {
        return FALSE;
    }

    // 是压缩位图，失败返回
    if(m_lpBmpInfoHeader->biCompression != BI_RGB)
    {
        return FALSE;
    }

    // 如果是灰度位图且进行了归一化，才进行紧缩
    if(IsGrade() && bToSame)
	{
		// 获取原位图信息
		LONG lHeight = GetHeight();
		LONG lWidth = GetWidth();
		UINT uLineByte = GetLineByte();	
	
		//获取标准化的宽度
		int w = m_charRect.front ().Width();

		//获取标准化的高度
		int h = m_charRect.front ().Height();
	
		// 获得矩形数量
		digitCount = m_charRect.size();
    
		// 计算灰度位图数据所需空间
		UINT uGradeBmpLineByte = (w * digitCount + 3) / 4 * 4;
		DWORD dwGradeBmpDataSize = uGradeBmpLineByte * h; 

		// 计算灰度位图所需空间
		DWORD dwGradeBmpSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256 + dwGradeBmpDataSize;

		// 设置灰度位图文件头
		LPBITMAPFILEHEADER lpGradeBmpFileHeader = (LPBITMAPFILEHEADER)new BYTE[sizeof(BITMAPFILEHEADER)];
		memset(lpGradeBmpFileHeader, 0, sizeof(BITMAPFILEHEADER));
		lpGradeBmpFileHeader->bfType = 0x4d42;
		lpGradeBmpFileHeader->bfSize = sizeof(BITMAPFILEHEADER) + dwGradeBmpSize;
		lpGradeBmpFileHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
											+ sizeof(RGBQUAD) * 256;
		lpGradeBmpFileHeader->bfReserved1 = 0;
		lpGradeBmpFileHeader->bfReserved2 = 0;            

		// 为灰度位图分配空间，并初始化为0
		LPBYTE lpGradeBmp = (LPBYTE)new BYTE[dwGradeBmpSize];
		memset(lpGradeBmp, 0, dwGradeBmpSize);

		// 设置灰度位图信息头
		LPBITMAPINFOHEADER lpGradeBmpInfoHeader = (LPBITMAPINFOHEADER)(lpGradeBmp);
		lpGradeBmpInfoHeader->biBitCount = 8;
		lpGradeBmpInfoHeader->biClrImportant = 0;
		lpGradeBmpInfoHeader->biClrUsed = 256;
		lpGradeBmpInfoHeader->biCompression = BI_RGB;
		lpGradeBmpInfoHeader->biHeight = h;
		lpGradeBmpInfoHeader->biPlanes = 1;
		lpGradeBmpInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
		lpGradeBmpInfoHeader->biSizeImage = dwGradeBmpDataSize;
		lpGradeBmpInfoHeader->biWidth = w * digitCount;
		lpGradeBmpInfoHeader->biXPelsPerMeter = m_lpBmpInfoHeader->biXPelsPerMeter;
		lpGradeBmpInfoHeader->biYPelsPerMeter = m_lpBmpInfoHeader->biYPelsPerMeter;

		// 设置灰度位图颜色表
		LPRGBQUAD lpGradeBmpRgbQuad = (LPRGBQUAD)(lpGradeBmp + sizeof(BITMAPINFOHEADER));

		// 初始化8位灰度图的调色板信息
		LPRGBQUAD lpRgbQuad;               
		for(int k = 0; k < 256; k++)
		{
			lpRgbQuad = (LPRGBQUAD)(lpGradeBmpRgbQuad + k);
			lpRgbQuad->rgbBlue = k; 
			lpRgbQuad->rgbGreen = k;
			lpRgbQuad->rgbRed = k;
			lpRgbQuad->rgbReserved = 0;
		}

		// 灰度位图数据处理
		LPBYTE lpGradeBmpData = (LPBYTE)(lpGradeBmp + sizeof(BITMAPINFOHEADER) 
											+ sizeof(RGBQUAD) * 256);

		int i_src, j_src;
		int cnt = 0;
		CRect rect, rectNew;
		m_charRectCopy.clear();
		while(m_charRect.size())
		{
			rect = m_charRect.front();
			m_charRect.pop_front();

			rectNew.left = cnt * w;
		
			rectNew.right = (cnt + 1) * w;

			rectNew.top = 0;

			rectNew.bottom = h;
		
			m_charRectCopy.push_back(rectNew);
	
			for(int i = 0; i < h; i++)
			{
				for(int j = cnt * w; j < (cnt + 1) * w; j++)
				{
					i_src = rect.top + i;
					j_src = rect.left + j - cnt * w;
					lpGradeBmpData[uGradeBmpLineByte * i + j] = m_lpData[uLineByte * i_src + j_src];
				}
			}
			cnt++;
		}
		m_charRect = m_charRectCopy;
		// 释放原有位图空间
		Empty(FALSE);

		// 重新设定原位图指针指向
		m_lpBmpFileHeader = lpGradeBmpFileHeader;
		m_lpDib = lpGradeBmp;
		m_lpBmpInfo = (LPBITMAPINFO)(lpGradeBmp);
		m_lpBmpInfoHeader = lpGradeBmpInfoHeader;
		m_lpRgbQuad = lpGradeBmpRgbQuad;
		m_lpData = lpGradeBmpData;

		// 设置颜色表标志
		m_bHasRgbQuad = TRUE;  
		// 设置位图有效标志
		m_bValid = TRUE;
		// 生成调色板
		MakePalette();
		return TRUE;
	}
	return FALSE;
}

//=======================================================
// 函数功能： 8位灰度位图转24位彩色位图
// 输入参数： 无
// 返回值：   BOOL-TRUE 成功；FALSE 失败
//=======================================================
BOOL CDib::GradeToRgb()
{
    // 位图无效，失败退出
    if(!IsValid())
    {
        return FALSE;
    }

    // 不是8位位图，失败退出
    if(GetBitCount() != 8)
    {
        return FALSE;
    }

    // 是压缩位图，失败返回
    if(m_lpBmpInfoHeader->biCompression != BI_RGB)
    {
        return FALSE;
    }

    // 是灰度图时，才需转换
    if(IsGrade())
    {
        // 获取原位图信息
        LONG lHeight = GetHeight();
        LONG lWidth = GetWidth();
        UINT uLineByte = GetLineByte();

        // 计算彩色位图数据所需空间
        UINT uColorBmpLineByte = (lWidth * 24 / 8 + 3) / 4 * 4;
        DWORD dwColorBmpDataSize = uColorBmpLineByte * lHeight; 

        // 计算彩色位图所需空间
        DWORD dwColorBmpSize = sizeof(BITMAPINFOHEADER) + dwColorBmpDataSize;

        // 设置彩色位图文件头
        LPBITMAPFILEHEADER lpColorBmpFileHeader = (LPBITMAPFILEHEADER)new BYTE[sizeof(BITMAPFILEHEADER)];
        memset(lpColorBmpFileHeader, 0, sizeof(BITMAPFILEHEADER));
        lpColorBmpFileHeader->bfType = 0x4d42;
        lpColorBmpFileHeader->bfSize = sizeof(BITMAPFILEHEADER) + dwColorBmpSize;
        lpColorBmpFileHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
        lpColorBmpFileHeader->bfReserved1 = 0;
        lpColorBmpFileHeader->bfReserved2 = 0;    

        // 为彩色位图分配空间，并初始化为0
        LPBYTE lpColorBmp = (LPBYTE)new BYTE[dwColorBmpSize];
        memset(lpColorBmp, 0, dwColorBmpSize);

        // 设置彩色位图信息头
        LPBITMAPINFOHEADER lpColorBmpInfoHeader = (LPBITMAPINFOHEADER)(lpColorBmp);
        lpColorBmpInfoHeader->biBitCount = 24;
        lpColorBmpInfoHeader->biClrImportant = 0;
        lpColorBmpInfoHeader->biClrUsed = 0;
        lpColorBmpInfoHeader->biCompression = BI_RGB;
        lpColorBmpInfoHeader->biHeight = lHeight;
        lpColorBmpInfoHeader->biPlanes = 1;
        lpColorBmpInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
        lpColorBmpInfoHeader->biSizeImage = dwColorBmpDataSize;
        lpColorBmpInfoHeader->biWidth = lWidth;
        lpColorBmpInfoHeader->biXPelsPerMeter = m_lpBmpInfoHeader->biXPelsPerMeter;
        lpColorBmpInfoHeader->biYPelsPerMeter = m_lpBmpInfoHeader->biYPelsPerMeter;

        // 彩色位图数据处理
        LPBYTE lpColorBmpData = (LPBYTE)(lpColorBmp + sizeof(BITMAPINFOHEADER));
        // 进行颜色转换
        for(int i = 0; i < lHeight; i++)
        {
            for(int j = 0; j < lWidth; j++)
            {
                BYTE btValue = m_lpData[i * uLineByte + j]; 
                lpColorBmpData[i * uColorBmpLineByte + 3 * j] = btValue;
                lpColorBmpData[i * uColorBmpLineByte + 3 * j + 1] = btValue;
                lpColorBmpData[i * uColorBmpLineByte + 3 * j + 2] = btValue;  
            }
        }

        // 释放原有位图空间
        Empty(FALSE);

        // 重新设定原位图指针指向
        m_lpBmpFileHeader = lpColorBmpFileHeader;
        m_lpDib = lpColorBmp;
        m_lpBmpInfo = (LPBITMAPINFO)(lpColorBmp);
        m_lpBmpInfoHeader = lpColorBmpInfoHeader;
        m_lpRgbQuad = NULL;
        m_lpData = lpColorBmpData;

        // 设置颜色表标志
        m_bHasRgbQuad = FALSE;  
        // 设置位图有效标志
        m_bValid = TRUE;
		return TRUE;
    }        

    return FALSE;   
}   
 
//=======================================================
// 函数功能： 判断是否含有颜色表
// 输入参数： 无
// 返回值：   判断结果：TRUE-含有颜色表；FALSE-不含颜色表
//=======================================================
BOOL CDib::HasRgbQuad()
{
    return m_bHasRgbQuad;
}

//=======================================================
// 函数功能： 判断是否是灰度图
// 输入参数： 无
// 返回值：   判断结果：TRUE-是灰度图；FALSE-是彩色图
//=======================================================
BOOL CDib::IsGrade()
{
    return (GetBitCount() < 9 && GetBitCount() > 0);
}

//=======================================================
// 函数功能： 判断位图是否有效
// 输入参数： 无
// 返回值：   判断结果：TRUE-位图有效；FALSE-位图无效
//=======================================================
BOOL CDib::IsValid()
{
    return m_bValid;
}

//=======================================================
// 函数功能： 清理空间
// 输入参数： BOOL bFlag-TRUE 全部清空；FALSE 部分清空
// 返回值：   无
//=======================================================
void CDib::Empty(BOOL bFlag)
{
    // 文件名清空
    if(bFlag)
    {
		StrCpyW(m_fileName, _T(""));
    }      

    // 释放位图文件头指针空间
    if(m_lpBmpFileHeader != NULL)
    {
        delete [] m_lpBmpFileHeader;
        m_lpBmpFileHeader = NULL;
    }    

    // 释放位图指针空间
    if(m_lpDib != NULL)
    {
        delete [] m_lpDib;
        m_lpDib = NULL;
        m_lpBmpInfo = NULL;
        m_lpBmpInfoHeader = NULL;
        m_lpRgbQuad = NULL;
        m_lpData = NULL;           
    }       

    // 释放调色板
    if(m_hPalette != NULL)
    {
        DeleteObject(m_hPalette);
        m_hPalette = NULL;
    }    

    // 设置不含颜色表
    m_bHasRgbQuad = FALSE;
    
    // 设置位图无效
    m_bValid = FALSE;

}  
