#include "stateupdater.hpp"

#include <stdexcept>
#include <tuple>
#include <type_traits>

#include <osg/BufferIndexBinding>
#include <osg/BufferObject>
#include <osg/BufferTemplate>
#include <osg/NodeVisitor>
#include <osg/StateAttribute>
#include <osg/StateSet>
#include <osg/Uniform>
#include <osg/ref_ptr>

#include <components/resource/scenemanager.hpp>
#include <components/sceneutil/lightmanager.hpp>

namespace fx
{
    StateUpdater::StateUpdater(bool useUBO)
        : mUseUBO(useUBO)
    {
    }

    void StateUpdater::setDefaults(osg::StateSet* stateset)
    {
        if (mUseUBO)
        {
            osg::ref_ptr<osg::UniformBufferObject> ubo = new osg::UniformBufferObject;

            osg::ref_ptr<osg::BufferTemplate<UniformData::BufferType>> data
                = new osg::BufferTemplate<UniformData::BufferType>();
            data->setBufferObject(ubo);

            osg::ref_ptr<osg::UniformBufferBinding> ubb = new osg::UniformBufferBinding(
                static_cast<int>(Resource::SceneManager::UBOBinding::PostProcessor), data, 0, mData.getGPUSize());

            stateset->setAttributeAndModes(ubb, osg::StateAttribute::ON);
        }
        else
        {
            const auto createUniform = [&](const auto& v) {
                using T = std::decay_t<decltype(v)>;
                std::string name = "omw." + std::string(T::sName);
                stateset->addUniform(new osg::Uniform(name.c_str(), mData.get<T>()));
            };

            std::apply([&](const auto&... v) { (createUniform(v), ...); }, mData.getData());
        }
    }

    void StateUpdater::apply(osg::StateSet* stateset, osg::NodeVisitor* nv)
    {
        if (mUseUBO)
        {
            osg::UniformBufferBinding* ubb = dynamic_cast<osg::UniformBufferBinding*>(
                stateset->getAttribute(osg::StateAttribute::UNIFORMBUFFERBINDING,
                    static_cast<int>(Resource::SceneManager::UBOBinding::PostProcessor)));
            if (!ubb)
                throw std::runtime_error("StateUpdater::apply: failed to get an UniformBufferBinding!");

            auto& dest = static_cast<osg::BufferTemplate<UniformData::BufferType>*>(ubb->getBufferData())->getData();
            mData.copyTo(dest);

            ubb->getBufferData()->dirty();
        }
        else
        {
            const auto setUniform = [&](const auto& v) {
                using T = std::decay_t<decltype(v)>;
                std::string name = "omw." + std::string(T::sName);
                stateset->getUniform(name)->set(mData.get<T>());
            };

            std::apply([&](const auto&... v) { (setUniform(v), ...); }, mData.getData());
        }

        if (mPointLightBuffer)
            mPointLightBuffer->applyUniforms(nv->getTraversalNumber(), stateset);
    }
}
