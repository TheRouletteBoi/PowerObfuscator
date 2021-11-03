
// PowerObfuscatorDlg.h : header file
//

#pragma once


// CPowerObfuscatorDlg dialog
class CPowerObfuscatorDlg : public CDialogEx
{
// Construction
public:
	CPowerObfuscatorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_POWEROBFUSCATOR_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void LoadPRXButton();
	afx_msg void ProtectPRXButton();
	afx_msg void OnEncryptAllButtonClicked();
	afx_msg void OnAntiDebuggerButtonClicked();
	afx_msg void OnLoadEbootButtonClicked();
};
