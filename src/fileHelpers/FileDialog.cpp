#include "FileDialog.h"

#include <windows.h>
#include <shlobj.h>
#include <cassert>

namespace FileDialog
{
    // Function that opens a dialoge window to choose a DICOM file
    std::string openFilename( char *filter = "*.dcm", HWND owner = NULL )
    {
        OPENFILENAMEA ofn;
        char fileName[MAX_PATH] = "";
        ZeroMemory(&ofn, sizeof(ofn));

        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = owner;
        ofn.lpstrInitialDir = "";
        ofn.lpstrFilter = filter;
        ofn.lpstrFile = fileName;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;  ofn.lpstrDefExt = "";

        std::string fileNameStr;
        if (GetOpenFileNameA(&ofn))
            fileNameStr = fileName;

        return fileNameStr;
    }

    // Function to browse the folder with the DICOM Series
    // TODO: set another root folder
    std::string openFolder( HWND hwnd = NULL, LPCTSTR szCurrent = NULL, LPTSTR szPath = new TCHAR[MAX_PATH] )
    {
        BROWSEINFO   bi = { 0 };
        LPITEMIDLIST pidl;
        TCHAR        szDisplay[MAX_PATH];

        LPVOID pvReserved = NULL;
        CoInitialize(pvReserved);

        bi.hwndOwner = hwnd;
        bi.pszDisplayName = szDisplay;
        bi.lpszTitle = TEXT("Please choose a folder.");
        bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
        bi.lParam = (LPARAM)szCurrent;

        pidl = SHBrowseForFolder(&bi);

        SHGetPathFromIDList(pidl, szPath);

        CoUninitialize();
        return szPath;
    }

    bool openDialog( std::string const& rootDir,
                     FileDialogType type,
                     std::string filter,
                     std::string & file )
    {
        if( type == FDTFolder )
        {
            files = openFolder( filter );
            return !files.empty();
        }
        else if( type == FDTFile )
        {
            files = openFilename( filter );
            return !files.empty();
        }
        else
        {
            assert( false && "unknown dialog type" );
            std::cerr << "unknown dialog type" << std::endl;
            return false;
        }
    }
} // namespace FileDialog
