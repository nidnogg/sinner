/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
//*/
#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "SynthComponent.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public AudioAppComponent,
                               public Slider::Listener
{
public:
    //==============================================================================
    MainContentComponent()      //  A clase principial é instanciada herdando os atributos a seguir,
    :   currentSampleRate(0.0), //  que são fundamentais para síntese da onda seno.
        currentAngle(0.0),      //  A sampleRate é a taxa de samples (unidade discreta num ponto do espaço | tempo) por segundo - 44.100hz
        angleDelta(0.0)         //  Quanto o ângulo irá variar
    {
       //addAndMakeVisible (synth);

        addAndMakeVisible (frequencySlider);
        frequencySlider.setRange (50.0, 5000.0);
        frequencySlider.setSkewFactorFromMidPoint (500.0); // [4]
        
        frequencySlider.addListener (this);
        
        addAndMakeVisible (levelSlider);
        levelSlider.setRange (0.0, 0.25);
        levelSlider.addListener (this);
        
        setSize (600, 300);
        setAudioChannels (0, 2);
    }

    ~MainContentComponent()
    {
        shutdownAudio();
    }

    //==============================================================================
    /* 
        Funções para onda de seno/
     */
    
    void updateAngleDelta()
    {
        const double cyclesPerSample = frequencySlider.getValue() / currentSampleRate; // the frequency is the number of cycles in each   output sample
        angleDelta = cyclesPerSample * 2.0 * double_Pi;        //2piRAD (1 sine cycle) * number of cycles for each output sample
    }
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        std::cout << "Booting Newtonium\n";
        std::cout << getWidth() << "vs ParentWidth (Parent)" << getParentWidth();
        
        currentSampleRate = sampleRate;
        updateAngleDelta();
    }

    void sliderValueChanged (Slider* slider) override
    {
        if (slider == &frequencySlider)
        {
            if (currentSampleRate > 0.0)
                updateAngleDelta();
        }
    }
    
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        
        const float level = levelSlider.getValue();
        float* const buffer = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);
        //only one channel, no need for double fors
        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            const float currentSample = (float) std::sin (currentAngle);
            currentAngle += angleDelta;
            buffer[sample] = currentSample * level;
        }
        
        // Clear the buffer to prevent the output of random noise
         //bufferToFill.clearActiveBufferRegion();
         
    }

    void releaseResources() override
    {
        // This will be called when the audio device stops, or when it is being
        // restarted due to a setting change.
        // For more details, see the help for AudioProcessor::releaseResources()
    }

    //==============================================================================
    void paint (Graphics& g) override
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
      //  g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
        g.fillAll (Colours::darkgrey);
    }

    void resized() override
    {
        // This is called when the MainContentComponent is resized.
        // If you add any child components, this is where you should
        // update their positions.
        //setBounds(0, 0, getWidth()/2, getHeight()/2);
        frequencySlider.setBounds (10, 10, getWidth() - 20, 20);
        levelSlider.setBounds (10, 30, getWidth() - 20, 20);
    }


private:
    //==============================================================================

    // Your private member variables go here...
    Slider frequencySlider, levelSlider;
 //   SmoothedValue<float> sine(0.0f, (int) sampleRate));
    double currentSampleRate, currentAngle, angleDelta; // [1]
    SynthComponent synth;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
/*
template<class T>
class SmoothedValue
{
public:
    SmoothedValue(T v, int steps=64): _v(v), _target(v), _delta(0), ;):(steps), _nsteps(0)
    {
    }
    ~SmoothedValue()
    {
    }
    
    void setValue(T v)
    {
        _nsteps=0;
        _target = v;
        _v = v;
        _delta = 0;
    }
    
    void setTargetValue(T target)
    {
        _nsteps = _ksteps;
        _target = target;
        _delta = (_target-_v)/(T)_nsteps;
    }
    
    T getValue() const
    {
        return _v;
    }
    
    T getTargetValue() const
    {
        return _target;
    }
    
    void tick()
    {
        if (_nsteps) {
            _v = _target-_delta*(T)_nsteps;
            _nsteps--;
        }
    }
    
    bool isStillSmoothing() const
    {
        return (_nsteps!=0);
    }
private:
    T _target;
    T _delta;
    T _v;
    int _nsteps;
    const int _ksteps;
};
*/
// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
