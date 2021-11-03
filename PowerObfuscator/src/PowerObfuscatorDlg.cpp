#include "stdafx.h"
#include "PowerObfuscator.h"
#include "PowerObfuscatorDlg.h"
#include "afxdialogex.h"

#include <winerror.h>
#include <windows.h>

#include <ctime>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <map>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma region Controls
CButton* loadPrxButton;
CButton* loadEbootButton;
CButton* loadConfigButton;
CButton* saveConfigButton;
CListCtrl* symbolListBox;

CButton* encryptAllFunctionsCheck;
CButton* encryptSelectedFunctionsCheck;
CButton* encryptDataCheck;

CButton* stripCheck;
CButton* antiIdaCheck;
CButton* antiDebuggerCheck;
CButton* tamperProtectionCheck;

CButton* protectButton;
#pragma endregion

CPowerObfuscatorDlg::CPowerObfuscatorDlg(CWnd* pParent) : CDialogEx(IDD_POWEROBFUSCATOR_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPowerObfuscatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPowerObfuscatorDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(loadPRX_button, &CPowerObfuscatorDlg::LoadPRXButton)
	ON_BN_CLICKED(protect_button, &CPowerObfuscatorDlg::ProtectPRXButton)
	ON_BN_CLICKED(encryptAll_check, &CPowerObfuscatorDlg::OnEncryptAllButtonClicked)
	ON_BN_CLICKED(antiDebugger_check, &CPowerObfuscatorDlg::OnAntiDebuggerButtonClicked)
	ON_BN_CLICKED(loadEboot_button, &CPowerObfuscatorDlg::OnLoadEbootButtonClicked)
END_MESSAGE_MAP()

BOOL CPowerObfuscatorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	std::srand(time(0));

	loadPrxButton = (CButton*)GetDlgItem(loadPRX_button);
	loadEbootButton = (CButton*)GetDlgItem(loadEboot_button);
	loadConfigButton = (CButton*)GetDlgItem(loadConfig_button);
	saveConfigButton = (CButton*)GetDlgItem(saveConfig_button);

	symbolListBox = (CListCtrl*)GetDlgItem(symbols_list);
	encryptAllFunctionsCheck = (CButton*)GetDlgItem(encryptAll_check);
	encryptSelectedFunctionsCheck = (CButton*)GetDlgItem(encryptSelected_check);
	encryptDataCheck = (CButton*)GetDlgItem(encryptData_check);

	stripCheck = (CButton*)GetDlgItem(stripSymbols_check);
	antiIdaCheck = (CButton*)GetDlgItem(antiIDA_check);
	antiDebuggerCheck = (CButton*)GetDlgItem(antiDebugger_check);
	tamperProtectionCheck = (CButton*)GetDlgItem(tamperProtection_check);

	protectButton = (CButton*)GetDlgItem(protect_button);

	symbolListBox->SetExtendedStyle(symbolListBox->GetStyle() | LVS_EX_CHECKBOXES);
	symbolListBox->InsertColumn(0, L"Symbols");
	symbolListBox->SetColumnWidth(0, 999);

	SetIcon(m_hIcon, FALSE);

	return TRUE;
}

void CPowerObfuscatorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CPowerObfuscatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

template<int A, int C, int M>
class Random
{
public:
	Random(UINT32 pSeed) : seed(pSeed) {}
	UINT32 next()
	{
		return seed = (A * seed + C) % M;
	}

private:
	UINT32 seed;
};
struct symbol
{
	symbol() {}
	symbol(std::wstring pName, std::wstring pType, int pOffset)
	{
		name = pName;
		type = pType;
		offset = pOffset;
	}

   std::wstring name;
   std::wstring type;
   int offset;
};

struct sectionInfo
{
	sectionInfo(int p, int s) : ptr(p), sz(s) {}
	sectionInfo() {}

   int ptr;
   int sz;
};

char* gPrxBuffer;
CString gLoadedPRXPath;
int gPrxFileSize;
int gPrxDestroyAddr = 0;
std::vector<symbol> gLoadedSymbols;
int gSymbolCount;
int gPBufferSz;
std::map<std::wstring, sectionInfo> gSections;
std::vector<short> gFnOffsets;

CString GetRunningExecPath()
{
	wchar_t result[MAX_PATH];
	wchar_t short_path[MAX_PATH];
	std::wstring path(result, GetModuleFileName(NULL, result, MAX_PATH));
	GetShortPathNameW(path.substr(0, path.find_last_of(L"\\/") + 1).c_str(), short_path, MAX_PATH);
	return CString(short_path);
}

int FakeSignPRX(CString pFileName)
{
	CString command = GetRunningExecPath() + L"make_fself.exe \"" + pFileName + L".prx\" \"" + pFileName + L".sprx\"";
	char commandcstr[512];
	strcpy_s(commandcstr, CStringA(command).GetString());
	int ret = system(commandcstr);
	remove(pFileName + ".prx");
	return ret;
}

int StripSymbolsFromPRX(CString pFileName)
{
	CString command = GetRunningExecPath() + L"ppu-lv2-prx-strip.exe \"" + pFileName + L".prx\"";
	char commandcstr[512];
	strcpy_s(commandcstr, CStringA(command).GetString());
	return system(commandcstr);
}

// TODO
int FindSectionsInPRX(CString pFileName, std::map<std::wstring, sectionInfo>& sections)
{
	FILE* fp;
	wchar_t line[4096];

	CString command = GetRunningExecPath() + L"readelf.exe -S -W \"" + pFileName + L"\"";
	if ((fp = _wpopen(command, L"r")) == NULL)
	{
		printf("Error opening pipe!\n");
		return -1;
	}

	while (fgetws(line, sizeof(line), fp) != NULL)
	{
		std::wstring sectionName(L".rodata");

		std::wstringstream stream(line);

		std::wstring dummy;
		std::wstring name;
		int off;
		int sz;

		stream >> dummy >> name >> dummy >> dummy >> std::hex >> off >> std::hex >> sz >> dummy >> dummy >> dummy >> dummy;

		sections.insert(std::pair<std::wstring, sectionInfo>(name, sectionInfo(off, sz)));
	}

	if (_pclose(fp))
	{
		printf("Command not found or exited with error status\n");
		return -1;
	}

	return 0;
}

bool ContainsSuffix(std::wstring s, std::wstring suffix)
{
	return s.substr(0, suffix.length()) == suffix;
}

int ProcessPRXSymbols(CString pFileName, std::vector<symbol>* pSymbols)
{
	FILE* fp;
	wchar_t line[4096];

	CString command = GetRunningExecPath() + L"nm.exe -C \"" + pFileName + L"\"";
	if ((fp = _wpopen(command, L"r")) == NULL)
	{
		printf("Error opening pipe!\n");
		return -1;
	}

	while (fgetws(line, sizeof(line), fp) != NULL)
	{
		std::wstringstream stream(line);
		std::wstring name;
		std::wstring type;
		int offset = 0;

		stream >> std::hex >> offset >> type;
		std::wstring partname;
		while (stream >> partname)
		{
			name += partname;
		}

		name = name.substr(1);
		symbol sym(name, type, offset);

		if (ContainsSuffix(sym.name, L"POBF_API"))
			continue;

		if (sym.type == L"T")
			pSymbols->push_back(sym);
	}

	if (_pclose(fp))
	{
		printf("Command not found or exited with error status\n");
		return -1;
	}

	return 0;
}

void GetSymbolInformation(CString pFileName, std::vector<symbol>* symbols)
{
	if (ProcessPRXSymbols(pFileName, symbols))
	{
		MessageBox(NULL, L"Could not dump symbols!", L"Error!", MB_ICONERROR);
	}
}

std::wstring GetPRXPath()
{
	wchar_t dir[MAX_PATH];
	_wsplitpath_s(gLoadedPRXPath, NULL, 0, dir, MAX_PATH, NULL, 0, NULL, 0);
	return dir;
}

std::wstring GetPRXName()
{
	wchar_t name[MAX_PATH];
	_wsplitpath_s(gLoadedPRXPath, NULL, 0, NULL, 0, name, MAX_PATH, NULL, 0);
	return name;
}

bool TestInvalidOperation(int pOperation)
{
	char skip[] = {
		0x30,
		0x3C,
		0x80,
		0xF8
	};

	for (int i = 0; i < sizeof(skip) / sizeof(skip[0]); i++)
	{
		if (((pOperation >> 24) == skip[i]))
		{
			return true;
		}
	}

	return false;
}

template <class T> 
void SwapEndianness(T* objp)
{
	unsigned char* memp = reinterpret_cast<unsigned char*>(objp);
	std::reverse(memp, memp + sizeof(T));
}

void LoadPRX(CFile* pFile, char* pBuffer, int length)
{
	pFile->SeekToBegin();
	pFile->Read(pBuffer, length);
}

bool CheckForLibAPI(char* pBuffer, int* pDecryptOffset)
{
	int code_start = *(int*)(pBuffer + 0x4C);
	SwapEndianness(&code_start);

	while (code_start < gPrxFileSize)
	{
		int operation1 = *(int*)(pBuffer + code_start);
		int operation2 = *(int*)(pBuffer + code_start + 0x04);
		SwapEndianness(&operation1);
		SwapEndianness(&operation2);

		if (operation1 == 0x3C80342E && operation2 == 0x60845333)
		{
			*pDecryptOffset = (int)(pBuffer + code_start);
			return true;
		}

		code_start += 0x04;
	}

	return false;
}

bool CheckMagic(char* pBuffer)
{
	return *(int*)pBuffer == 0x464C457F; // ELF but flipped. real one is: 0x7F454C46
}

void WriteEncryptDataInstructions(int pDataStart, int pDecryptOffset)
{
	int operation;

	operation = 0x60630000 | (pDataStart - 0xF0); //lis r3, %start
	SwapEndianness(&operation);
	*(int*)(pDecryptOffset - 0x24) = operation;

	operation = 0x2C030000 | (gSections[L".rodata"].sz / 0x04); //cmpwi r3, %count
	SwapEndianness(&operation);
	*(int*)(pDecryptOffset + 0x44) = operation;
}

void EncryptAllData(char* pBuffer, int pDecryptOffset)
{
	int data_start = gSections[L".rodata"].ptr;
	int data_end = data_start + gSections[L".rodata"].sz;

	WriteEncryptDataInstructions(data_start, pDecryptOffset);

	while (data_start != data_end)
	{
		int data = *(int*)(pBuffer + data_start);
		SwapEndianness(&data);
		data ^= 0x342E5333;//todo
		SwapEndianness(&data);
		*(int*)(pBuffer + data_start) = data;
		data_start += 0x04;
	}
}

#define START_PATTERN 0xAA, 0xBB, 0xCC, 0xDD, 0x12, 0x34, 0x56, 0x78, 0xEE, 0xFF, 0xEE, 0xFF 

void FindAPIExports(char* pBuffer, int** exports)
{
	char toFind[] = { START_PATTERN };
	char* found = std::search(pBuffer, pBuffer + gPBufferSz, toFind, toFind + sizeof(toFind));
	if (found > 0)
	{
		*exports = (int*)found;
	}
}

void WriteEncryptFnInstrs()
{
	/*
	struct __fnData 
	{
		int16_t count;
		int32_t start;
		int16_t offset[256];
	} fnData __attribute__((section(".fnData")));
	*/

	int fnDataSectionStart = gSections[L".fnData"].ptr;
	int offset = 0;

	int codeStart = *(int*)(gPrxBuffer + 0x4C);

	*(int*)(gPrxBuffer + fnDataSectionStart + (offset++)) = gFnOffsets.size();
	*(int*)(gPrxBuffer + fnDataSectionStart + (offset++)) = codeStart;

	for (short fn : gFnOffsets)
	{
		*(int*)(gPrxBuffer + fnDataSectionStart + (offset++)) = fn;
	}
}

void EncryptSingleFunction(char* pBuffer, int pFunction, int pSeed)
{
	std::unique_ptr rr = std::make_unique<Random<0x111111111, 0x4f0d668d, 0x7FFFFFFF>>(pSeed);

	int code_start = *(int*)(pBuffer + 0x4C);
	SwapEndianness(&code_start);
	code_start += pFunction;

	int operation;
	int operation_big;
	int key;
	do
	{
		key = rr->next();
		operation = *(int*)(pBuffer + code_start);
		SwapEndianness(&operation);
		operation_big = operation;
		if (!TestInvalidOperation(operation))
		{
			operation ^= key;
			SwapEndianness(&operation);
			*(int*)(pBuffer + code_start) = operation;
		}
		code_start += 0x04;
	} while (operation_big != 0x4E800020 && operation_big != 0x4E800420);
}

unsigned int HashString32(char* str)
{
	unsigned int h = 0;
	unsigned char* p;
	for (p = (unsigned char*)str; *p != '\0'; p++)
		h = (37 * h) + *p;
	return h;
}

void DoEncryptFunctions()
{
	int encKey32 = 0x81318923;

	bool encryptAll = encryptAllFunctionsCheck->GetCheck();
	if (encryptAll || encryptSelectedFunctionsCheck->GetCheck())
	{
		int numItems = symbolListBox->GetItemCount(), i = 0;
		while (i < numItems)
		{
			if (encryptAll || symbolListBox->GetCheck(i))
			{
				EncryptSingleFunction(gPrxBuffer, gLoadedSymbols[i].offset, encKey32);
				int code_start = *(int*)(gPrxBuffer + 0x4C);
				gFnOffsets.push_back(code_start + gLoadedSymbols[i].offset);
			}
			i++;
		}
		WriteEncryptFnInstrs();
	}
}

void DoEncryptData(int decryptOffset)
{
	if (encryptDataCheck->GetCheck())
	{
		EncryptAllData(gPrxBuffer, decryptOffset);
	}
}

CString DoWritePRXBufferToFile()
{
	CFile savefile;
	CString savepath = L"C:" + CString(GetPRXPath().c_str());
	CString savename = savepath + CString(GetPRXName().c_str()) + L"_obf";
	if (savefile.Open(savename + L".prx", CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary))
	{
		savefile.Write(gPrxBuffer, gPrxDestroyAddr == 0 ? gPrxFileSize : gPrxDestroyAddr);
		savefile.Close();
	}
	delete[] gPrxBuffer;

	return savename;
}

void DoStripPRXFile(CString saveFile)
{
	if (stripCheck->GetCheck())
	{
		if (StripSymbolsFromPRX(saveFile))
		{
			MessageBox(NULL, L"Failed to strip PRX!", L"Error!", MB_ICONERROR);
		}
	}
}

void DoFakeSignPRX(CString saveFile)
{
	if (FakeSignPRX(saveFile))
	{
		MessageBox(NULL, L"Failed to convert to SPRX!", L"Error!", MB_ICONERROR);
	}
}

void DestroySectionInfos(CString filePath)
{
	CFile prxfile;
	char* pBuffer = NULL;

	if (prxfile.Open(filePath, CFile::modeReadWrite | CFile::typeBinary))
	{
		gPBufferSz = prxfile.GetLength();
		pBuffer = new char[gPBufferSz];
		prxfile.SeekToBegin();
		prxfile.Read(pBuffer, gPBufferSz);
		prxfile.Close();
	}

	char toFind[] = { 0x2E, 0x74, 0x65, 0x78, 0x74, 0x00, 0x2E, 0x72, 0x65, 0x6C, 0x61, 0x2E, 0x74, 0x65, 0x78, 0x74 };
	char* found = std::search(pBuffer, pBuffer + gPBufferSz, toFind, toFind + sizeof(toFind));
	if (found > 0)
	{
		gPrxDestroyAddr = (int)(found - pBuffer);
	}

	CFile savefile;
	CString savepath = L"C:" + CString(GetPRXPath().c_str());
	CString savename = savepath + CString(GetPRXName().c_str()) + L"_obf";
	if (savefile.Open(savename + L".sprx", CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary))
	{
		savefile.Write(pBuffer, gPrxDestroyAddr);
		savefile.Close();
	}

	if (found != NULL)
		delete[] pBuffer;
}

void DoAntiIDA(CString filePath)
{
	if (antiIdaCheck->GetCheck()) 
	{
		DestroySectionInfos(filePath);
	}
}

void DoOpenEboot()
{
	CFileDialog fOpenDlg(TRUE, L"prx", L"", OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, L"PS3 EBOOT (EBOOT.BIN)|EBOOT.BIN|", NULL);
	fOpenDlg.m_pOFN->lpstrTitle = L"Select a EBOOT file";
	CFile prxfile;
	if (fOpenDlg.DoModal() == IDOK)
	{
		gLoadedPRXPath = fOpenDlg.GetPathName();
		if (prxfile.Open(gLoadedPRXPath, CFile::modeReadWrite | CFile::typeBinary))
		{
			gPrxFileSize = prxfile.GetLength();
			gPrxBuffer = new char[gPrxFileSize];

			prxfile.Close();
			delete[] gPrxBuffer;
		}
	}
}

void CPowerObfuscatorDlg::LoadPRXButton()
{
	CFileDialog fOpenDlg(TRUE, L"prx", L"", OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, L"Unsigned PRX Plugins (*.prx)|*.prx|", this);
	fOpenDlg.m_pOFN->lpstrTitle = L"Select a PRX file";
	CFile prxfile;
	if (fOpenDlg.DoModal() == IDOK)
	{
		gLoadedPRXPath = fOpenDlg.GetPathName();
		if (prxfile.Open(gLoadedPRXPath, CFile::modeReadWrite | CFile::typeBinary))
		{
			gPrxFileSize = prxfile.GetLength();
			gPrxBuffer = new char[gPrxFileSize];
			LoadPRX(&prxfile, gPrxBuffer, gPrxFileSize);
			prxfile.Close();
			if (!CheckMagic(gPrxBuffer))
			{
				MessageBox(L"Invalid file type! Input must be in PRX format.", L"Error!", MB_ICONERROR);
				delete[] gPrxBuffer;
				return;
			}
		}

		if (FindSectionsInPRX(gLoadedPRXPath, gSections) != 0)
		{
			MessageBox(L"Could not find sections!", L"Error!", MB_ICONERROR);
			return;
		}

		gLoadedSymbols.clear();
		symbolListBox->DeleteAllItems();

		GetSymbolInformation(gLoadedPRXPath, &gLoadedSymbols);
		int index = 0;
		for (symbol sym : gLoadedSymbols)
		{
			symbolListBox->InsertItem(index++, sym.name.c_str());
		}
		gSymbolCount = symbolListBox->GetItemCount();

		protectButton->EnableWindow();

		std::wstring title = L"PowerObfuscator | " + GetPRXName();
		SetWindowText(title.c_str());
	}
}

void CPowerObfuscatorDlg::ProtectPRXButton()
{
	int decryptOffset;
	if (!CheckForLibAPI(gPrxBuffer, &decryptOffset))
	{
		MessageBox(L"Could not find reference to API!", L"Error!", MB_ICONERROR);
		return;
	}

	// pre-save
	DoEncryptFunctions();
	DoEncryptData(decryptOffset);

	CString saveFile = DoWritePRXBufferToFile();

	// post-save
	DoStripPRXFile(saveFile);
	DoFakeSignPRX(saveFile);

	DoAntiIDA((CString)saveFile + L".sprx");

	MessageBox(L"Saved to " + saveFile + L"!", L"Success!", MB_ICONASTERISK);

	protectButton->EnableWindow(0);
}

void CPowerObfuscatorDlg::OnEncryptAllButtonClicked()
{
	bool checked = encryptAllFunctionsCheck->GetCheck();
	symbolListBox->EnableWindow(!checked);
}

void CPowerObfuscatorDlg::OnAntiDebuggerButtonClicked()
{
	bool checked = antiDebuggerCheck->GetCheck();
	loadEbootButton->EnableWindow(checked);
	if (checked)
	{
		MessageBoxW(L"Please select an EBOOT to continue.", L"Select an EBOOT", MB_ICONINFORMATION);
		DoOpenEboot();
	}
}

void CPowerObfuscatorDlg::OnLoadEbootButtonClicked()
{
	DoOpenEboot();
}