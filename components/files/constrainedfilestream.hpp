#ifndef OPENMW_CONSTRAINEDFILESTREAM_H
#define OPENMW_CONSTRAINEDFILESTREAM_H

#include "constrainedfilestreambuf.hpp"
#include "istreamptr.hpp"
#include "streamwithbuffer.hpp"

#include <cstddef>
#include <filesystem>
#include <limits>

namespace Files
{

    /// A file stream constrained to a specific region in the file, specified by the 'start' and 'length' parameters.
    using ConstrainedFileStream = StreamWithBuffer<ConstrainedFileStreamBuf>;

    IStreamPtr openConstrainedFileStream(const std::filesystem::path& filename, std::size_t start = 0,
        std::size_t length = std::numeric_limits<std::size_t>::max());

}

#endif
