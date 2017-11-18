/*
  ==============================================================================

    SynthComponent.h
    Created: 17 Nov 2017 6:54:56pm
    Author:  Henrique Vermelho

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class SynthComponent    : public Component
{
public:
    SynthComponent();
    ~SynthComponent();

    void paint (Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthComponent)
};
