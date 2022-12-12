#include "controller.hpp"

#include <memory>
#include <stddef.h>

#include <components/nif/base.hpp>
#include <components/nif/niffile.hpp>
#include <components/nif/nifstream.hpp>

#include "controlled.hpp"
#include "data.hpp"
#include "node.hpp"
#include "recordptr.hpp"

namespace Nif
{

    void Controller::read(NIFStream* nif)
    {
        next.read(nif);

        flags = nif->getUShort();

        frequency = nif->getFloat();
        phase = nif->getFloat();
        timeStart = nif->getFloat();
        timeStop = nif->getFloat();

        target.read(nif);
    }

    void Controller::post(Reader& nif)
    {
        Record::post(nif);
        next.post(nif);
        target.post(nif);
    }

    void NiInterpController::read(NIFStream* nif)
    {
        Controller::read(nif);
        if (nif->getVersion() >= NIFStream::generateVersion(10, 1, 0, 104)
            && nif->getVersion() <= NIFStream::generateVersion(10, 1, 0, 108))
            mManagerControlled = nif->getBoolean();
    }

    void NiSingleInterpController::read(NIFStream* nif)
    {
        NiInterpController::read(nif);
        if (nif->getVersion() >= NIFStream::generateVersion(10, 1, 0, 104))
            mInterpolator.read(nif);
    }

    void NiSingleInterpController::post(Reader& nif)
    {
        NiInterpController::post(nif);
        mInterpolator.post(nif);
    }

    void NiParticleSystemController::read(NIFStream* nif)
    {
        Controller::read(nif);

        velocity = nif->getFloat();
        velocityRandom = nif->getFloat();
        verticalDir = nif->getFloat();
        verticalAngle = nif->getFloat();
        horizontalDir = nif->getFloat();
        horizontalAngle = nif->getFloat();
        /*normal?*/ nif->getVector3();
        color = nif->getVector4();
        size = nif->getFloat();
        startTime = nif->getFloat();
        stopTime = nif->getFloat();
        nif->getChar();
        emitRate = nif->getFloat();
        lifetime = nif->getFloat();
        lifetimeRandom = nif->getFloat();

        emitFlags = nif->getUShort();
        offsetRandom = nif->getVector3();

        emitter.read(nif);

        /* Unknown Short, 0?
         * Unknown Float, 1.0?
         * Unknown Int, 1?
         * Unknown Int, 0?
         * Unknown Short, 0?
         */
        nif->skip(16);

        numParticles = nif->getUShort();
        activeCount = nif->getUShort();

        particles.resize(numParticles);
        for (size_t i = 0; i < particles.size(); i++)
        {
            particles[i].velocity = nif->getVector3();
            nif->getVector3(); /* unknown */
            particles[i].lifetime = nif->getFloat();
            particles[i].lifespan = nif->getFloat();
            particles[i].timestamp = nif->getFloat();
            nif->getUShort(); /* unknown */
            particles[i].vertex = nif->getUShort();
        }

        nif->getUInt(); /* -1? */
        affectors.read(nif);
        colliders.read(nif);
        nif->getChar();
    }

    void NiParticleSystemController::post(Reader& nif)
    {
        Controller::post(nif);
        emitter.post(nif);
        affectors.post(nif);
        colliders.post(nif);
    }

    void NiMaterialColorController::read(NIFStream* nif)
    {
        NiPoint3InterpController::read(nif);
        // Two bits that correspond to the controlled material color.
        // 00: Ambient
        // 01: Diffuse
        // 10: Specular
        // 11: Emissive
        if (nif->getVersion() >= NIFStream::generateVersion(10, 1, 0, 0))
            mTargetColor = nif->getUShort() & 3;
        else
            mTargetColor = (flags >> 4) & 3;
        if (nif->getVersion() <= NIFStream::generateVersion(10, 1, 0, 103))
            mData.read(nif);
    }

    void NiMaterialColorController::post(Reader& nif)
    {
        NiPoint3InterpController::post(nif);
        mData.post(nif);
    }

    void NiLookAtController::read(NIFStream* nif)
    {
        Controller::read(nif);
        if (nif->getVersion() >= NIFStream::generateVersion(10, 1, 0, 0))
            lookAtFlags = nif->getUShort();
        target.read(nif);
    }

    void NiLookAtController::post(Reader& nif)
    {
        Controller::post(nif);
        target.post(nif);
    }

    void NiPathController::read(NIFStream* nif)
    {
        Controller::read(nif);

        bankDir = nif->getInt();
        maxBankAngle = nif->getFloat();
        smoothing = nif->getFloat();
        followAxis = nif->getShort();
        posData.read(nif);
        floatData.read(nif);
    }

    void NiPathController::post(Reader& nif)
    {
        Controller::post(nif);

        posData.post(nif);
        floatData.post(nif);
    }

    void NiUVController::read(NIFStream* nif)
    {
        Controller::read(nif);

        uvSet = nif->getUShort();
        data.read(nif);
    }

    void NiUVController::post(Reader& nif)
    {
        Controller::post(nif);
        data.post(nif);
    }

    void NiKeyframeController::read(NIFStream* nif)
    {
        NiSingleInterpController::read(nif);
        if (nif->getVersion() <= NIFStream::generateVersion(10, 1, 0, 103))
            mData.read(nif);
    }

    void NiKeyframeController::post(Reader& nif)
    {
        NiSingleInterpController::post(nif);
        mData.post(nif);
    }

    void NiMultiTargetTransformController::read(NIFStream* nif)
    {
        NiInterpController::read(nif);
        size_t numTargets = nif->getUShort();
        std::vector<NodePtr> targets;
        targets.resize(numTargets);
        for (size_t i = 0; i < targets.size(); i++)
            targets[i].read(nif);
        mExtraTargets = targets;
    }

    void NiMultiTargetTransformController::post(Reader& nif)
    {
        NiInterpController::post(nif);
        postRecordList(nif, mExtraTargets);
    }

    void NiAlphaController::read(NIFStream* nif)
    {
        NiFloatInterpController::read(nif);
        if (nif->getVersion() <= NIFStream::generateVersion(10, 1, 0, 103))
            mData.read(nif);
    }

    void NiAlphaController::post(Reader& nif)
    {
        NiFloatInterpController::post(nif);
        mData.post(nif);
    }

    void NiRollController::read(NIFStream* nif)
    {
        NiSingleInterpController::read(nif);
        if (nif->getVersion() <= NIFStream::generateVersion(10, 1, 0, 103))
            mData.read(nif);
    }

    void NiRollController::post(Reader& nif)
    {
        NiSingleInterpController::post(nif);
        mData.post(nif);
    }

    void NiGeomMorpherController::read(NIFStream* nif)
    {
        NiInterpController::read(nif);
        if (nif->getVersion() >= NIFFile::NIFVersion::VER_OB_OLD)
            mUpdateNormals = nif->getUShort() & 1;
        mData.read(nif);
        if (nif->getVersion() >= NIFFile::NIFVersion::VER_MW)
        {
            mAlwaysActive = nif->getChar();
            if (nif->getVersion() >= NIFStream::generateVersion(10, 1, 0, 106))
            {
                if (nif->getVersion() <= NIFFile::NIFVersion::VER_OB)
                {
                    readRecordList(nif, mInterpolators);
                    if (nif->getVersion() >= NIFStream::generateVersion(10, 2, 0, 0) && nif->getBethVersion() > 9)
                    {
                        unsigned int numUnknown = nif->getUInt();
                        nif->skip(4 * numUnknown);
                    }
                }
                else
                {
                    std::vector<NiInterpolatorPtr> interpolators;
                    size_t numInterps = nif->getUInt();
                    interpolators.resize(numInterps);
                    mWeights.resize(numInterps);
                    for (size_t i = 0; i < numInterps; i++)
                    {
                        interpolators[i].read(nif);
                        mWeights[i] = nif->getFloat();
                    }
                    mInterpolators = interpolators;
                }
            }
        }
    }

    void NiGeomMorpherController::post(Reader& nif)
    {
        NiInterpController::post(nif);
        mData.post(nif);
        postRecordList(nif, mInterpolators);
    }

    void NiVisController::read(NIFStream* nif)
    {
        NiBoolInterpController::read(nif);
        if (nif->getVersion() <= NIFStream::generateVersion(10, 1, 0, 103))
            mData.read(nif);
    }

    void NiVisController::post(Reader& nif)
    {
        NiBoolInterpController::post(nif);
        mData.post(nif);
    }

    void NiFlipController::read(NIFStream* nif)
    {
        NiFloatInterpController::read(nif);
        mTexSlot = nif->getUInt();
        if (nif->getVersion() <= NIFStream::generateVersion(10, 1, 0, 103))
        {
            timeStart = nif->getFloat();
            mDelta = nif->getFloat();
        }
        readRecordList(nif, mSources);
    }

    void NiFlipController::post(Reader& nif)
    {
        NiFloatInterpController::post(nif);
        postRecordList(nif, mSources);
    }

    void bhkBlendController::read(NIFStream* nif)
    {
        Controller::read(nif);
        nif->getUInt(); // Zero
    }

    void NiControllerManager::read(NIFStream* nif)
    {
        Controller::read(nif);
        mCumulative = nif->getBoolean();
        unsigned int numSequences = nif->getUInt();
        nif->skip(4 * numSequences); // Controller sequences
        nif->skip(4); // Object palette
    }

    void NiPoint3Interpolator::read(NIFStream* nif)
    {
        defaultVal = nif->getVector3();
        data.read(nif);
    }

    void NiPoint3Interpolator::post(Reader& nif)
    {
        data.post(nif);
    }

    void NiBoolInterpolator::read(NIFStream* nif)
    {
        defaultVal = nif->getBoolean();
        data.read(nif);
    }

    void NiBoolInterpolator::post(Reader& nif)
    {
        data.post(nif);
    }

    void NiFloatInterpolator::read(NIFStream* nif)
    {
        defaultVal = nif->getFloat();
        data.read(nif);
    }

    void NiFloatInterpolator::post(Reader& nif)
    {
        data.post(nif);
    }

    void NiTransformInterpolator::read(NIFStream* nif)
    {
        defaultPos = nif->getVector3();
        defaultRot = nif->getQuaternion();
        defaultScale = nif->getFloat();
        if (nif->getVersion() <= NIFStream::generateVersion(10, 1, 0, 109))
        {
            if (!nif->getBoolean())
                defaultPos = osg::Vec3f();
            if (!nif->getBoolean())
                defaultRot = osg::Quat();
            if (!nif->getBoolean())
                defaultScale = 1.f;
        }
        data.read(nif);
    }

    void NiTransformInterpolator::post(Reader& nif)
    {
        data.post(nif);
    }

    void NiColorInterpolator::read(NIFStream* nif)
    {
        defaultVal = nif->getVector4();
        data.read(nif);
    }

    void NiColorInterpolator::post(Reader& nif)
    {
        data.post(nif);
    }

}
