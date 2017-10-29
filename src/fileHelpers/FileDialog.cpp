#include "FileDialog.h"

#ifdef SYSTEM_WINDOWS
  #include <windows.h>
  #include <shlobj.h>
#endif
#ifdef SYSTEM_LINUX
  #include <gtk/gtk.h>
  #include <cstring>
  #include <malloc.h>
#endif

#include <iostream>
#include <cassert>

namespace FileDialog
{
#ifdef SYSTEM_WINDOWS
    // Function that opens a dialoge window to choose a file
    bool openFilen( std::string const & filterList,
                    std::string const & defaultPath,
                    std::string & outPath )
    {
        OPENFILENAMEA ofn;
        char fileName[MAX_PATH] = "";
        ZeroMemory(&ofn, sizeof(ofn));

        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = NULL;
        ofn.lpstrInitialDir = defaultPath.c_str();
        ofn.lpstrFilter = filter.c_str();
        ofn.lpstrFile = fileName;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;  
		ofn.lpstrDefExt = "";

		if(!GetOpenFileNameA(&ofn))
			return false;
		
		outPath = fileName;
		
        return true;
    }

	static int CALLBACK browseFolderCallback( HWND hwnd, 
											  UINT uMsg, 
											  LPARAM lParam, 
											  LPARAM lpData )
	{
		if (uMsg == BFFM_INITIALIZED)
		{
			LPCTSTR path = reinterpret_cast<LPCTSTR>(lpData);
			::SendMessage(hwnd, BFFM_SETSELECTION, true, (LPARAM)path);
		}
		return 0;
	}

    // Function to browse the folder with the DICOM Series
    bool openFolder( std::string const & defaultPath,
                     std::string& outPath )
    {
        BROWSEINFO   bi = { 0 };
        LPITEMIDLIST pidl;
        TCHAR        szDisplay[MAX_PATH];

        LPVOID pvReserved = NULL;
        CoInitialize(pvReserved);

        bi.hwndOwner = NULL;
        bi.pszDisplayName = szDisplay;
        bi.lpszTitle = TEXT("Please choose a folder.");
        bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
		bi.lpfn = browseFolderCallback;
		if (defaultPath.empty())
		{
			bi.lParam = NULL;
		}
		else
		{
			bi.lParam = (LPARAM)(defaultPath.c_str());
		}

        pidl = SHBrowseForFolder(&bi);

		std::string result;
		LPTSTR szPath = new TCHAR[MAX_PATH];
		if (!SHGetPathFromIDList(pidl, szPath))
			return false;

		result = std::string(szPath);
		delete[] szPath;
        
		CoUninitialize();

        return true;
    }
#endif

#ifdef SYSTEM_LINUX
    static void WaitForCleanup(void)
    {
        while (gtk_events_pending())
            gtk_main_iteration();
    }

    static void AddTypeToFilterName( const char *typebuf, char *filterName, size_t bufsize )
    {
        const char SEP[] = ", ";

        size_t len = strlen(filterName);
        if ( len != 0 )
        {
            strncat( filterName, SEP, bufsize - len - 1 );
            len += strlen(SEP);
        }

        strncat( filterName, typebuf, bufsize - len - 1 );
    }

    static bool NFDi_IsFilterSegmentChar( char ch )
    {
        return (ch==','||ch==';'||ch=='\0');
    }

    static void AddFiltersToDialog( GtkWidget *dialog, const char *filterList )
    {
        GtkFileFilter *filter;
        char typebuf[4096] = {0};
        const char *p_filterList = filterList;
        char *p_typebuf = typebuf;
        char filterName[4096] = {0};

        if ( !filterList || strlen(filterList) == 0 )
            return;

        filter = gtk_file_filter_new();
        while ( 1 )
        {

            if ( NFDi_IsFilterSegmentChar(*p_filterList) )
            {
                char typebufWildcard[4096];
                /* add another type to the filter */
                assert( strlen(typebuf) > 0 );
                assert( strlen(typebuf) < 4096-1 );

                snprintf( typebufWildcard, 4096, "*.%s", typebuf );
                AddTypeToFilterName( typebuf, filterName, 4096 );

                gtk_file_filter_add_pattern( filter, typebufWildcard );

                p_typebuf = typebuf;
                memset( typebuf, 0, sizeof(char) * 4096 );
            }

            if ( *p_filterList == ';' || *p_filterList == '\0' )
            {
                /* end of filter -- add it to the dialog */

                gtk_file_filter_set_name( filter, filterName );
                gtk_file_chooser_add_filter( GTK_FILE_CHOOSER(dialog), filter );

                filterName[0] = '\0';

                if ( *p_filterList == '\0' )
                    break;

                filter = gtk_file_filter_new();
            }

            if ( !NFDi_IsFilterSegmentChar( *p_filterList ) )
            {
                *p_typebuf = *p_filterList;
                p_typebuf++;
            }

            p_filterList++;
        }

        /* always append a wildcard option to the end*/

        filter = gtk_file_filter_new();
        gtk_file_filter_set_name( filter, "*.*" );
        gtk_file_filter_add_pattern( filter, "*" );
        gtk_file_chooser_add_filter( GTK_FILE_CHOOSER(dialog), filter );
    }

    static void SetDefaultPath( GtkWidget *dialog, const char *defaultPath )
    {
        if ( !defaultPath || strlen(defaultPath) == 0 )
            return;

        /* GTK+ manual recommends not specifically setting the default path.
           We do it anyway in order to be consistent across platforms.
           If consistency with the native OS is preferred, this is the line
           to comment out. -ml */
        gtk_file_chooser_set_current_folder( GTK_FILE_CHOOSER(dialog), defaultPath );
    }

    bool openFilen( std::string const & filterList,
                    std::string const & defaultPath,
                    std::string & outPath )
    {
        GtkWidget *dialog;
        bool result;

        if ( !gtk_init_check( NULL, NULL ) )
        {
            return false;
        }

        dialog = gtk_file_chooser_dialog_new( "Open File",
                                              NULL,
                                              GTK_FILE_CHOOSER_ACTION_OPEN,
                                              "_Cancel", GTK_RESPONSE_CANCEL,
                                              "_Open", GTK_RESPONSE_ACCEPT,
                                              NULL );

        /* Build the filter list */
        AddFiltersToDialog(dialog, filterList.c_str() );

        /* Set the default path */
        SetDefaultPath(dialog, defaultPath.c_str() );

        result = false;
        if ( gtk_dialog_run( GTK_DIALOG(dialog) ) == GTK_RESPONSE_ACCEPT )
        {
            char *filename;

            filename = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(dialog) );

            {
                outPath = std::string( filename );
                if ( outPath.empty() )
                {
                    g_free( filename );
                    gtk_widget_destroy(dialog);
                    return false;
                }
            }
            g_free( filename );

            result = true;
        }

        WaitForCleanup();
        gtk_widget_destroy(dialog);
        WaitForCleanup();

        return result;
    }

    bool openFolder( std::string const & defaultPath,
                     std::string& outPath )
    {
        GtkWidget *dialog;
        bool result;

        if (!gtk_init_check(NULL, NULL))
        {
            return false;
        }

        dialog = gtk_file_chooser_dialog_new( "Select folder",
                                              NULL,
                                              GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                              "_Cancel", GTK_RESPONSE_CANCEL,
                                              "_Select", GTK_RESPONSE_ACCEPT,
                                              NULL );
        gtk_file_chooser_set_do_overwrite_confirmation( GTK_FILE_CHOOSER(dialog), TRUE );


        /* Set the default path */
        SetDefaultPath( dialog, defaultPath.c_str() );

        result = false;
        if ( gtk_dialog_run( GTK_DIALOG(dialog) ) == GTK_RESPONSE_ACCEPT )
        {
            char *filename = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(dialog) );

            {
                outPath = std::string( filename );
                if ( outPath.empty() )
                {
                    g_free( filename );
                    gtk_widget_destroy(dialog);
                    return false;
                }
            }
            g_free(filename);

            result = true;
        }

        WaitForCleanup();
        gtk_widget_destroy(dialog);
        WaitForCleanup();

        return result;
    }
#endif
} // namespace FileDialog
