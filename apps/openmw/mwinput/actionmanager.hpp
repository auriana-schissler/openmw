#ifndef MWINPUT_ACTIONMANAGER_H
#define MWINPUT_ACTIONMANAGER_H

#include <osg/ref_ptr>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

namespace MWInput
{
    class BindingsManager;

    class ActionManager
    {
    public:
        ActionManager(BindingsManager* bindingsManager,
            osgViewer::ScreenCaptureHandler::CaptureOperation* screenCaptureOperation,
            osg::ref_ptr<osgViewer::Viewer> viewer, osg::ref_ptr<osgViewer::ScreenCaptureHandler> screenCaptureHandler);

        void update(float dt, bool triedToMove);

        void executeAction(int action);

        bool checkAllowedToUseItems() const;

        void toggleMainMenu();
        void toggleSpell();
        void toggleWeapon();
        void toggleInventory();
        void toggleConsole();
        void screenshot();
        void toggleJournal();
        void activate();
        void toggleWalking();
        void toggleSneaking();
        void toggleAutoMove();
        void rest();
        void quickLoad();
        void quickSave();

        void quickKey(int index);
        void showQuickKeysMenu();

        void resetIdleTime();
        float getIdleTime() const { return mTimeIdle; }

        bool isAlwaysRunActive() const { return mAlwaysRunActive; }
        bool isSneaking() const;

        void setAttemptJump(bool enabled) { mAttemptJump = enabled; }

    private:
        void handleGuiArrowKey(int action);

        BindingsManager* mBindingsManager;
        osg::ref_ptr<osgViewer::Viewer> mViewer;
        osg::ref_ptr<osgViewer::ScreenCaptureHandler> mScreenCaptureHandler;
        osgViewer::ScreenCaptureHandler::CaptureOperation* mScreenCaptureOperation;

        bool mAlwaysRunActive;
        bool mAttemptJump;

        float mTimeIdle;
    };
}
#endif
