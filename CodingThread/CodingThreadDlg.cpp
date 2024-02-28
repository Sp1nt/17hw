#include "CodingThreadDlg.h"

CodingThreadDlg* CodingThreadDlg::ptr = NULL;
HANDLE hMutex;

CodingThreadDlg::CodingThreadDlg(void)
{
    ptr = this;
    hDialog = NULL;
}

CodingThreadDlg::~CodingThreadDlg(void)
{

}

void CodingThreadDlg::Cls_OnClose(HWND hwnd)
{
    ReleaseMutex(hMutex);
    EndDialog(hwnd, 0);
}

DWORD WINAPI Coding_Thread(LPVOID lp)
{
    CodingThreadDlg* ptr = (CodingThreadDlg*)lp;
    char buf[100];
    std::ifstream in("music.txt", std::ios::in | std::ios::binary);

    if (!in)
    {
        MessageBoxA(ptr->hDialog, "Ошибка открытия файла!", "Мьютекс", MB_OK | MB_ICONINFORMATION);
        return 1;
    }

    hMutex = OpenMutex(MUTEX_ALL_ACCESS, false, L"{B8A2C367-10FE-494d-A869-841B2AF972E0}");

    DWORD dwAnswer = WaitForSingleObject(hMutex, INFINITE);

    std::ofstream out("coding.txt", std::ios::out | std::ios::trunc | std::ios::binary);
    const int KEY = 100;
    while (!in.eof())
    {
        in.getline(buf, 100);
        out << buf << std::endl;
    }

    out.close();
    in.close();
    ReleaseMutex(hMutex);

    return 0;
}

DWORD WINAPI Thread1(LPVOID lp)
{
    CodingThreadDlg* ptr = (CodingThreadDlg*)lp;
    HWND hEdit = GetDlgItem(ptr->hDialog, IDC_EDIT1);
    HWND hButton = GetDlgItem(ptr->hDialog, IDC_BUTTON1);
    char buf[100];
    std::ifstream in("music.txt", std::ios::in | std::ios::binary);

    if (!in)
    {
        MessageBoxA(ptr->hDialog, "Ошибка открытия файла!", "Мьютекс", MB_OK | MB_ICONINFORMATION);
        return 1;
    }

    hMutex = OpenMutex(MUTEX_ALL_ACCESS, false, L"{0D68E69C-F6D7-456B-ADE5-8277ED004AE3}");

    DWORD dwAnswer = WaitForSingleObject(hMutex, INFINITE);

    std::ofstream out("coding.txt", std::ios::out | std::ios::trunc | std::ios::binary);

    while (!in.eof())
    {
        in.getline(buf, 100);
        out << buf << std::endl;

        int len = MultiByteToWideChar(CP_UTF8, 0, buf, -1, NULL, 0);
        wchar_t* wide_buf = new wchar_t[len];
        MultiByteToWideChar(CP_UTF8, 0, buf, -1, wide_buf, len);

        SendMessageW(hEdit, EM_REPLACESEL, 0, (LPARAM)wide_buf);
        SendMessageW(hEdit, EM_REPLACESEL, 0, (LPARAM)L"\r\n");

        delete[] wide_buf;
    }

    out.close();
    in.close();

    return 0;
}

BOOL CodingThreadDlg::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    hDialog = hwnd;
    CreateThread(NULL, 0, Coding_Thread, this, 0, NULL);
    CreateThread(NULL, 0, Thread1, this, 0, NULL);
    return TRUE;
}

BOOL CALLBACK CodingThreadDlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
        HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_BUTTON1) {
            ReleaseMutex(hMutex);
            EndDialog(hwnd, 0);
        }
        break;
    }
    return FALSE;
}