
// MFCApplication1Dlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MFCApplication1.h"
#include "MFCApplication1Dlg.h"
#include "afxdialogex.h"
#include<conio.h>
#include "CompileMXF.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCApplication1Dlg �Ի���



CMFCApplication1Dlg::CMFCApplication1Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFCAPPLICATION1_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_EditVideo);
	DDX_Control(pDX, IDC_EDIT2, m_EditAudio);
	DDX_Control(pDX, IDC_EDIT3, m_EditOut);
	DDX_Control(pDX, IDC_EDIT4, m_EditDuration);
}

BEGIN_MESSAGE_MAP(CMFCApplication1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCApplication1Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCApplication1Dlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CMFCApplication1Dlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CMFCApplication1Dlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CMFCApplication1Dlg ��Ϣ�������

BOOL CMFCApplication1Dlg::OnInitDialog()
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	AllocConsole();
	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	m_EditAudio.SetWindowText(L"F:\\Avid MediaFiles\\MXF\\A01.5AB88DEA_55AB88DE9.mxf");
	m_EditOut.SetWindowText(L"F:\\Avid MediaFiles\\MXF\\1");
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMFCApplication1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMFCApplication1Dlg::OnPaint()
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
HCURSOR CMFCApplication1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//Video
void CMFCApplication1Dlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strExt(_T("*"));
	CString strFilter(_T("Input�ļ�(*.mxf)|*.mxf||"));

	CFileDialog filedialog(TRUE, strExt, NULL, OFN_FILEMUSTEXIST, strFilter, this);

	if (filedialog.DoModal() != IDOK)
		return;

	m_EditVideo.SetWindowText(filedialog.GetPathName());
	
}

//Audio
void CMFCApplication1Dlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strExt(_T("*"));
	CString strFilter(_T("Input�ļ�(*.mxf)|*.mxf||"));

	CFileDialog filedialog(TRUE, strExt, NULL, OFN_FILEMUSTEXIST, strFilter, this);

	if (filedialog.DoModal() != IDOK)
		return;

	m_EditAudio.SetWindowText(filedialog.GetPathName());
}

//Out
void CMFCApplication1Dlg::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BROWSEINFO bInfo = { 0 };
	LPITEMIDLIST lpDlist;
	TCHAR szPathName[MAX_PATH] = {0};

	lpDlist = SHBrowseForFolder(&bInfo);

	SHGetPathFromIDList(lpDlist, szPathName);

	m_EditOut.SetWindowText(szPathName);
}

//Read
void CMFCApplication1Dlg::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CCombinationMXF oMyCompileAudio;
	CString csAudio,csOut;
	DWORD dwMinSize;
	CString csDuration;
	int iDuration = 0;

	m_EditDuration.GetWindowText(csDuration);
	m_EditAudio.GetWindowText(csAudio);
	m_EditOut.GetWindowText(csOut);

	iDuration = _ttoi(csDuration);

	dwMinSize = WideCharToMultiByte(CP_ACP, NULL, csAudio, -1, NULL, 0, NULL, FALSE);
	char * chRead = new char[dwMinSize];
	WideCharToMultiByte(CP_OEMCP, NULL, csAudio, -1, chRead, dwMinSize, NULL, FALSE);

	dwMinSize = WideCharToMultiByte(CP_ACP, NULL, csOut, -1, NULL, 0, NULL, FALSE);
	char * chOut = new char[dwMinSize];
	WideCharToMultiByte(CP_OEMCP, NULL, csOut, -1, chOut, dwMinSize, NULL, FALSE);
	

	oMyCompileAudio.SetRWMXFPath(chRead,chOut);

	delete[] chRead; chRead = nullptr;
	delete[] chOut; chOut = nullptr;

	oMyCompileAudio.CombinationHeader();
	
	char *chData = new char[1920];
	char *chV = new char[3887104];

	FILE *fpAudio = nullptr;
	FILE *fpVideo = nullptr;

	fopen_s(&fpAudio, "F:\\Avid MediaFiles\\MXF\\A02.5AB88DEB_55AB88DE9.mxf", "rb+");
	fopen_s(&fpVideo, "F:\\Avid MediaFiles\\MXF\\V01.5AB88F03_55AB88F03.mxf", "rb+");
	
	for (int i = 0; i < iDuration; i++)
	{
		_fseeki64(fpAudio,266240+i*1920,SEEK_SET);
		fread_s(chData, 1920, 1, 1920, fpAudio);
		oMyCompileAudio.CombinationAudioData(chData, 1920);
		
		_fseeki64(fpVideo,393216 + i* 3887104,SEEK_SET);
		fread_s(chV, 3887104, 1, 3887104, fpVideo);
		oMyCompileAudio.CombinationVideoData(chV, 3887104);
	}
	
	oMyCompileAudio.Flush();
	
	fclose(fpAudio);
	fclose(fpVideo);
	delete[] chV; chV = nullptr;
	delete[] chData; chData = nullptr;
}
