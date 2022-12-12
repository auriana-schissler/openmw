#include "additivelayer.hpp"

#include <osg/BlendFunc>
#include <osg/StateSet>

#include <MyGUI_RenderManager.h>

#include "myguirendermanager.hpp"

namespace MyGUI
{
    class IRenderTarget;
}

namespace osgMyGUI
{

    AdditiveLayer::AdditiveLayer()
    {
        mStateSet = new osg::StateSet;
        mStateSet->setAttributeAndModes(new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE));
    }

    AdditiveLayer::~AdditiveLayer()
    {
        // defined in .cpp file since we can't delete incomplete types
    }

    void AdditiveLayer::renderToTarget(MyGUI::IRenderTarget* _target, bool _update)
    {
        RenderManager& renderManager = static_cast<RenderManager&>(MyGUI::RenderManager::getInstance());

        renderManager.setInjectState(mStateSet.get());

        MyGUI::OverlappedLayer::renderToTarget(_target, _update);

        renderManager.setInjectState(nullptr);
    }

}
