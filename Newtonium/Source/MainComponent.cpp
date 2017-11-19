#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
//#include "juce_gui_basics\widgets\juce_Slider.h"
//#include "juce_gui_basics\widgets\juce_Label.h"

#include <limits>

//==============================================================================
/*
 This component lives inside our window, and this is where you should put all
 your controls and content.
 */
class SmoothedValue
{
public:
    SmoothedValue(float v, int steps=64): _v(v), _target(v), _delta(0), _ksteps(steps), _nsteps(0)
    {
    }
    ~SmoothedValue()
    {
    }
    
    void setValue(float v)
    {
        _nsteps=0;
        _target = v;
        _v = v;
        _delta = 0;
    }
    
    void setTargetValue(float target)
    {
        _nsteps = _ksteps;
        _target = target;
        _delta = (_target-_v)/(float)_nsteps;
    }
    
    float getValue() const
    {
        return _v;
    }
    
    float getTargetValue() const
    {
        return _target;
    }
    
    void tick()
    {
        if (_nsteps) {
            _v = _target-_delta*(float)_nsteps;
            _nsteps--;
        }
    }
    
    bool isStillSmoothing() const
    {
        return (_nsteps!=0);
    }
private:
    float _target;
    float _delta;
    float _v;
    int _nsteps;
    const int _ksteps;
};

class MainContentComponent : public AudioAppComponent, public Slider::Listener, public TextButton::Listener
{
public:
    //==============================================================================
    MainContentComponent()
    {
        setSize (480, 140);
        
        // specify the number of input and output channels that we want to open
        setAudioChannels (0, 2);
        
        // volume slider
        addAndMakeVisible (volumeSlider);
        volumeSlider.setRange (-96, 6);
        volumeSlider.setTextValueSuffix (" db");
        volumeSlider.setValue (-6);
        volumeSlider.addListener (this);
        //volumeSlider.setSkewFactorFromMidPoint(0.5);
        
        volumeLabel.setText ("Volume", dontSendNotification);
        volumeLabel.attachToComponent (&volumeSlider, true);
        
        // freq slider
        addAndMakeVisible (freqSlider);
        freqSlider.setRange (10, 20000);
        freqSlider.setTextValueSuffix (" Hz");
        freqSlider.setValue (500.0);
        freqSlider.addListener (this);
        freqSlider.setSkewFactorFromMidPoint (500);
        
        freqLabel.setText ("Freq", dontSendNotification);
        freqLabel.attachToComponent (&freqSlider, true);
        
        // mute button
        addAndMakeVisible (muteButton);
        muteButton.setButtonText ("Mute");
        muteButton.addListener (this);
        muteButton.setEnabled (true);
        
        
        
    }
    
    ~MainContentComponent()
    {
        shutdownAudio();
    }
    
    void buttonClicked (Button* button) override
    {
        if (button == &muteButton) {
            mute = !mute;
        }
    }
    
    void sliderValueChanged (Slider *slider) override
    {
        if (slider == &volumeSlider)
        {
            amplitude = pow (10, ((float)volumeSlider.getValue() / 20.0));
            //amplitude = interpolate(amplitude, targetAmplitude, time, (time+deltaTime));

        }
        
        if (slider == &freqSlider)
        {
            targetFrequency = (float)freqSlider.getValue();
            //freqSmooth (frequency, 512);
           // freqSmooth.setTargetValue(targetFrequency);
          //  frequency = freqSmooth.getValue;
            //frequency = interpolate(frequency, targetFrequency, sample, sampleRate);
        }
      
    }
    
    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override
    {
        // This function will be called when the audio device is started, or when
        // its settings (i.e. sample rate, block size, etc) are changed.
        // You can use this function to initialise any resources you might need,
        // but be careful - it will be called on the audio thread, not the GUI thread.
        // For more details, see the help for AudioProcessor::prepareToPlay()
        
        amplitude = 0.5;
        frequency = 500.0;
        targetFrequency = frequency;
        //freqSmooth.setValue (frequency);
       // freqSmooth.setTargetValue (targetFrequency);
        time = 0.0;
        deltaTime = 1 / sampleRate;
        currentSampleRate = sampleRate;
    }
    
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override
    {
        
        if (time >= std::numeric_limits<float>::max())
        {
            time = 0.0;
        }
        
        float *monoBuffer = new float[bufferToFill.numSamples];
        
        if (mute) return bufferToFill.clearActiveBufferRegion();
        
        // generate sin wave in mono
        if (targetFrequency == frequency) //Se a frequência do slider for igual a que queremos, nao precisamos do smoothing.
        {
            for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
            {
                float value = amplitude * sin(2 * double_Pi * frequency * time);
                //printf("penis");
                monoBuffer[sample] = value;
                time += deltaTime;
                //std::cout << "buffer[sample]: " << monoBuffer[sample] << " sample: " << sample << " frequency: " << frequency << std::endl;

            }
        }
        else //interpolation goes here :)
        {
            for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
            {
               
               // frequency = interpolate(sample, bufferToFill.numSamples, frequency, targetFrequency, deltaTime);
                SmoothedValue freqSmooth (frequency, bufferToFill.numSamples);
                freqSmooth.setValue (frequency);
                freqSmooth.setTargetValue (targetFrequency);
                freqSmooth.tick();
                frequency = freqSmooth.getValue();
               // std::cout << "buffer[sample]: " << monoBuffer[sample] << " sample: " << sample << " frequency: " << frequency << std::endl;
               // std::cout << "is smoothing?" << freqSmooth.isStillSmoothing() << std::endl;
                float value = amplitude * sin(2 * double_Pi * freqSmooth.getTargetValue() * time);
                
                monoBuffer[sample] = value;
                time += deltaTime;
            }
            
        }
        
        // iterate over all available output channels
        for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
        {
            // Get a pointer to the start sample in the buffer for this audio output channel
            float* const buffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);
            
            for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
            {
                buffer[sample] = monoBuffer[sample];
                //std::cout << "buffer[sample]: " << buffer[sample] << " sample: " << sample << std::endl;

               // std::cout << "buffer[sample]: " << buffer[sample] << " sample: " << sample << std::endl;
            }
        }
    }
    
    void releaseResources() override
    {
        // This will be called when the audio device stops, or when it is being
        // restarted due to a setting change.
        // For more details, see the help for AudioProcessor::releaseResources()
    }
    
    float interpolate (float sample, float numSamples, float curVal, float targetVal, float deltaTime)
    {
        //float deltaTime = 1/numSamples;
        float x0 = deltaTime * sample;
        float x1 = deltaTime * (sample+1);
        float y0 = curVal;
        float y1 = targetVal;
        float y; //result
        float avg = (x0 + x1)/2;  //um valor entre x0 e x1
        if (y0 == y1) { return y0; }
        else
        {
            y = y0 + ( (avg) - x0 ) * ( (y1 - y0) / (x1 - x0) );
            return y;
        }
        /* Second idea
        float increment = 1/numSamples;
        if(curValue == targetVal)
        {
            curValue = targetVal;
            return curValue;
        }
        else
        {
            increment = (targetVal - curValue) / numSamples;
            curValue += increment;
            return curValue;
        }
        /*
         //Original "idea"
        float avg = (valueX + targetValueX)/2;
        if (valueY == targetValueY) { return valueY; }
        else
        {
            valueY = valueY + ( (avg) - valueX ) * ( (targetValueY - valueY) / (targetValueX - valueX));
        }
        return valueY;
         */
        
    }
    //==============================================================================
    void paint(Graphics& g) override
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll(Colours::darkgrey);
    }
    
    void resized() override
    {
        // This is called when the MainContentComponent is resized.
        // If you add any child components, this is where you should
        // update their positions.
        const int sliderLeft = 50;
        volumeSlider.setBounds(sliderLeft, 20, getWidth() - sliderLeft - 10, 20);
        freqSlider.setBounds(sliderLeft, 50, getWidth() - sliderLeft - 10, 20);
        muteButton.setBounds(10, 110, getWidth() - 20, 20);
    }
    
private:
    //==============================================================================
    
    // Your private member variables go here...
    float currentSampleRate;
    float amplitude, targetAmplitude;
    float frequency, targetFrequency;
    float time;
    float deltaTime;
    
    // GUI
    Slider volumeSlider;
    Slider freqSlider;
    Label volumeLabel;
    Label freqLabel;
    
    TextButton muteButton;
    bool mute;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent() { return new MainContentComponent(); }

#endif  // MAINCOMPONENT_H_INCLUDED

