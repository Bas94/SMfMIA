#pragma once

#include <string>

namespace FileDialog
{
/*!
     * \brief openFile is a function which executes
     *        an open dialog for a single file
     * \param defaultDir is the folder which is opened
     * \param filter is the file suffix which ca be
     *        opened by the dialog (e.g. "*.dcm")
     * \param filename is the returned filename,
     *        it will be left unchanged if no file was opened
     * \return true if a file was selected, otherwise false
     */
    bool openFile( std::string const & defaultDir,
                   std::string const & filter,
                   std::string & filename );

    /*!
     * \brief openFolder is a function which executes
     *        an open dialog for a folder
     * \param defaultDir is the folder which is opened
     * \param folderPath is the returned path to the selected folder,
     *        it will be left unchanged if no file was opened
     * \return true if a folder was selected, otherwise false
     */
    bool openFolder( std::string const& defaultDir,
                     std::string & folderPath );
} // namespace FileDialog
