#pragma once

#include <string>

namespace FileDialog
{
    enum FileDialogType
    {
        FDTFolder,
        FDTFile
    };

    bool openDialog( std::string const& rootDir,
                     FileDialogType type,
                     std::string filter,
                     std::string & file );
} // namespace FileDialog
