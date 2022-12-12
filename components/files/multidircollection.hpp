#ifndef COMPONENTS_FILES_MULTIDIRSOLLECTION_HPP
#define COMPONENTS_FILES_MULTIDIRSOLLECTION_HPP

#include <compare>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include <components/misc/strings/algorithm.hpp>

namespace Files
{
    typedef std::vector<std::filesystem::path> PathContainer;

    struct NameLess
    {
        bool mStrict;

        NameLess(bool strict)
            : mStrict(strict)
        {
        }

        bool operator()(const std::string& left, const std::string& right) const
        {
            if (mStrict)
                return left < right;
            return Misc::StringUtils::ciLess(left, right);
        }
    };

    /// \brief File collection across several directories
    ///
    /// This class lists all files with one specific extensions within one or more
    /// directories. If the same file appears more than once, the file in the directory
    /// with the higher priority is used.
    class MultiDirCollection
    {
    public:
        typedef std::map<std::string, std::filesystem::path, NameLess> TContainer;
        typedef TContainer::const_iterator TIter;

    private:
        TContainer mFiles;

    public:
        MultiDirCollection(const Files::PathContainer& directories, const std::string& extension, bool foldCase);
        ///< Directories are listed with increasing priority.
        /// \param extension The extension that should be listed in this collection. Must
        /// contain the leading dot.
        /// \param foldCase Ignore filename case

        std::filesystem::path getPath(const std::string& file) const;
        ///< Return full path (including filename) of \a file.
        ///
        /// If the file does not exist, an exception is thrown. \a file must include
        /// the extension.

        bool doesExist(const std::string& file) const;
        ///< \return Does a file with the given name exist?

        TIter begin() const;
        ///< Return iterator pointing to the first file.

        TIter end() const;
        ///< Return iterator pointing past the last file.
    };
}

#endif
