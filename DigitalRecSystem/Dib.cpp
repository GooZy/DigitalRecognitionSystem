//======================================================================
// �ļ��� Dib.cpp
// ���ݣ� �豸�޹�λͼ��-ԭ�ļ�
// ���ܣ� ��1��λͼ�ļ����뱣�棻
//        ��2��λͼ��Ϣ�Ļ�ȡ��
//        ��3��λͼ���ݵĻ�ȡ��
//        ��3��λͼ����ʾ��
//        ��4��λͼ��ת����
//        ��5��λͼ����жϣ�
// ���ߣ� ��ƽ��
// ���£� ����Ң
// ��ϵ�� lipingke@126.com
// ���ڣ� 2009-7-26
//======================================================================

#include "StdAfx.h"
#include "Dib.h"

//=======================================================
// �������ܣ� ���캯������ʼ�����ݳ�Ա
// ��������� ��
// ����ֵ��   ��
//=======================================================
CDib::CDib(void)
{
    // ���ݳ�Ա��ʼ��
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
// �������ܣ� �����������ͷ��ڴ�ռ�
// ��������� ��
// ����ֵ��   ��
//=======================================================
CDib::~CDib(void)
{
    // ����ռ�
    Empty();
}

//=======================================================
// �������ܣ� ���ļ�����λͼ
// ��������� LPCTSTR lpszPath-������λͼ�ļ�·��
// ����ֵ��   BOOL-TRUE �ɹ���FALSE ʧ��
//=======================================================
BOOL CDib::LoadFromFile(LPCTSTR lpszPath)
{
	bToSame = false;

    // ��¼λͼ�ļ���
	StrCpyW(m_fileName, lpszPath);

    // �Զ�ģʽ��λͼ�ļ�
    CFile dibFile;
    if(!dibFile.Open(m_fileName, CFile::modeRead | CFile::shareDenyWrite))
    {
        return FALSE;
    }

    // ����ռ�
    Empty(); 
    
    // Ϊλͼ�ļ�ͷ����ռ䣬����ʼ��Ϊ0
    m_lpBmpFileHeader = (LPBITMAPFILEHEADER)new BYTE[sizeof(BITMAPFILEHEADER)];
    memset(m_lpBmpFileHeader, 0, sizeof(BITMAPFILEHEADER)); 

    // ��ȡλͼ�ļ�ͷ
    int nCount = dibFile.Read((void *)m_lpBmpFileHeader, sizeof(BITMAPFILEHEADER));
    if(nCount != sizeof(BITMAPFILEHEADER)) 
    {
        return FALSE;
    } 

    // �жϴ��ļ��ǲ���λͼ�ļ�����0x4d42������BM����
    if(m_lpBmpFileHeader->bfType == 0x4d42)
    {
        // ��λͼ�ļ�

        // �����λͼ�ļ�ͷ�Ŀռ��С������ռ䲢��ʼ��Ϊ0
        DWORD dwDibSize = dibFile.GetLength() - sizeof(BITMAPFILEHEADER);
        m_lpDib = new BYTE[dwDibSize];
        memset(m_lpDib, 0, dwDibSize);

        // ��ȡ��λͼ�ļ�ͷ����������
        dibFile.Read(m_lpDib, dwDibSize);

        // �ر�λͼ�ļ�
        dibFile.Close();

        // ����λͼ��Ϣָ��
        m_lpBmpInfo = (LPBITMAPINFO)m_lpDib;

        // ����λͼ��Ϣͷָ��
        m_lpBmpInfoHeader = (LPBITMAPINFOHEADER)m_lpDib;

        // ����λͼ��ɫ��ָ��
        m_lpRgbQuad = (LPRGBQUAD)(m_lpDib + m_lpBmpInfoHeader->biSize);

        // ���λͼû������λͼʹ�õ���ɫ����������
        if(m_lpBmpInfoHeader->biClrUsed == 0)
        {
            m_lpBmpInfoHeader->biClrUsed = GetNumOfColor();
        }

        // ������ɫ����
        DWORD dwRgbQuadLength = CalcRgbQuadLength();

        // ����λͼ����ָ��
        m_lpData = m_lpDib + m_lpBmpInfoHeader->biSize + dwRgbQuadLength;

        // �ж��Ƿ�����ɫ��
        if(m_lpRgbQuad == (LPRGBQUAD)m_lpData)
        {
            m_lpRgbQuad = NULL;    // ��λͼ��ɫ��ָ���ÿ�
            m_bHasRgbQuad = FALSE; // ����ɫ��
        }
        else
        {
            m_bHasRgbQuad = TRUE;  // ����ɫ��
            MakePalette();         // ������ɫ�����ɵ�ɫ��
        }        

        // ����λͼ��С����Ϊ�ܶ�λͼ�ļ��������ô��
        m_lpBmpInfoHeader->biSizeImage = GetSize();

        // λͼ��Ч
        m_bValid = TRUE;

        return TRUE;
    }
    else
    {
        // ����λͼ�ļ�
        m_bValid = FALSE;

        return FALSE;
    }     

}

//=======================================================
// �������ܣ� ��λͼ���浽�ļ�
// ��������� LPCTSTR lpszPath-λͼ�ļ�����·��
// ����ֵ��   BOOL-TRUE �ɹ���FALSE ʧ��
//=======================================================
BOOL CDib::SaveToFile(LPCTSTR lpszPath)
{
    // ��дģʽ���ļ�
	CFile dibFile;
	if(!dibFile.Open(lpszPath, CFile::modeCreate | CFile::modeReadWrite 
		| CFile::shareExclusive))
    {
        return FALSE;
    }

    // ��¼λͼ�ļ���
	StrCpyW(m_fileName, lpszPath);

    // ���ļ�ͷ�ṹд���ļ�
    dibFile.Write(m_lpBmpFileHeader, sizeof(BITMAPFILEHEADER));

    // ���ļ���Ϣͷ�ṹд���ļ�
    dibFile.Write(m_lpBmpInfoHeader, sizeof(BITMAPINFOHEADER));

    // ������ɫ����
    DWORD dwRgbQuadlength = CalcRgbQuadLength();

    // �������ɫ��Ļ�������ɫ��д���ļ�
    if(dwRgbQuadlength != 0)
    {
        dibFile.Write(m_lpRgbQuad, dwRgbQuadlength);
    }                                                        

    // ��λͼ����д���ļ�
    DWORD dwDataSize = GetLineByte() * GetHeight();
    dibFile.Write(m_lpData, dwDataSize);

    // �ر��ļ�
    dibFile.Close();
		
    return TRUE;
}

//=======================================================
// �������ܣ� ��ȡλͼ�ļ���
// ��������� ��
// ����ֵ��   LPCTSTR-λͼ�ļ���
//=======================================================
LPCTSTR CDib::GetFileName()
{
    return m_fileName;
}

//=======================================================
// �������ܣ� ��ȡλͼ���
// ��������� ��
// ����ֵ��   LONG-λͼ���
//=======================================================
LONG CDib::GetWidth()
{
    return m_lpBmpInfoHeader->biWidth;
}

//=======================================================
// �������ܣ� ��ȡλͼ�߶�
// ��������� ��
// ����ֵ��   LONG-λͼ�߶�
//=======================================================
LONG CDib::GetHeight()
{
    return m_lpBmpInfoHeader->biHeight;
}

//=======================================================
// �������ܣ� ��ȡλͼ�Ŀ�Ⱥ͸߶�
// ��������� ��
// ����ֵ��   CSize-λͼ�Ŀ�Ⱥ͸߶�
//=======================================================
CSize CDib::GetDimension()
{
    return CSize(GetWidth(), GetHeight());
}

//=======================================================
// �������ܣ� ��ȡλͼ��С
// ��������� ��
// ����ֵ��   DWORD-λͼ��С
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
// �������ܣ� ��ȡ����������ռλ��
// ��������� ��
// ����ֵ��   WORD-����������ռλ��
//=======================================================
WORD CDib::GetBitCount()
{
    return m_lpBmpInfoHeader->biBitCount;
}       

//=======================================================
// �������ܣ� ��ȡÿ��������ռ�ֽ���
// ��������� ��
// ����ֵ��   UINT-ÿ��������ռ�ֽ���
//=======================================================
UINT CDib::GetLineByte()
{ 
    return (GetWidth() * GetBitCount() / 8 + 3) / 4 * 4;;
}

//=======================================================
// �������ܣ� ��ȡλͼ��ɫ��
// ��������� ��
// ����ֵ��   DWORD-λͼ��ɫ��
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
// �������ܣ� ����λͼ��ɫ����
// ��������� ��
// ����ֵ��   DWORD-λͼ��ɫ����
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
// �������ܣ� ��ȡλͼ��ɫ��
// ��������� ��
// ����ֵ��   LPRGBQUAD-λͼ��ɫ��ָ��
//=======================================================
LPRGBQUAD CDib::GetRgbQuad()
{
    return m_lpRgbQuad;
}

//=======================================================
// �������ܣ� ��ȡλͼ����
// ��������� ��
// ����ֵ��   LPBYTE-λͼ����ָ��
//=======================================================
LPBYTE CDib::GetData()
{
    return m_lpData;
}

//=======================================================
// �������ܣ� ������ɫ�����ɵ�ɫ��
// ��������� ��
// ����ֵ��   BOOL-TRUE �ɹ���FALSE ʧ��
//=======================================================
BOOL CDib::MakePalette()
{
    // ������ɫ����
    DWORD dwRgbQuadLength = CalcRgbQuadLength();

    // �����ɫ����Ϊ0���������߼���ɫ��
	if(dwRgbQuadLength == 0) 
    {
        return FALSE;
    }

	//ɾ���ɵĵ�ɫ�����
	if(m_hPalette != NULL) 
    {
        DeleteObject(m_hPalette);
        m_hPalette = NULL;
    }

	// ���뻺��������ʼ��Ϊ0
    DWORD dwNumOfColor = GetNumOfColor();
    DWORD dwSize = 2 * sizeof(WORD) + dwNumOfColor * sizeof(PALETTEENTRY);
	LPLOGPALETTE lpLogPalette = (LPLOGPALETTE) new BYTE[dwSize];
    memset(lpLogPalette, 0, dwSize);

    // �����߼���ɫ��
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

	// �����߼���ɫ��
	m_hPalette = CreatePalette(lpLogPalette);

	// �ͷŻ�����
	delete [] lpLogPalette;

    return TRUE;
}

//=======================================================
// �������ܣ� ��ʾλͼ
// ���������
//            CDC *pDC-�豸����ָ��
//            CPoint origin-��ʾ������������Ͻ�
//            CSize size-��ʾ��������ĳߴ�
// ����ֵ��
//            BOOL-TRUE �ɹ���FALSE ʧ��
//=======================================================
BOOL CDib::Draw(CDC *pDC, CPoint origin, CSize size)
{
    // λͼ��Ч���޷����ƣ����ش���
    if(!IsValid())
    {
        return FALSE;
    }

    // �ɵĵ�ɫ����
	HPALETTE hOldPalette = NULL;

	// ���λͼָ��Ϊ�գ��򷵻�FALSE
	if(m_lpDib == NULL) 
    {
        return FALSE;
    }

	// ���λͼ�е�ɫ�壬��ѡ���豸������
	if(m_hPalette != NULL) 
    {
		hOldPalette = SelectPalette(pDC->GetSafeHdc(), m_hPalette, TRUE);
	}

	// ����λͼ����ģʽ
	pDC->SetStretchBltMode(COLORONCOLOR);

	// ��λͼ��pDC��ָ����豸�Ͻ�����ʾ
	StretchDIBits(pDC->GetSafeHdc(), origin.x, origin.y, size.cx, size.cy,
		0, 0, GetWidth(), GetHeight(), m_lpData, m_lpBmpInfo, DIB_RGB_COLORS, SRCCOPY);

	// �ָ��ɵĵ�ɫ��
	if(hOldPalette != NULL)
    {
        SelectPalette(pDC->GetSafeHdc(), hOldPalette, TRUE);
    }

    return TRUE;
}

//=======================================================
// �������ܣ� 24λ��ɫλͼת8λ�Ҷ�λͼ
// ��������� ��
// ����ֵ��   BOOL-TRUE �ɹ���FALSE ʧ��
//=======================================================
BOOL CDib::RgbToGrade()
{
    // λͼ��Ч��ʧ�ܷ���
    if(!IsValid())
    {
        return FALSE;
    }

    // ����24λλͼ��ʧ�ܷ���
    if(GetBitCount() != 24)
    {
        return FALSE;
    }

    // ��ѹ��λͼ��ʧ�ܷ���
    if(m_lpBmpInfoHeader->biCompression != BI_RGB)
    {
        return FALSE;
    }

    // ������ǻҶ�λͼ������Ҫת��
    if(!IsGrade())
    {
        // ��ȡԭλͼ��Ϣ
        LONG lHeight = GetHeight();
        LONG lWidth = GetWidth();
        UINT uLineByte = GetLineByte();

        // ����Ҷ�λͼ��������ռ�
        UINT uGradeBmpLineByte = (lWidth + 3) / 4 * 4;
        DWORD dwGradeBmpDataSize = uGradeBmpLineByte * lHeight; 

        // ����Ҷ�λͼ����ռ�
        DWORD dwGradeBmpSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256 + dwGradeBmpDataSize;

        // ���ûҶ�λͼ�ļ�ͷ
        LPBITMAPFILEHEADER lpGradeBmpFileHeader = (LPBITMAPFILEHEADER)new BYTE[sizeof(BITMAPFILEHEADER)];
        memset(lpGradeBmpFileHeader, 0, sizeof(BITMAPFILEHEADER));
        lpGradeBmpFileHeader->bfType = 0x4d42;
        lpGradeBmpFileHeader->bfSize = sizeof(BITMAPFILEHEADER) + dwGradeBmpSize;
        lpGradeBmpFileHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
                                          + sizeof(RGBQUAD) * 256;
        lpGradeBmpFileHeader->bfReserved1 = 0;
        lpGradeBmpFileHeader->bfReserved2 = 0;            

        // Ϊ�Ҷ�λͼ����ռ䣬����ʼ��Ϊ0
        LPBYTE lpGradeBmp = (LPBYTE)new BYTE[dwGradeBmpSize];
        memset(lpGradeBmp, 0, dwGradeBmpSize);

        // ���ûҶ�λͼ��Ϣͷ
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

        // ���ûҶ�λͼ��ɫ��
        LPRGBQUAD lpGradeBmpRgbQuad = (LPRGBQUAD)(lpGradeBmp + sizeof(BITMAPINFOHEADER));

        // ��ʼ��8λ�Ҷ�ͼ�ĵ�ɫ����Ϣ
        LPRGBQUAD lpRgbQuad;               
        for(int k = 0; k < 256; k++)
        {
            lpRgbQuad = (LPRGBQUAD)(lpGradeBmpRgbQuad + k);
            lpRgbQuad->rgbBlue = k; 
            lpRgbQuad->rgbGreen = k;
            lpRgbQuad->rgbRed = k;
            lpRgbQuad->rgbReserved = 0;
        }

        // �Ҷ�λͼ���ݴ���
        BYTE r, g, b; 
        LPBYTE lpGradeBmpData = (LPBYTE)(lpGradeBmp + sizeof(BITMAPINFOHEADER) 
                                         + sizeof(RGBQUAD) * 256);
        // ������ɫת��
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

        // �ͷ�ԭ��λͼ�ռ�
        Empty(FALSE);

        // �����趨ԭλͼָ��ָ��
        m_lpBmpFileHeader = lpGradeBmpFileHeader;
        m_lpDib = lpGradeBmp;
        m_lpBmpInfo = (LPBITMAPINFO)(lpGradeBmp);
        m_lpBmpInfoHeader = lpGradeBmpInfoHeader;
        m_lpRgbQuad = lpGradeBmpRgbQuad;
        m_lpData = lpGradeBmpData;

        // ������ɫ���־
        m_bHasRgbQuad = TRUE;
        // ����λͼ��Ч��־
        m_bValid = TRUE;
        // ���ɵ�ɫ��
        MakePalette();
    }

    return TRUE;   
}

//=======================================================
// �������ܣ� 8λ�Ҷ�λͼ��ֵ��
// ��������� ��
// ����ֵ��   BOOL-TRUE �ɹ���FALSE ʧ��
//=======================================================
BOOL CDib::GrayToWhiteBlack()
{
	// λͼ��Ч��ʧ�ܷ���
    if(!IsValid())
    {
        return FALSE;
    }

    // ��ѹ��λͼ��ʧ�ܷ���
    if(m_lpBmpInfoHeader->biCompression != BI_RGB)
    {
        return FALSE;
    }

    // ����ǻҶ�λͼ���Ž���ת��
    if(IsGrade())
    {
		 // ��ȡԭλͼ��Ϣ
		 LONG lHeight = GetHeight();
		 LONG lWidth = GetWidth();
		 UINT uLineByte = GetLineByte();

		 // ����Ҷ�λͼ��������ռ�
		 UINT uGradeBmpLineByte = (lWidth + 3) / 4 * 4;
		 DWORD dwGradeBmpDataSize = uGradeBmpLineByte * lHeight; 

		 // ����Ҷ�λͼ����ռ�
		 DWORD dwGradeBmpSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256 + dwGradeBmpDataSize;

		 // ���ûҶ�λͼ�ļ�ͷ
		 LPBITMAPFILEHEADER lpGradeBmpFileHeader = (LPBITMAPFILEHEADER)new BYTE[sizeof(BITMAPFILEHEADER)];
		 memset(lpGradeBmpFileHeader, 0, sizeof(BITMAPFILEHEADER));
		 lpGradeBmpFileHeader->bfType = 0x4d42;
		 lpGradeBmpFileHeader->bfSize = sizeof(BITMAPFILEHEADER) + dwGradeBmpSize;
		 lpGradeBmpFileHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
										   + sizeof(RGBQUAD) * 256;
		 lpGradeBmpFileHeader->bfReserved1 = 0;
		 lpGradeBmpFileHeader->bfReserved2 = 0;            

		 // Ϊ�Ҷ�λͼ����ռ䣬����ʼ��Ϊ0
		 LPBYTE lpGradeBmp = (LPBYTE)new BYTE[dwGradeBmpSize];
		 memset(lpGradeBmp, 0, dwGradeBmpSize);

		 // ���ûҶ�λͼ��Ϣͷ
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

		 // ���ûҶ�λͼ��ɫ��
		 LPRGBQUAD lpGradeBmpRgbQuad = (LPRGBQUAD)(lpGradeBmp + sizeof(BITMAPINFOHEADER));

		 // ��ʼ��8λ�Ҷ�ͼ�ĵ�ɫ����Ϣ
		 LPRGBQUAD lpRgbQuad;               
		 for(int k = 0; k < 256; k++)
		 {
			 lpRgbQuad = (LPRGBQUAD)(lpGradeBmpRgbQuad + k);
			 lpRgbQuad->rgbBlue = k; 
			 lpRgbQuad->rgbGreen = k;
			 lpRgbQuad->rgbRed = k;
			 lpRgbQuad->rgbReserved = 0;
		 }

		 // �Ҷ�λͼ���ݴ���
		 BYTE wb; 
		 LPBYTE lpGradeBmpData = (LPBYTE)(lpGradeBmp + sizeof(BITMAPINFOHEADER) 
										  + sizeof(RGBQUAD) * 256);

		 // ѡ���趨��ɫ
		 int selectColor;

		 // ������ɫ��ѡ����ɫ
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

		 // ѡ����ɫ
		 if (black > white) selectColor = 255;
		 else selectColor = 0;

		 // ������ɫת��
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

		 // �ͷ�ԭ��λͼ�ռ�
		 Empty(FALSE);

		 // �����趨ԭλͼָ��ָ��
		 m_lpBmpFileHeader = lpGradeBmpFileHeader;
		 m_lpDib = lpGradeBmp;
		 m_lpBmpInfo = (LPBITMAPINFO)(lpGradeBmp);
		 m_lpBmpInfoHeader = lpGradeBmpInfoHeader;
		 m_lpRgbQuad = lpGradeBmpRgbQuad;
		 m_lpData = lpGradeBmpData;

		 // ������ɫ���־
		 m_bHasRgbQuad = TRUE;  
		 // ����λͼ��Ч��־
		 m_bValid = TRUE;
		 // ���ɵ�ɫ��
		 MakePalette();
		 return TRUE;
	}
	return FALSE;
}

//=======================================================
// �������ܣ� ͼ����
// ��������� ��
// ����ֵ��   BOOL-TRUE �ɹ���FALSE ʧ��
//=======================================================
BOOL CDib::ImageSharp()
{
	// λͼ��Ч��ʧ�ܷ���
    if(!IsValid())
    {
        return FALSE;
    }

    // ��ѹ��λͼ��ʧ�ܷ���
    if(m_lpBmpInfoHeader->biCompression != BI_RGB)
    {
        return FALSE;
    }

    // ����ǻҶ�λͼ���Ž���ת��
    if(IsGrade())
    {
		 // ��ȡԭλͼ��Ϣ
		 LONG lHeight = GetHeight();
		 LONG lWidth = GetWidth();
		 UINT uLineByte = GetLineByte();

		 //�м����
		 BYTE bTemp, bThre = 2;
		 BYTE *pSrc, *pSrc1, *pSrc2;

		// ������ɫת��
		for(int i = 0; i < lHeight; i++)
		{
			 for(int j = 0; j < lWidth; j++)
			 {
				 // ָ��DIB��i�У���j�����ص�ָ��
				pSrc = &m_lpData[uLineByte * (lHeight - 1 - i) + j];
			
				// ָ��DIB��i+1�У���j�����ص�ָ��
				pSrc1 = &m_lpData[uLineByte * (lHeight - 2 - i) + j];
			
				// ָ��DIB��i�У���j+1�����ص�ָ��
				pSrc2 = &m_lpData[uLineByte * (lHeight - 1 - i) + j + 1];
			

				//�����ݶ�ֵ
				bTemp = abs((*pSrc)-(*pSrc1)) + abs((*pSrc)-(*pSrc2));
			
				// �ж��Ƿ�С����ֵ
				if (bTemp < 255)
				{  

					// �ж��Ƿ������ֵ������С��������Ҷ�ֵ���䡣
				    if (bTemp >= bThre)
				    {

						// ֱ�Ӹ�ֵΪbTemp
						*pSrc = bTemp;

				    }

				}
			    else
				{
					// ֱ�Ӹ�ֵΪ255
					*pSrc = 255;
				}
			}
		}

		//���Ҫ����һ��ͼ��������������
		for(int j = 0; j < lWidth; j++)
		{   
	
		    //����λ�õ���������Ϊ255�����׵�
			m_lpData[j] = 255;

		}
		return TRUE;
	}
	return FALSE;
}

//=======================================================
// �������ܣ� �ж���ɢ��
// ��������� ��
// ����ֵ��   BOOL-TRUE ������ɢ�㣻FALSE ��������ɢ�� 
//=======================================================
BOOL CDib::DeleteScaterJudge(LPBYTE lpmark, int x, int y, CPoint mark[], int thres)
{
	// ��ȡԭλͼ��Ϣ
    LONG lHeight = GetHeight();
    LONG lWidth = GetWidth();
    UINT uLineByte = GetLineByte();
	
	//���������������Ҫ��˵��������ɢ�㣬����
	if(m_lConsec >= thres)
		return TRUE;
	
	//���ȼ�һ
	m_lConsec++;

	//�趨���ʱ�־
	lpmark[lWidth * y + x] = true;
	
	//������ʵ�����
	mark[m_lConsec - 1].x = x;
	mark[m_lConsec - 1].y = y;

	//���صĻҶ�ֵ
	LONG gray;
  
	//��������ʡʱʡ��
	int dir[8][2] = {0, 1,  1, 0,  1, 1,  -1, -1,  -1, 1,  1, -1,  -1, 0,  0, -1};

    //���������������Ҫ��˵��������ɢ�㣬����
	if(m_lConsec >= thres)
		return TRUE;
	else
	{	
		//����Ǻ�ɫ�㣬����ú���������еݹ�	
		for(int i = 0; i < 8; i++) 
		{
			int nx = x + dir[i][0], ny = y + dir[i][1];
			//���ݻҶ�ֵ
			gray = m_lpData[uLineByte * ny + nx];

			if(gray == 255) continue;
			//�������ͼ���ڡ���ɫΪ��ɫ����û�б����ʹ�
			if(ny >= 0 && lpmark[ny * lWidth + nx] == false)

			//���еݹ鴦��		
			DeleteScaterJudge(lpmark, nx, ny, mark, thres);

			//���������������Ҫ��˵��������ɢ�㣬����
			if(m_lConsec >= thres)
				return TRUE;
		}
	}
	//����ݹ����������false��˵������ɢ��
	return FALSE;
}

//=======================================================
// �������ܣ� ͼ��ȥ��
// ��������� ��
// ����ֵ��   BOOL-TRUE �ɹ���FALSE ʧ��
//=======================================================
BOOL CDib::RemoveScatterNoise()
{
	//����
	int dir[8][2] = {-1, -1, 0, 1, 1, 1, 1, 0, 1, -1, 0, -1, -1, -1, -1, 0};

	// λͼ��Ч��ʧ�ܷ���
    if(!IsValid())
    {
        return FALSE;
    }

    // ��ѹ��λͼ��ʧ�ܷ���
    if(m_lpBmpInfoHeader->biCompression != BI_RGB)
    {
        return FALSE;
    }

    // ����ǻҶ�λͼ���Ž���ȥ��
    if(IsGrade())
    {
        // ��ȡԭλͼ��Ϣ
        LONG lHeight = GetHeight();
        LONG lWidth = GetWidth();
        UINT uLineByte = GetLineByte();

		// �趨��ֵ
		CONST LONG thres = 15;

		// ��ʼ��������
		m_lConsec = 0;
 
		//����һ��������ű�־���ڴ�����
		LPBYTE lpmark = new BYTE[lHeight * lWidth];

		//����һ������������ɢ�ж�������ڴ�����
		bool *lpTemp = new bool[lHeight * lWidth];

		for (int i = 0; i < lHeight * lWidth; i++)
			//�����еı�־λ����Ϊ��
			lpmark[i] = false;

		//���������ɢ������������
		CPoint mark[40];
   	
		//ɨ������ͼ��

		for(int i = 0; i < lHeight; i++)
		{  
			for(int j = 0; j < lWidth; j++)
			{	
				//�Ȱѱ�־λ��false
				for(int k = 0; k < m_lConsec; k++)
					lpmark[mark[k].y * lWidth + mark[k].x] = false;

				//��������0
				m_lConsec = 0;

				//������ɢ���ж�
				lpTemp[i * lWidth + j] = DeleteScaterJudge(lpmark, j, i, mark, thres);
			}
		}

		//ɨ������ͼ�񣬰���ɢ�����ɰ�ɫ
		for(int i = 0; i < lHeight; i++)
		{
			for(int j = 0; j < lWidth; j++)
			{       
				// �鿴��־λ,���Ϊ���򽫴˵���Ϊ�׵�
				if(lpTemp[i * lWidth + j] == false)
				{	
					// ����i�е�j��������Ϊ��ɫ
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
// �������ܣ� �������
// ��������� ��
// ����ֵ��   BOOL-TRUE �ɹ���FALSE ʧ��
//=======================================================
BOOL CDib::FillNumber()
{
	//����
	int dir[8][2] = {-1, -1, 0, 1, 1, 1, 1, 0, 1, -1, 0, -1, -1, -1, -1, 0};

	// λͼ��Ч��ʧ�ܷ���
    if(!IsValid())
    {
        return FALSE;
    }

    // ��ѹ��λͼ��ʧ�ܷ���
    if(m_lpBmpInfoHeader->biCompression != BI_RGB)
    {
        return FALSE;
    }

    // ����ǻҶ�λͼ���Ž����޲�
    if(IsGrade())
    {
		 // ��ȡԭλͼ��Ϣ
		 LONG lHeight = GetHeight();
		 LONG lWidth = GetWidth();
		 UINT uLineByte = GetLineByte();

		 //��¼��Ҫ���ĵ�
		 bool *needRepair = new bool[uLineByte * (lHeight + lWidth)];

		 for(int i = 0; i < lHeight; i++)
		{
			 for(int j = 0; j < lWidth; j++)
			 {
				 needRepair[i * uLineByte + j] = false; 
			 }
		 }

		// ������䴦��
		for(int i = 0; i < lHeight; i++)
		{
			 for(int j = 0; j < lWidth; j++)
			 {
				 if (m_lpData[i * uLineByte + j] != 255) continue; 

				 int findBlack = 0; //ͳ����Χ�ڵ�

				 //������Χ�˸�����ͳ�ƺڵ����
				 for (int k = 0; k < 8; ++k)
				 {
					 int nx = i + dir[k][0], ny = j + dir[k][1];
					 if (0 <= nx && nx < lHeight && 0 <= ny && ny < lWidth)
					 {
						if (m_lpData[nx * uLineByte + ny] == 0) ++findBlack;
					 }
				 }

				 //����ҵ�����4���ڵ�
				 if (findBlack > 4)
				 {
					 needRepair[i * uLineByte + j] = true;
				 }
			 }
		}

		//�����޲�
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
// �������ܣ� б�ʵ���
// ��������� ��
// ����ֵ��   BOOL-TRUE �ɹ���FALSE ʧ��
//=======================================================
BOOL CDib::SlopeAdjust()
{
	// λͼ��Ч��ʧ�ܷ���
    if(!IsValid())
    {
        return FALSE;
    }

    // ��ѹ��λͼ��ʧ�ܷ���
    if(m_lpBmpInfoHeader->biCompression != BI_RGB)
    {
        return FALSE;
    }

    // ����ǻҶ�λͼ���Ž�������
    if(IsGrade())
    {
		 // ��ȡԭλͼ��Ϣ
		 LONG lHeight = GetHeight();
		 LONG lWidth = GetWidth();
		 UINT uLineByte = GetLineByte();

		 // ͼ�����Ұ�ߵ�ƽ���߶�
		 double dLeftAver = 0.0;
		 double dRightAver = 0.0;

		 // ͼ�����б��
		 double dSlope;

		 //ѭ������
		 LONG i, j;

		 // ͳ��ѭ������
		 LONG lCounts = 0;

		 // ɨ�����ߵ�ͼ�����ɫ���ص�ƽ���߶�
		 for (i = 0; i < lHeight; ++i)
		 {
			 for (j = 0; j < lWidth / 2; ++j)
			 {
				 // ����Ǻڵ�
				 if (m_lpData[uLineByte *  i + j] == 0)
				 {
					 // ����߶Ƚ���ͳ�Ƶ���
					 lCounts += lWidth / 2 - j;
					 dLeftAver += i * (lWidth / 2 - j);
				 }
			 }
		 }

		 // ����ƽ���߶�
		 dLeftAver /= lCounts;

		 // ��ͳ��ѭ���������¸�ֵ
		 lCounts =0;

		// ɨ���Ұ�ߵ�ͼ�����ɫ���ص�ƽ���߶�
		for (i = 0; i < lHeight; i++)
		{
			for (j = lWidth / 2; j < lWidth; j++)
			{
				// ����Ǻڵ�
				if (m_lpData[uLineByte *  i + j] == 0)
				{
					// ����߶Ƚ���ͳ�Ƶ���
					lCounts += lWidth / 2 - j;
					dRightAver += i * (lWidth / 2 - j);
				}
			}
		}

		// �����Ұ�ߵ�ƽ���߶�
		dRightAver /= lCounts;
	
		// ����б��
		dSlope = (dLeftAver - dRightAver) / (lWidth/2);

		// ����Ҷ�λͼ��������ռ�
		 UINT uGradeBmpLineByte = (lWidth + 3) / 4 * 4;
		 DWORD dwGradeBmpDataSize = uGradeBmpLineByte * lHeight; 

		 // ����Ҷ�λͼ����ռ�
		 DWORD dwGradeBmpSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256 + dwGradeBmpDataSize;

		 // ���ûҶ�λͼ�ļ�ͷ
		 LPBITMAPFILEHEADER lpGradeBmpFileHeader = (LPBITMAPFILEHEADER)new BYTE[sizeof(BITMAPFILEHEADER)];
		 memset(lpGradeBmpFileHeader, 0, sizeof(BITMAPFILEHEADER));
		 lpGradeBmpFileHeader->bfType = 0x4d42;
		 lpGradeBmpFileHeader->bfSize = sizeof(BITMAPFILEHEADER) + dwGradeBmpSize;
		 lpGradeBmpFileHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
										   + sizeof(RGBQUAD) * 256;
		 lpGradeBmpFileHeader->bfReserved1 = 0;
		 lpGradeBmpFileHeader->bfReserved2 = 0;            

		 // Ϊ�Ҷ�λͼ����ռ䣬����ʼ��Ϊ0
		 LPBYTE lpGradeBmp = (LPBYTE)new BYTE[dwGradeBmpSize];
		 memset(lpGradeBmp, 0, dwGradeBmpSize);

		 // ���ûҶ�λͼ��Ϣͷ
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

		 // ���ûҶ�λͼ��ɫ��
		 LPRGBQUAD lpGradeBmpRgbQuad = (LPRGBQUAD)(lpGradeBmp + sizeof(BITMAPINFOHEADER));

		 // ��ʼ��8λ�Ҷ�ͼ�ĵ�ɫ����Ϣ
		 LPRGBQUAD lpRgbQuad;               
		 for(int k = 0; k < 256; k++)
		 {
			 lpRgbQuad = (LPRGBQUAD)(lpGradeBmpRgbQuad + k);
			 lpRgbQuad->rgbBlue = k; 
			 lpRgbQuad->rgbGreen = k;
			 lpRgbQuad->rgbRed = k;
			 lpRgbQuad->rgbReserved = 0;
		 }

		 // �Ҷ�λͼ���ݴ���
		 LPBYTE lpGradeBmpData = (LPBYTE)(lpGradeBmp + sizeof(BITMAPINFOHEADER) 
										  + sizeof(RGBQUAD) * 256);
		 // ���ص�ĻҶ�ֵ
		 int gray;
    
		 // λ��ӳ��ֵ
		 int i_src;

		 // ����б�ʣ��ѵ�ǰ��ͼ��ĵ�ӳ�䵽Դͼ��ĵ�
		 for (i = 0; i < lHeight; i++)
		 {
   			for (j = 0; j < lWidth; j++)
			{
				i_src=int(i - (j - lWidth / 2) * dSlope);

				//�������ͼ���⣬�����ð�ɫ
				if (i_src <0 || i_src >=lHeight )
					gray = 255;
				else
				{	
					//����Դͼ�����ҵ㣬ȡ������ֵ
					gray = m_lpData[uLineByte *  i_src + j];
				}
				//����ͼ��ĵ��õõ�������ֵ���
				lpGradeBmpData[uLineByte * i + j] = gray;
			}
		 }

		 // �ͷ�ԭ��λͼ�ռ�
        Empty(FALSE);

        // �����趨ԭλͼָ��ָ��
        m_lpBmpFileHeader = lpGradeBmpFileHeader;
        m_lpDib = lpGradeBmp;
        m_lpBmpInfo = (LPBITMAPINFO)(lpGradeBmp);
        m_lpBmpInfoHeader = lpGradeBmpInfoHeader;
        m_lpRgbQuad = lpGradeBmpRgbQuad;
        m_lpData = lpGradeBmpData;

        // ������ɫ���־
        m_bHasRgbQuad = TRUE;

        // ����λͼ��Ч��־
        m_bValid = TRUE;

        // ���ɵ�ɫ��
        MakePalette();

		return TRUE;
	}
	return FALSE;
}

//=======================================================
// �������ܣ� �ַ��ָ�
// ��������� ��
// ����ֵ��   BOOL-TRUE �ɹ���FALSE ʧ��
//=======================================================
BOOL CDib::NumberDivide()
{
	// λͼ��Ч��ʧ�ܷ���
    if(!IsValid())
    {
        return FALSE;
    }

    // ��ѹ��λͼ��ʧ�ܷ���
    if(m_lpBmpInfoHeader->biCompression != BI_RGB)
    {
        return FALSE;
    }

    // ����ǻҶ�λͼ���Ž��зָ�
    if(IsGrade())
    {
		 //�����������ÿ���ַ����������
		CRectLink charRect1,charRect2;
		charRect1.clear();
		charRect2.clear();

		// ��ȡԭλͼ��Ϣ
		 LONG lHeight = GetHeight();
		 LONG lWidth = GetWidth();
		 UINT uLineByte = GetLineByte();

		//�������±߽���������
		int top, bottom;

		//����ѭ������
		int i,j;

		//����ͳ��ͼ�����ַ������ļ�����
		digitCount = 0;


		//��������ɨ�裬�ҵ��ϱ߽�
		for (i = 0; i < lHeight; i++)
		{
  			for (j = 0; j < lWidth; j++)
			{
				//���Ƿ�Ϊ�ڵ�
				if (m_lpData[uLineByte * i + j] == 0)
				{   
				   //��Ϊ�ڵ㣬�Ѵ˵���Ϊ�ַ����µ���ߵ�
					top = i;

					//��iǿ�и�ֵ���ж�ѭ��
					i = lHeight;

					//����ѭ��
					break;
				}
			}
		}


		//��������ɨ�裬���±߽�
		for (i = lHeight - 1; i >= 0; i--)
		{
			for (j = 0; j < lWidth; j++)
			{
				//�ж��Ƿ�Ϊ�ڵ�
				if (m_lpData[uLineByte * i + j] == 0)
				{
					//��Ϊ�ڵ㣬�Ѵ˵���Ϊ�ַ����µ���͵�
					bottom = i;

					//��iǿ�и�ֵ���ж�ѭ��
					i = -1;

					//����ѭ��
					break;
				}
			}
	
		}

		//lab �����Ƿ����һ���ַ��ָ�ı�־
		bool lab = false;

		//����ɨ��һ�����Ƿ��ֺ�ɫ��
		bool black = false;

		//���λ����Ϣ�Ľṹ��
		CRect rect;

		//����������
		digitCount=0;
   
		for (i = 0; i < lWidth; i++)
		{
			//��ʼɨ��һ��
			black = false;

			for (j = 0; j < lHeight; j++)
			{	
				//�ж��Ƿ�Ϊ�ڵ�
				if (m_lpData[uLineByte * j + i] == 0)
				{
					//������ֺڵ㣬���ñ�־λ
					black=true;

					//�����û�н���һ���ַ��ķָ�
					if(lab==false)
					{	
						//�������߽�
						rect.left = i;

						//�ַ��ָʼ
						lab = true;
					}

					//����ַ��ָ��Ѿ���ʼ�˾�����ѭ��
					else break;
				}		
			}

			//����Ѿ�ɨ�������ұ����У�˵������ͼ��ɨ����ϡ��˳�
			if(i == (lWidth - 1))
			   break;

			//�������black��Ϊfalse��˵��ɨ����һ�У���û�з��ֺڵ㡣������ǰ�ַ��ָ����
			if( lab == true && black == false)
			{   
			   //��λ����Ϣ����ṹ����

			   //�����ұ߽�
				rect.right =i;

				//�����ϱ߽�
				rect.top =top;

				//�����±߽�
				rect.bottom =bottom;

				//��������һ�����أ�����ѹ���ַ�
				rect.InflateRect(1,1);

				//������ṹ�������λ����Ϣ������1�ĺ���
				charRect1.push_back(rect);

				//���ñ�־λ����ʼ��һ�ε��ַ��ָ�
				lab = false;

				//�ַ�����ͳ�Ƽ�������1
				digitCount++;
			}
		}

	   //�ٽ������������ε�top��bottom��ȷ��

		//������1��ֵ������2
		charRect2 = charRect1;

		//������2���������
		charRect2.clear ();

		//����һ���µĴ��λ����Ϣ�Ľṹ��
		CRect rectnew;

		//��������1��ͷ��β�������ɨ��
		while(!charRect1.empty())
		{    
			//������1ͷ�ϵõ�һ������
			rect = charRect1.front();

			//������1ͷ����ɾ��һ��
			charRect1.pop_front();

			//������Ӿ�ȷ�ľ�������

			//��þ�ȷ����߽�
			rectnew.left = rect.left - 1 ;

			//��þ�ȷ���ұ߽�
			rectnew.right = rect.right + 1 ;

			//ͨ����õľ�ȷ���ұ߽�����±߾����½��о�ȷ��λ

			// ���¶���ɨ������ϱ߽�
			for(i = rect.top; i < rect.bottom; i++)
			{   
				for(j = rect.left; j < rect.right; j++)
				{
					//�����������Ǻڵ�
					if (m_lpData[uLineByte * i + j] == 0)
					{	
						//�����ϱ߽�
						rectnew.top = i - 1;
					
						//��i����ǿ�ƶ���������ѭ��
						i = rect.bottom;

						//����ѭ��
						break;
					}	
				}
			}

			//���¶���ɨ������±߽�
			for(i = rect.bottom - 1; i >= rect.top; i--)
			{
				for(j = rect.left; j < rect.right; j++)
				{
					//�õ����Ϊ�ڵ�
					if (m_lpData[uLineByte * i + j] == 0)
					{	
						//�����±߽�
						rectnew.bottom = i + 1;

						//��i����ǿ�ƶ���������ѭ��
						i = -1;

						//����ѭ��
						break;
					}	
				}
			}

			//���õ����µ�׼ȷ��λ����Ϣ�Ӻ���嵽����2��β��
			charRect2.push_back(rectnew);
		}

		//������2 ���ݸ�����1
		charRect1 = charRect2;

		//������1����
		m_charRect = charRect1;

		return TRUE;
	}
	return FALSE;
}

//=======================================================
// �������ܣ� ���ƾ��ο�
// ��������� ��
// ����ֵ��   BOOL-TRUE �ɹ���FALSE ʧ��
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
		//�ӱ�ͷ�ϵõ�һ������
		rect2 = rect = tempRect.front();
		//������ͷ����ɾ��һ��
		tempRect.pop_front();
		//ע�⣬����ԭ�ȵ�rect�������ͼ��ԭ��(���½�)�ģ�
		//������Ļ�ϻ�ͼʱ��Ҫת���Կͻ���Ϊԭ�������
		rect.top = GetHeight() - rect2.bottom;
		rect.bottom =GetHeight() - rect2.top;
		pDC->Rectangle (&rect);
	}
}

//=======================================================
// �������ܣ� ��һ������
// ��������� ��
// ����ֵ��   BOOL-TRUE �ɹ���FALSE ʧ��
//=======================================================
BOOL CDib::ToSame(double tarWidth, double tarHeight)
{
	// λͼ��Ч��ʧ�ܷ���
    if(!IsValid())
    {
        return FALSE;
    }

    // ��ѹ��λͼ��ʧ�ܷ���
    if(m_lpBmpInfoHeader->biCompression != BI_RGB)
    {
        return FALSE;
    }

    // ����ǻҶ�λͼ���Ž��й�һ��
    if(IsGrade())
    {
		//�����˹�һ��
		bToSame = true;

		// ��ȡԭλͼ��Ϣ
		LONG lHeight = GetHeight();
		LONG lWidth = GetWidth();
		UINT uLineByte = GetLineByte();

		// ѭ������
		int	i, j;

		//��ȡ��߶ȷ����ϵ���������
		double wscale, hscale;

		// ����Ҷ�λͼ��������ռ�
		 UINT uGradeBmpLineByte = (lWidth + 3) / 4 * 4;
		 DWORD dwGradeBmpDataSize = uGradeBmpLineByte * lHeight; 

		 // ����Ҷ�λͼ����ռ�
		 DWORD dwGradeBmpSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256 + dwGradeBmpDataSize;

		 // ���ûҶ�λͼ�ļ�ͷ
		 LPBITMAPFILEHEADER lpGradeBmpFileHeader = (LPBITMAPFILEHEADER)new BYTE[sizeof(BITMAPFILEHEADER)];
		 memset(lpGradeBmpFileHeader, 0, sizeof(BITMAPFILEHEADER));
		 lpGradeBmpFileHeader->bfType = 0x4d42;
		 lpGradeBmpFileHeader->bfSize = sizeof(BITMAPFILEHEADER) + dwGradeBmpSize;
		 lpGradeBmpFileHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
										   + sizeof(RGBQUAD) * 256;
		 lpGradeBmpFileHeader->bfReserved1 = 0;
		 lpGradeBmpFileHeader->bfReserved2 = 0;            

		 // Ϊ�Ҷ�λͼ����ռ䣬����ʼ��Ϊ0
		 LPBYTE lpGradeBmp = (LPBYTE)new BYTE[dwGradeBmpSize];
		 memset(lpGradeBmp, 0, dwGradeBmpSize);

		 // ���ûҶ�λͼ��Ϣͷ
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

		 // ���ûҶ�λͼ��ɫ��
		 LPRGBQUAD lpGradeBmpRgbQuad = (LPRGBQUAD)(lpGradeBmp + sizeof(BITMAPINFOHEADER));

		 // ��ʼ��8λ�Ҷ�ͼ�ĵ�ɫ����Ϣ
		 LPRGBQUAD lpRgbQuad;               
		 for(int k = 0; k < 256; k++)
		 {
			 lpRgbQuad = (LPRGBQUAD)(lpGradeBmpRgbQuad + k);
			 lpRgbQuad->rgbBlue = k; 
			 lpRgbQuad->rgbGreen = k;
			 lpRgbQuad->rgbRed = k;
			 lpRgbQuad->rgbReserved = 0;
		 }

		 // �Ҷ�λͼ���ݴ���
		 LPBYTE lpGradeBmpData = (LPBYTE)(lpGradeBmp + sizeof(BITMAPINFOHEADER) 
										  + sizeof(RGBQUAD) * 256);

		 //��ʼ��Ϊ��ɫ
		 for(i = 0; i < lHeight; i++)
		{   
			for(j = 0; j < lWidth; j++)
			{
				lpGradeBmpData[uLineByte * i + j] = 255;	
			}
		}

		//����ӳ��������������
		int i_src,j_src;

		//����ַ�λ����Ϣ�Ľṹ��
		CRect rect;
		CRect rectnew;

		//�����һ���µľ������������Ա�洢��׼����ľ�����������
		m_charRectCopy.clear();

		//��ͷ��β���ɨ��������
		while(!m_charRect.empty())
		{
			//�ӱ�ͷ�ϵõ�һ������
			rect = m_charRect.front();

			//������ͷ����ɾ��һ��
			m_charRect.pop_front();

			if(rect.Height() < 3 || rect.Width() < 6) continue;

			//������������

			//�����귽�����������
			wscale = tarWidth / rect.Width ();

			//�����귽�����������
			hscale = tarHeight / rect.Height ();

			//�����׼������

			//�ϱ߽�
			rectnew.top = rect.top ;

			//�±߽�
			rectnew.bottom = rect.top + tarHeight;

			//��߽�
			rectnew.left = rect.left ;

			//�ұ߽�
			rectnew.right = rectnew.left + tarWidth;

			//��ԭ���ο��ڵ�����ӳ�䵽�µľ��ο���
			for(i = rectnew.top; i < rectnew.bottom; i++)
			{
				for(j = rectnew.left; j < rectnew.right; j++)
				{   

					//����ӳ������
					i_src = rectnew.top + int((i - rectnew.top) / hscale);
					j_src = rectnew.left + int((j - rectnew.left) / wscale);

					//�����Ӧ�����ص����ӳ�����
					lpGradeBmpData[uLineByte * i + j] = m_lpData[uLineByte * i_src + j_src];
				}
			}
			//����׼����ľ�����������µ�����
			m_charRectCopy.push_back(rectnew);
		}

		//�洢��׼�����µ�rect����
		m_charRect = m_charRectCopy;

		// �ͷ�ԭ��λͼ�ռ�
        Empty(FALSE);

        // �����趨ԭλͼָ��ָ��
        m_lpBmpFileHeader = lpGradeBmpFileHeader;
        m_lpDib = lpGradeBmp;
        m_lpBmpInfo = (LPBITMAPINFO)(lpGradeBmp);
        m_lpBmpInfoHeader = lpGradeBmpInfoHeader;
        m_lpRgbQuad = lpGradeBmpRgbQuad;
        m_lpData = lpGradeBmpData;

        // ������ɫ���־
        m_bHasRgbQuad = TRUE;
        // ����λͼ��Ч��־
        m_bValid = TRUE;
        // ���ɵ�ɫ��
        MakePalette();

		return TRUE;
	}
	return FALSE;
}

//=======================================================
// �������ܣ� ��������
// ��������� ��
// ����ֵ��   BOOL-TRUE �ɹ���FALSE ʧ��
//=======================================================
BOOL CDib::AutoAlign()
{
	// λͼ��Ч��ʧ�ܷ���
    if(!IsValid())
    {
        return FALSE;
    }

    // ��ѹ��λͼ��ʧ�ܷ���
    if(m_lpBmpInfoHeader->biCompression != BI_RGB)
    {
        return FALSE;
    }

    // ����ǻҶ�λͼ�ҽ����˹�һ�����Ž��н���
    if(IsGrade() && bToSame)
	{
		// ��ȡԭλͼ��Ϣ
		LONG lHeight = GetHeight();
		LONG lWidth = GetWidth();
		UINT uLineByte = GetLineByte();	
	
		//��ȡ��׼���Ŀ��
		int w = m_charRect.front ().Width();

		//��ȡ��׼���ĸ߶�
		int h = m_charRect.front ().Height();
	
		// ��þ�������
		digitCount = m_charRect.size();
    
		// ����Ҷ�λͼ��������ռ�
		UINT uGradeBmpLineByte = (w * digitCount + 3) / 4 * 4;
		DWORD dwGradeBmpDataSize = uGradeBmpLineByte * h; 

		// ����Ҷ�λͼ����ռ�
		DWORD dwGradeBmpSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256 + dwGradeBmpDataSize;

		// ���ûҶ�λͼ�ļ�ͷ
		LPBITMAPFILEHEADER lpGradeBmpFileHeader = (LPBITMAPFILEHEADER)new BYTE[sizeof(BITMAPFILEHEADER)];
		memset(lpGradeBmpFileHeader, 0, sizeof(BITMAPFILEHEADER));
		lpGradeBmpFileHeader->bfType = 0x4d42;
		lpGradeBmpFileHeader->bfSize = sizeof(BITMAPFILEHEADER) + dwGradeBmpSize;
		lpGradeBmpFileHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
											+ sizeof(RGBQUAD) * 256;
		lpGradeBmpFileHeader->bfReserved1 = 0;
		lpGradeBmpFileHeader->bfReserved2 = 0;            

		// Ϊ�Ҷ�λͼ����ռ䣬����ʼ��Ϊ0
		LPBYTE lpGradeBmp = (LPBYTE)new BYTE[dwGradeBmpSize];
		memset(lpGradeBmp, 0, dwGradeBmpSize);

		// ���ûҶ�λͼ��Ϣͷ
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

		// ���ûҶ�λͼ��ɫ��
		LPRGBQUAD lpGradeBmpRgbQuad = (LPRGBQUAD)(lpGradeBmp + sizeof(BITMAPINFOHEADER));

		// ��ʼ��8λ�Ҷ�ͼ�ĵ�ɫ����Ϣ
		LPRGBQUAD lpRgbQuad;               
		for(int k = 0; k < 256; k++)
		{
			lpRgbQuad = (LPRGBQUAD)(lpGradeBmpRgbQuad + k);
			lpRgbQuad->rgbBlue = k; 
			lpRgbQuad->rgbGreen = k;
			lpRgbQuad->rgbRed = k;
			lpRgbQuad->rgbReserved = 0;
		}

		// �Ҷ�λͼ���ݴ���
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
		// �ͷ�ԭ��λͼ�ռ�
		Empty(FALSE);

		// �����趨ԭλͼָ��ָ��
		m_lpBmpFileHeader = lpGradeBmpFileHeader;
		m_lpDib = lpGradeBmp;
		m_lpBmpInfo = (LPBITMAPINFO)(lpGradeBmp);
		m_lpBmpInfoHeader = lpGradeBmpInfoHeader;
		m_lpRgbQuad = lpGradeBmpRgbQuad;
		m_lpData = lpGradeBmpData;

		// ������ɫ���־
		m_bHasRgbQuad = TRUE;  
		// ����λͼ��Ч��־
		m_bValid = TRUE;
		// ���ɵ�ɫ��
		MakePalette();
		return TRUE;
	}
	return FALSE;
}

//=======================================================
// �������ܣ� 8λ�Ҷ�λͼת24λ��ɫλͼ
// ��������� ��
// ����ֵ��   BOOL-TRUE �ɹ���FALSE ʧ��
//=======================================================
BOOL CDib::GradeToRgb()
{
    // λͼ��Ч��ʧ���˳�
    if(!IsValid())
    {
        return FALSE;
    }

    // ����8λλͼ��ʧ���˳�
    if(GetBitCount() != 8)
    {
        return FALSE;
    }

    // ��ѹ��λͼ��ʧ�ܷ���
    if(m_lpBmpInfoHeader->biCompression != BI_RGB)
    {
        return FALSE;
    }

    // �ǻҶ�ͼʱ������ת��
    if(IsGrade())
    {
        // ��ȡԭλͼ��Ϣ
        LONG lHeight = GetHeight();
        LONG lWidth = GetWidth();
        UINT uLineByte = GetLineByte();

        // �����ɫλͼ��������ռ�
        UINT uColorBmpLineByte = (lWidth * 24 / 8 + 3) / 4 * 4;
        DWORD dwColorBmpDataSize = uColorBmpLineByte * lHeight; 

        // �����ɫλͼ����ռ�
        DWORD dwColorBmpSize = sizeof(BITMAPINFOHEADER) + dwColorBmpDataSize;

        // ���ò�ɫλͼ�ļ�ͷ
        LPBITMAPFILEHEADER lpColorBmpFileHeader = (LPBITMAPFILEHEADER)new BYTE[sizeof(BITMAPFILEHEADER)];
        memset(lpColorBmpFileHeader, 0, sizeof(BITMAPFILEHEADER));
        lpColorBmpFileHeader->bfType = 0x4d42;
        lpColorBmpFileHeader->bfSize = sizeof(BITMAPFILEHEADER) + dwColorBmpSize;
        lpColorBmpFileHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
        lpColorBmpFileHeader->bfReserved1 = 0;
        lpColorBmpFileHeader->bfReserved2 = 0;    

        // Ϊ��ɫλͼ����ռ䣬����ʼ��Ϊ0
        LPBYTE lpColorBmp = (LPBYTE)new BYTE[dwColorBmpSize];
        memset(lpColorBmp, 0, dwColorBmpSize);

        // ���ò�ɫλͼ��Ϣͷ
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

        // ��ɫλͼ���ݴ���
        LPBYTE lpColorBmpData = (LPBYTE)(lpColorBmp + sizeof(BITMAPINFOHEADER));
        // ������ɫת��
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

        // �ͷ�ԭ��λͼ�ռ�
        Empty(FALSE);

        // �����趨ԭλͼָ��ָ��
        m_lpBmpFileHeader = lpColorBmpFileHeader;
        m_lpDib = lpColorBmp;
        m_lpBmpInfo = (LPBITMAPINFO)(lpColorBmp);
        m_lpBmpInfoHeader = lpColorBmpInfoHeader;
        m_lpRgbQuad = NULL;
        m_lpData = lpColorBmpData;

        // ������ɫ���־
        m_bHasRgbQuad = FALSE;  
        // ����λͼ��Ч��־
        m_bValid = TRUE;
		return TRUE;
    }        

    return FALSE;   
}   
 
//=======================================================
// �������ܣ� �ж��Ƿ�����ɫ��
// ��������� ��
// ����ֵ��   �жϽ����TRUE-������ɫ��FALSE-������ɫ��
//=======================================================
BOOL CDib::HasRgbQuad()
{
    return m_bHasRgbQuad;
}

//=======================================================
// �������ܣ� �ж��Ƿ��ǻҶ�ͼ
// ��������� ��
// ����ֵ��   �жϽ����TRUE-�ǻҶ�ͼ��FALSE-�ǲ�ɫͼ
//=======================================================
BOOL CDib::IsGrade()
{
    return (GetBitCount() < 9 && GetBitCount() > 0);
}

//=======================================================
// �������ܣ� �ж�λͼ�Ƿ���Ч
// ��������� ��
// ����ֵ��   �жϽ����TRUE-λͼ��Ч��FALSE-λͼ��Ч
//=======================================================
BOOL CDib::IsValid()
{
    return m_bValid;
}

//=======================================================
// �������ܣ� ����ռ�
// ��������� BOOL bFlag-TRUE ȫ����գ�FALSE �������
// ����ֵ��   ��
//=======================================================
void CDib::Empty(BOOL bFlag)
{
    // �ļ������
    if(bFlag)
    {
		StrCpyW(m_fileName, _T(""));
    }      

    // �ͷ�λͼ�ļ�ͷָ��ռ�
    if(m_lpBmpFileHeader != NULL)
    {
        delete [] m_lpBmpFileHeader;
        m_lpBmpFileHeader = NULL;
    }    

    // �ͷ�λͼָ��ռ�
    if(m_lpDib != NULL)
    {
        delete [] m_lpDib;
        m_lpDib = NULL;
        m_lpBmpInfo = NULL;
        m_lpBmpInfoHeader = NULL;
        m_lpRgbQuad = NULL;
        m_lpData = NULL;           
    }       

    // �ͷŵ�ɫ��
    if(m_hPalette != NULL)
    {
        DeleteObject(m_hPalette);
        m_hPalette = NULL;
    }    

    // ���ò�����ɫ��
    m_bHasRgbQuad = FALSE;
    
    // ����λͼ��Ч
    m_bValid = FALSE;

}  
