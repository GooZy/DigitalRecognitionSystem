//======================================================================
// �ļ��� Dib.h
// ���ݣ� �豸�޹�λͼ��-ͷ�ļ�
// ���ܣ� ��1��λͼ�ļ����뱣�棻
//        ��2��λͼ��Ϣ�Ļ�ȡ��
//        ��3��λͼ���ݵĻ�ȡ��
//        ��3��λͼ����ʾ��
//        ��4��λͼ��ת����
//        ��5��λͼ����жϣ�
// ���ߣ� ��ƽ��
// ����:  ����Ң
// ��ϵ�� lipingke@126.com
// ���ڣ� 2009-7-26
//======================================================================

#pragma once

#include "afx.h"
#include <deque>
using namespace std;
typedef deque<CRect> CRectLink;

class CDib : public CObject
{
public:
    // ���캯������ʼ�����ݳ�Ա
    CDib(void);

    // �����������ͷ��ڴ�ռ�
    ~CDib(void);

    // ���ļ�����λͼ
    BOOL LoadFromFile(LPCTSTR lpszPath);

    // ��λͼ���浽�ļ�
    BOOL SaveToFile(LPCTSTR lpszPath);

    // ��ȡλͼ�ļ���
    LPCTSTR GetFileName();

    // ��ȡλͼ���
    LONG GetWidth();

    // ��ȡλͼ�߶�
    LONG GetHeight();

    // ��ȡλͼ�Ŀ�Ⱥ͸߶�
    CSize GetDimension();  
    
    // ��ȡλͼ��С
    DWORD GetSize();

    // ��ȡ����������ռλ��
    WORD GetBitCount();

    // ��ȡÿ��������ռ�ֽ���
    UINT GetLineByte();

    // ��ȡλͼ��ɫ��
    DWORD GetNumOfColor();

    // ��ȡλͼ��ɫ��
    LPRGBQUAD GetRgbQuad();

    // ��ȡλͼ����
    LPBYTE GetData();
      
    // ��ʾλͼ
    BOOL Draw(CDC *pDC, CPoint origin, CSize size);

    // 24λ��ɫλͼת8λ�Ҷ�λͼ
    BOOL RgbToGrade();

	// 8λ�Ҷ�λͼ��ֵ��
	BOOL GrayToWhiteBlack();

	// ͼ����
	BOOL ImageSharp();

	// �жϲ�ȥ����ɢ���
	BOOL DeleteScaterJudge(LPBYTE lpmrak, int x, int y, CPoint mark[], int consec);

	// ͼ��ȥ��
	BOOL RemoveScatterNoise();

	// �������
	BOOL FillNumber();

	// б�ʵ���
	BOOL SlopeAdjust();

	// �ַ��ָ�
	BOOL NumberDivide();

	//���ƾ��ο�
	void DrawFrame(CDC* pDC, CRectLink tempRect, unsigned int linewidth,COLORREF color);

	//��һ������
	BOOL ToSame(double tarWidth, double tarHeight);

	//��������
	BOOL AutoAlign();

    // 8λ�Ҷ�λͼת24λ��ɫλͼ
    BOOL GradeToRgb();

    // �ж��Ƿ�����ɫ��
    BOOL HasRgbQuad();

    // �ж��Ƿ��ǻҶ�ͼ
    BOOL IsGrade();

    // �ж�λͼ�Ƿ���Ч
    BOOL IsValid();  

	//�и����ַ�
	CRectLink m_charRect;

	// �Ƿ��һ��
	bool bToSame;

protected:
    // ����λͼ��ɫ����
    DWORD CalcRgbQuadLength();

    // ������ɫ�����ɵ�ɫ��
    BOOL MakePalette();

    // ����ռ�
    void Empty(BOOL bFlag = TRUE);

private:
    // λͼ�ļ���
    TCHAR m_fileName[_MAX_PATH];

    // λͼ�ļ�ͷָ��    
    LPBITMAPFILEHEADER m_lpBmpFileHeader; // ��Ҫ��̬������ͷ� 

    // λͼָ�루������λͼ�ļ�ͷ���������ݣ�
	LPBYTE m_lpDib;                       // ��Ҫ��̬������ͷ�

    // λͼ��Ϣָ��
    LPBITMAPINFO m_lpBmpInfo;

	// λͼ��Ϣͷָ��
	LPBITMAPINFOHEADER m_lpBmpInfoHeader;  

    // λͼ��ɫ��ָ��
	LPRGBQUAD m_lpRgbQuad; 

    // λͼ����ָ��
	LPBYTE m_lpData; 

    // ��ɫ����
	HPALETTE m_hPalette;

    // �Ƿ�����ɫ��
    BOOL m_bHasRgbQuad;

    // λͼ�Ƿ���Ч
    BOOL m_bValid;

	// ������
	LONG m_lConsec;

	// ����ͳ��ͼ�����ַ������ļ�����
	int digitCount;

	//��ʱ�洢��һ���ľ���
	CRectLink m_charRectCopy;
};
