#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <limits>

//==============================================================================
/*
 MainWindow > MainComponent
 */

class Customized : public LookAndFeel_V4
{
public:
    Customized()
    {
        setColour (Slider::thumbColourId, Colours::red);
    }
    
    void drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override
    {
        const float radius = jmin (width / 2, height / 2) - 4.0f;
        const float centreX = x + width * 0.5f;
        const float centreY = y + height * 0.5f;
        const float rx = centreX - radius;
        const float ry = centreY - radius;
        const float rw = radius * 2.0f;
        const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        
        g.setColour (Colours::orange);
        g.fillEllipse (rx, ry, rw, rw);
        
        g.setColour (Colours::red);
        g.drawEllipse (rx, ry, rw, rw, 1.0f);
        
        Path p;
        const float pointerLength = radius * 0.33f;
        const float pointerThickness = 2.0f;
        p.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform (AffineTransform::rotation (angle).translated (centreX, centreY));
        g.setColour (Colours::yellow);
        g.fillPath (p);
        
    }
};
class MainContentComponent : public AudioAppComponent, public Slider::Listener, public TextButton::Listener
{
public:
    //==============================================================================
    MainContentComponent()

    {
        setSize (600, 300);
        
        //Two output channels
        setAudioChannels (0, 2);
        
        setLookAndFeel (&visual);
        //getLookAndFeel().setColour (Slider::thumbColourId, Colours::red);
        //volumeSlider.setLookAndFeel (&visual);
        
        //Volume Slider
        addAndMakeVisible (volumeSlider);
        volumeSlider.setRange (-96, 6);
        volumeSlider.setTextValueSuffix (" db");
        volumeSlider.setValue (-6);
        volumeSlider.addListener (this);
        volumeSlider.setSkewFactorFromMidPoint(0.5);
        volumeLabel.setText ("Volume", dontSendNotification);
        volumeLabel.attachToComponent (&volumeSlider, true);
        
        
        //Frequency Slider
        addAndMakeVisible (freqSlider);
        freqSlider.setRange (10, 20000);
        freqSlider.setTextValueSuffix (" Hz");
        freqSlider.setValue (500.0);
        freqSlider.addListener (this);
        freqSlider.setSkewFactorFromMidPoint (500);
        freqLabel.setText ("Freq", dontSendNotification);
        freqLabel.attachToComponent (&freqSlider, true);
        
        
        //Mute Button
        addAndMakeVisible (muteButton);
        muteButton.setButtonText ("Mute");
        muteButton.addListener (this);
        muteButton.setEnabled (true);
        
        //Interpolate Button
        addAndMakeVisible (interpolateButton);
        interpolateButton.setButtonText ("Interpolate");
        interpolateButton.addListener (this);
        interpolateButton.setEnabled (true);
        
        //Effect Button
        addAndMakeVisible (effectButton);
        effectButton.setButtonText ("Mystery Effect");
        effectButton.addListener (this);
        effectButton.setEnabled (true);
        
        setWantsKeyboardFocus (true);
        
        
    }
    
    ~MainContentComponent()
    {
        shutdownAudio();
    }
    
    float interpolate (float currentValue, float targetValue, float interval)
    {
    
        float x0, x1;
        
        x0 = currentValue;
        x1 = targetValue;
        
        return x0 * (1 - interval) + x1 * interval;
        
    }
    
    bool keyPressed (const KeyPress &k) override
    {
        if (k.getTextCharacter() == 'q')
            printf("q\n");
        if (k.getTextCharacter() == 'w')
            printf("w\n");
        if (k.getTextCharacter() == 'e')
            printf("e\n");
        if (k.getTextCharacter() == 'r')
            printf("r\n");
        if (k.getTextCharacter() == 't')
            printf("t\n");
        if (k.getTextCharacter() == 'y')
            printf("y\n");
        
        
    }
    
    void buttonClicked (Button* button) override
    {
        //Only two buttons here, mute and interpolation for their respective toggling.
        if (button == &muteButton)
        {
            mute = !mute;
            std::cout << "Mute: " << mute << std::endl;

        }
        if (button == &interpolateButton)
        {
            interpolating = !interpolating; //starts as 1
            std::cout << "Interpolating: " << interpolating << std::endl;

        }
        if (button == &effectButton)
        {
            effect = !effect;
            std::cout << "Effect: " << effect << std::endl;
        }
        
    }
    
    void sliderValueChanged (Slider *slider) override
    {
        if (slider == &volumeSlider)
        {
            //Amplitude is given in the db
            targetAmplitude = pow (10, ((float)volumeSlider.getValue() / 20.0));

        }
        
        if (slider == &freqSlider)
        {
            if (!interpolating )
            {
                targetFrequency = (float)freqSlider.getValue();
            }
            else
            {
                targetFrequency = (float)freqSlider.getValue();
            }
         
        }
    
      
    }
    
    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        // This function will be called when the audio device is started, or when
        // its settings (i.e. sample rate, block size, etc) are changed.
        // You can use this function to initialise any resources you might need,
        // but be careful - it will be called on the audio thread, not the GUI thread.
        // For more details, see the help for AudioProcessor::prepareToPlay()
        
        amplitude = 0.5;
        targetAmplitude = amplitude;
        frequency = 500.0;
        targetFrequency = frequency;
        time = 0.0;
        phase = 0.0;
        timeDelta = 1 / sampleRate;
        currentSampleRate = sampleRate;
        interpolationLength = currentSampleRate * 0.100;
        //for TESTEXT
        angle  = 0.0;
        
    }
    
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
       // freopen ("~/Google Drive/@2017-2/sinner/Reference/data.txt", "w", stdout);
       // String filepath = "~/Google Drive/@2017-2/newtonium/Documentation/data.txt";
       // File f (filepath);
        
        //Loops time if it gets to numeric limits
        if (time >= std::numeric_limits<float>::max())
        {
            time = 0.0;
        }
        
        const double localTargetFrequency = targetFrequency;
        double sampleFail = 0.0;
    
        //First the samples are processed in a monoBuffer, and then later iterated in a second for loop for channels,
        //where each monoBuffer[sample] is assigned to the iterating channel's current buffer[sample] .
        float *monoBuffer = new float[bufferToFill.numSamples];
    
        if (mute) return bufferToFill.clearActiveBufferRegion();
    
        if (!interpolating && !effect)
        {
            for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
            {
                sampleFail = targetAmplitude * std::sin(2 * double_Pi * localTargetFrequency * time);
                monoBuffer[sample] = sampleFail;
                time += timeDelta;
                
                //Diagnostics
                //printf ("regular sample: (%f %f)\n", sampleFail, time);
                //std::cout << "Does file exist lel" << f.exists() << std::endl;
                //printf("frequency %f\n", localTargetFrequency);
                //printf("amplitude %f\n", targetAmplitude);
            }
        }
        else if (interpolating && (effect == false))
        {
            //no need to smooth if it's the same
            if (localTargetFrequency == frequency)
            {
                
                for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
                {
                    const float curSample = (float) std::sin (angle);
                    angle += angleDelta;
                    monoBuffer[sample] = targetAmplitude * curSample;
                    updateAngleDelta();
                    time += timeDelta;
                    
                    //Diagnostics
                    // printf ("non-moving sample: (%f %f)\n", curSample, time);
                    // printf("frequency interpolating %f\n", frequency);
                    // printf("amplitude interpolating %f\n", targetAmplitude);
                }
            }
            else
            {
               
                const double frequencyDelta = (localTargetFrequency - frequency) / bufferToFill.numSamples;
                for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
                {
                    const float curSample = (float) std::sin (angle);
                    frequency += frequencyDelta;
                    updateAngleDelta();
                    angle += angleDelta;
                    monoBuffer[sample] = targetAmplitude * curSample;
                    time += timeDelta;
                    
                    //Diagnostics
                    // printf ("moving sample: (%f %f)\n", curSample, time);
                    // printf("frequency interpolating %f\n", frequency);
                    // printf("amplitude interpolating %f\n", targetAmplitude);
                    
                }
                frequency = localTargetFrequency;
            }
        }
        else if (effect && (interpolating == false))
        {
            if (localTargetFrequency == frequency)
            {
                double angle =  2 * double_Pi * (frequency / currentSampleRate);
                for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
                {
                    float curSample = (float) std::sin (angle);
                    float sampleDelta = 2.0 * double_Pi * (frequency / currentSampleRate);
                    angle += sampleDelta;
                    monoBuffer[sample] = curSample;
                    time += timeDelta;
                    //Diagnostics
                    //printf("curSample %f\n", curSample);
                    
                }
            }
            else  //interpolation goes here
            {
                double frequencyDelta = (localTargetFrequency - frequency) / bufferToFill.numSamples;
                double angle =  2 * double_Pi * (frequency / currentSampleRate);
                for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
                {
                    float curSample = (float) std::sin (angle);
                    frequency += frequencyDelta;
                    float sampleDelta = 2.0 * double_Pi * (frequency / currentSampleRate);
                    angle += sampleDelta;
                    monoBuffer[sample] = targetAmplitude * curSample;
                    time += timeDelta;
                  //  printf("curSample %f\n", curSample);
                    
                }
            }
        }
        else
        {
            printf("Error! Setting buttons to defaults.\n");
            interpolating = false;
            effect = false;
            
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
    
    void updateAngleDelta()
    {
        const double cyclesPerSample = freqSlider.getValue() / currentSampleRate;
        angleDelta = cyclesPerSample * 2.0 * double_Pi;
    }
    void releaseResources() override
    {
    }
    
    float interpolate (float sample, float numSamples, float curVal, float targetVal, float timeDelta)
    {
        float increment = (curVal + targetVal) / numSamples;
        return curVal += increment;
     
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
        volumeSlider.setBounds (sliderLeft, 20, getWidth() - sliderLeft - 10, 20);
        freqSlider.setBounds (sliderLeft, 50, getWidth() - sliderLeft - 10, 20);
        muteButton.setBounds (10, 110, getWidth() - 20, 20);
        interpolateButton.setBounds (10, 150, getWidth() - 20, 20);
        effectButton.setBounds (10, 190, getWidth() - 20, 20);
        
    }
    
private:
    //==============================================================================
    
    // Member Variables
    float currentSampleRate;
    float amplitude, targetAmplitude;
    float frequency, targetFrequency;
    float time;
    float phase;
    float timeDelta;
    float interpolationLength;
    float frac;
    float angleDelta, angle;
    // GUI
    Slider volumeSlider;
    Slider freqSlider;
    Label volumeLabel;
    Label freqLabel;
    
    TextButton muteButton;
    TextButton interpolateButton;
    TextButton effectButton;
    bool mute;
    bool interpolating;
    bool effect;
    
    //Look and Feel
    Customized visual;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent() { return new MainContentComponent(); }

#endif  // MAINCOMPONENT_H_INCLUDED

