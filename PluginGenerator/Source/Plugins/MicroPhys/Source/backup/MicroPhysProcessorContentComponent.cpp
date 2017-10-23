/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 4.2.1

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright (c) 2015 - ROLI Ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "MicroPhysProcessorContentComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
MicroPhysProcessorContentComponent::MicroPhysProcessorContentComponent ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    addAndMakeVisible (textEditor = new TextEditor ("new text editor"));
    textEditor->setTooltip (TRANS("Enter number between 1000 - 10000"));
    textEditor->setMultiLine (false);
    textEditor->setReturnKeyStartsNewLine (false);
    textEditor->setReadOnly (false);
    textEditor->setScrollbarsShown (true);
    textEditor->setCaretVisible (true);
    textEditor->setPopupMenuEnabled (true);
    textEditor->setText (TRANS("2000"));

    addAndMakeVisible (textEditor2 = new TextEditor ("new text editor"));
    textEditor2->setMultiLine (false);
    textEditor2->setReturnKeyStartsNewLine (false);
    textEditor2->setReadOnly (false);
    textEditor2->setScrollbarsShown (true);
    textEditor2->setCaretVisible (true);
    textEditor2->setPopupMenuEnabled (true);
    textEditor2->setText (TRANS("192"));

    addAndMakeVisible (textEditor3 = new TextEditor ("new text editor"));
    textEditor3->setMultiLine (false);
    textEditor3->setReturnKeyStartsNewLine (false);
    textEditor3->setReadOnly (false);
    textEditor3->setScrollbarsShown (true);
    textEditor3->setCaretVisible (true);
    textEditor3->setPopupMenuEnabled (true);
    textEditor3->setText (TRANS("168"));

    addAndMakeVisible (textEditor4 = new TextEditor ("new text editor"));
    textEditor4->setMultiLine (false);
    textEditor4->setReturnKeyStartsNewLine (false);
    textEditor4->setReadOnly (false);
    textEditor4->setScrollbarsShown (true);
    textEditor4->setCaretVisible (true);
    textEditor4->setPopupMenuEnabled (true);
    textEditor4->setText (TRANS("0"));

    addAndMakeVisible (textEditor5 = new TextEditor ("new text editor"));
    textEditor5->setMultiLine (false);
    textEditor5->setReturnKeyStartsNewLine (false);
    textEditor5->setReadOnly (false);
    textEditor5->setScrollbarsShown (true);
    textEditor5->setCaretVisible (true);
    textEditor5->setPopupMenuEnabled (true);
    textEditor5->setText (TRANS("100"));


    //[UserPreSize]
    //[/UserPreSize]

    setSize (150, 200);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

MicroPhysProcessorContentComponent::~MicroPhysProcessorContentComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    textEditor = nullptr;
    textEditor2 = nullptr;
    textEditor3 = nullptr;
    textEditor4 = nullptr;
    textEditor5 = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void MicroPhysProcessorContentComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.setColour (Colours::black);
    g.setFont (Font (Font::getDefaultMonospacedFontName(), 12.00f, Font::plain));
    g.drawText (TRANS("Sample Rate (Hz)"),
                4, 12, 116, 28,
                Justification::centred, true);

    g.setColour (Colours::black);
    g.setFont (Font (Font::getDefaultMonospacedFontName(), 12.00f, Font::plain));
    g.drawText (TRANS("IP Address"),
                4, 68, 84, 20,
                Justification::centred, true);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void MicroPhysProcessorContentComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    textEditor->setBounds (9, 38, 104, 24);
    textEditor2->setBounds (8, 88, 32, 24);
    textEditor3->setBounds (48, 88, 32, 24);
    textEditor4->setBounds (88, 88, 32, 24);
    textEditor5->setBounds (128, 88, 32, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="MicroPhysProcessorContentComponent"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="150" initialHeight="200">
  <BACKGROUND backgroundColour="ffffff">
    <TEXT pos="4 12 116 28" fill="solid: ff000000" hasStroke="0" text="Sample Rate (Hz)"
          fontname="Default monospaced font" fontsize="12" bold="0" italic="0"
          justification="36"/>
    <TEXT pos="4 68 84 20" fill="solid: ff000000" hasStroke="0" text="IP Address"
          fontname="Default monospaced font" fontsize="12" bold="0" italic="0"
          justification="36"/>
  </BACKGROUND>
  <TEXTEDITOR name="new text editor" id="2074765275e4a960" memberName="textEditor"
              virtualName="" explicitFocusOrder="0" pos="9 38 104 24" tooltip="Enter number between 1000 - 10000"
              initialText="2000" multiline="0" retKeyStartsLine="0" readonly="0"
              scrollbars="1" caret="1" popupmenu="1"/>
  <TEXTEDITOR name="new text editor" id="3e06394d4de3f157" memberName="textEditor2"
              virtualName="" explicitFocusOrder="0" pos="8 88 32 24" initialText="192"
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <TEXTEDITOR name="new text editor" id="ad496cf6d0057ca9" memberName="textEditor3"
              virtualName="" explicitFocusOrder="0" pos="48 88 32 24" initialText="168"
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <TEXTEDITOR name="new text editor" id="d418a2a92c94fb07" memberName="textEditor4"
              virtualName="" explicitFocusOrder="0" pos="88 88 32 24" initialText="0"
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <TEXTEDITOR name="new text editor" id="1b03bf36378d5c0d" memberName="textEditor5"
              virtualName="" explicitFocusOrder="0" pos="128 88 32 24" initialText="100"
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
