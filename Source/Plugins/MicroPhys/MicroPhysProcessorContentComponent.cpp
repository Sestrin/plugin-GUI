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
                4, 0, 116, 34,
                Justification::centred, true);

    g.setColour (Colours::black);
    g.setFont (Font (Font::getDefaultMonospacedFontName(), 12.00f, Font::plain));
    g.drawText (TRANS("IP Address"),
                4, 58, 84, 20,
                Justification::centred, true);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void MicroPhysProcessorContentComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]


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
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
