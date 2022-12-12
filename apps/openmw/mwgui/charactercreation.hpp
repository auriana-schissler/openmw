#ifndef CHARACTER_CREATION_HPP
#define CHARACTER_CREATION_HPP

#include <map>
#include <memory>
#include <string>

#include <apps/openmw/mwmechanics/stat.hpp>

#include <components/esm3/loadclas.hpp>
#include <components/esm3/loadskil.hpp>
#include <vector>

#include "statswatcher.hpp"

namespace osg
{
    class Group;
}

namespace Resource
{
    class ResourceSystem;
}

namespace MWGui
{
    class BirthDialog;
    class ClassChoiceDialog;
    class CreateClassDialog;
    class GenerateClassResultDialog;
    class InfoBoxDialog;
    class PickClassDialog;
    class RaceDialog;
    class ReviewDialog;
    class TextInputDialog;
    class WindowBase;

    class CharacterCreation : public StatsListener
    {
    public:
        typedef std::vector<int> SkillList;

        CharacterCreation(osg::Group* parent, Resource::ResourceSystem* resourceSystem);
        virtual ~CharacterCreation();

        // Show a dialog
        void spawnDialog(const char id);

        void setValue(const std::string& id, const MWMechanics::AttributeValue& value) override;
        void setValue(const std::string& id, const MWMechanics::DynamicStat<float>& value) override;
        void setValue(const ESM::Skill::SkillEnum parSkill, const MWMechanics::SkillValue& value) override;
        void configureSkills(const SkillList& major, const SkillList& minor) override;

        void onFrame(float duration);

    private:
        osg::Group* mParent;
        Resource::ResourceSystem* mResourceSystem;

        SkillList mPlayerMajorSkills, mPlayerMinorSkills;
        std::map<int, MWMechanics::AttributeValue> mPlayerAttributes;
        std::map<int, MWMechanics::SkillValue> mPlayerSkillValues;

        // Dialogs
        std::unique_ptr<TextInputDialog> mNameDialog;
        std::unique_ptr<RaceDialog> mRaceDialog;
        std::unique_ptr<ClassChoiceDialog> mClassChoiceDialog;
        std::unique_ptr<InfoBoxDialog> mGenerateClassQuestionDialog;
        std::unique_ptr<GenerateClassResultDialog> mGenerateClassResultDialog;
        std::unique_ptr<PickClassDialog> mPickClassDialog;
        std::unique_ptr<CreateClassDialog> mCreateClassDialog;
        std::unique_ptr<BirthDialog> mBirthSignDialog;
        std::unique_ptr<ReviewDialog> mReviewDialog;

        // Player data
        std::string mPlayerName;
        std::string mPlayerRaceId;
        std::string mPlayerBirthSignId;
        ESM::Class mPlayerClass;

        // Class generation vars
        unsigned mGenerateClassStep; // Keeps track of current step in Generate Class dialog
        ESM::Class::Specialization mGenerateClassResponses[3];
        unsigned mGenerateClassSpecializations[3]; // A counter for each specialization which is increased when an
                                                   // answer is chosen
        std::string mGenerateClass; // In order: Combat, Magic, Stealth

        ////Dialog events
        // Name dialog
        void onNameDialogDone(WindowBase* parWindow);

        // Race dialog
        void onRaceDialogDone(WindowBase* parWindow);
        void onRaceDialogBack();
        void selectRace();

        // Class dialogs
        void onClassChoice(int _index);
        void onPickClassDialogDone(WindowBase* parWindow);
        void onPickClassDialogBack();
        void onCreateClassDialogDone(WindowBase* parWindow);
        void onCreateClassDialogBack();
        void showClassQuestionDialog();
        void onClassQuestionChosen(int _index);
        void onGenerateClassBack();
        void onGenerateClassDone(WindowBase* parWindow);
        void selectGeneratedClass();
        void selectCreatedClass();
        void selectPickedClass();

        // Birthsign dialog
        void onBirthSignDialogDone(WindowBase* parWindow);
        void onBirthSignDialogBack();
        void selectBirthSign();

        // Review dialog
        void onReviewDialogDone(WindowBase* parWindow);
        void onReviewDialogBack();
        void onReviewActivateDialog(int parDialog);

        enum CSE // Creation Stage Enum
        {
            CSE_NotStarted,
            CSE_NameChosen,
            CSE_RaceChosen,
            CSE_ClassChosen,
            CSE_BirthSignChosen,
            CSE_ReviewBack,
            CSE_ReviewNext
        };

        CSE mCreationStage; // Which state the character creating is in, controls back/next/ok buttons

        void handleDialogDone(CSE currentStage, int nextMode);
    };
}

#endif
