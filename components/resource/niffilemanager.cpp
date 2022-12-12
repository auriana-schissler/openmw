#include "niffilemanager.hpp"

#include <memory>

#include <osg/CopyOp>
#include <osg/Object>
#include <osg/Stats>
#include <osg/ref_ptr>

#include <components/resource/resourcemanager.hpp>
#include <components/vfs/manager.hpp>

#include "objectcache.hpp"

namespace Resource
{

    class NifFileHolder : public osg::Object
    {
    public:
        NifFileHolder(const Nif::NIFFilePtr& file)
            : mNifFile(file)
        {
        }
        NifFileHolder(const NifFileHolder& copy, const osg::CopyOp& copyop)
            : mNifFile(copy.mNifFile)
        {
        }

        NifFileHolder() {}

        META_Object(Resource, NifFileHolder)

        Nif::NIFFilePtr mNifFile;
    };

    NifFileManager::NifFileManager(const VFS::Manager* vfs)
        : ResourceManager(vfs)
    {
    }

    NifFileManager::~NifFileManager() {}

    Nif::NIFFilePtr NifFileManager::get(const std::string& name)
    {
        osg::ref_ptr<osg::Object> obj = mCache->getRefFromObjectCache(name);
        if (obj)
            return static_cast<NifFileHolder*>(obj.get())->mNifFile;
        else
        {
            auto file = std::make_shared<Nif::NIFFile>(name);
            Nif::Reader reader(*file);
            reader.parse(mVFS->get(name));
            obj = new NifFileHolder(file);
            mCache->addEntryToObjectCache(name, obj);
            return file;
        }
    }

    void NifFileManager::reportStats(unsigned int frameNumber, osg::Stats* stats) const
    {
        stats->setAttribute(frameNumber, "Nif", mCache->getCacheSize());
    }

}
