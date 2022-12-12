#include "base.hpp"

#include <components/nif/nifstream.hpp>
#include <components/nif/recordptr.hpp>

namespace Nif
{
    class Reader;

    void Extra::read(NIFStream* nif)
    {
        if (nif->getVersion() >= NIFStream::generateVersion(10, 0, 1, 0))
            name = nif->getString();
        else if (nif->getVersion() <= NIFStream::generateVersion(4, 2, 2, 0))
        {
            next.read(nif);
            recordSize = nif->getUInt();
        }
    }

    void Named::read(NIFStream* nif)
    {
        name = nif->getString();
        if (nif->getVersion() < NIFStream::generateVersion(10, 0, 1, 0))
            extra.read(nif);
        else
            readRecordList(nif, extralist);
        controller.read(nif);
    }

    void Named::post(Reader& nif)
    {
        extra.post(nif);
        postRecordList(nif, extralist);
        controller.post(nif);
    }
}
