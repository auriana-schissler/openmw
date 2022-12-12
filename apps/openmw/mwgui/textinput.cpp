#include "textinput.hpp"

#include <MyGUI_Button.h>
#include <MyGUI_Delegate.h>
#include <MyGUI_EditBox.h>
#include <MyGUI_EventPair.h>
#include <MyGUI_KeyCode.h>
#include <MyGUI_UString.h>
#include <MyGUI_WidgetInput.h>

#include <apps/openmw/mwgui/windowbase.hpp>

#include <components/misc/notnullptr.hpp>

#include "../mwbase/environment.hpp"
#include "../mwbase/windowmanager.hpp"

#include "ustring.hpp"

namespace MyGUI
{
    class Widget;
}

namespace MWGui
{

    TextInputDialog::TextInputDialog()
        : WindowModal("openmw_text_input.layout")
    {
        // Centre dialog
        center();

        getWidget(mTextEdit, "TextEdit");
        mTextEdit->eventEditSelectAccept += newDelegate(this, &TextInputDialog::onTextAccepted);

        MyGUI::Button* okButton;
        getWidget(okButton, "OKButton");
        okButton->eventMouseButtonClick += MyGUI::newDelegate(this, &TextInputDialog::onOkClicked);

        // Make sure the edit box has focus
        MWBase::Environment::get().getWindowManager()->setKeyFocusWidget(mTextEdit);
    }

    void TextInputDialog::setNextButtonShow(bool shown)
    {
        MyGUI::Button* okButton;
        getWidget(okButton, "OKButton");

        if (shown)
            okButton->setCaption(
                toUString(MWBase::Environment::get().getWindowManager()->getGameSettingString("sNext", {})));
        else
            okButton->setCaption(
                toUString(MWBase::Environment::get().getWindowManager()->getGameSettingString("sOK", {})));
    }

    void TextInputDialog::setTextLabel(std::string_view label)
    {
        setText("LabelT", label);
    }

    void TextInputDialog::onOpen()
    {
        WindowModal::onOpen();
        // Make sure the edit box has focus
        MWBase::Environment::get().getWindowManager()->setKeyFocusWidget(mTextEdit);
    }

    // widget controls

    void TextInputDialog::onOkClicked(MyGUI::Widget* _sender)
    {
        if (mTextEdit->getCaption().empty())
        {
            MWBase::Environment::get().getWindowManager()->messageBox("#{sNotifyMessage37}");
            MWBase::Environment::get().getWindowManager()->setKeyFocusWidget(mTextEdit);
        }
        else
            eventDone(this);
    }

    void TextInputDialog::onTextAccepted(MyGUI::Edit* _sender)
    {
        onOkClicked(_sender);

        // To do not spam onTextAccepted() again and again
        MWBase::Environment::get().getWindowManager()->injectKeyRelease(MyGUI::KeyCode::None);
    }

    std::string TextInputDialog::getTextInput() const
    {
        return mTextEdit->getCaption();
    }

    void TextInputDialog::setTextInput(const std::string& text)
    {
        mTextEdit->setCaption(text);
    }

}
