void DelayChain::applyDelay (AudioSampleBuffer& buffer,  const float delInSamples, int channel, const float dryMix, const float wetMix, const float feedback) {
    
    float fractionalDelay = delInSamples - (int)delInSamples;

    this->delayReadPosition_ = (int)(delayWritePosition_ - (delInSamples) + delayBufferLen_) % delayBufferLen_;
    
    const float* inputBuffer = buffer.getReadPointer (channel);
    float* outputBuffer = buffer.getWritePointer(channel);

    float* delayBuffer = this->delayBuffer_.getWritePointer (channel);

    for (int i = 0; i < wetBuffer_.getNumSamples(); ++i)
    {
        float xn = inputBuffer[i];
        float yn = delayBuffer[this->delayReadPosition_];
        int rIndex_1 = this->delayReadPosition_ - 1;
        if (rIndex_1 < 0) {
            rIndex_1 = delayBufferLen_ - 1;
        }
        float yn_1 = delayBuffer[rIndex_1];
        float interp = linInterp(0.0, 1.0, yn, yn_1, fractionalDelay);
        yn = interp;
        delayBuffer[this->delayWritePosition_] = xn + yn*feedback;
        outputBuffer[i] = wetMix*yn + dryMix*xn;
        ++this->delayReadPosition_ %= delayBufferLen_;
        ++this->delayWritePosition_ %= delayBufferLen_;
    }
}

void OutputCollinDelayAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    feedbackSmooth.setValue(*parameters.getRawParameterValue ("feedback"));
    delLenSmoothL.setValue(*parameters.getRawParameterValue ("timeL"));
    delLenSmoothR.setValue(*parameters.getRawParameterValue ("timeR"));
    dryWetMixSmooth.setValue(*parameters.getRawParameterValue ("wetDry"));
    
    const float dryMix = (1.0-dryWetMixSmooth.getNextValue());
    const float wetMix = dryWetMixSmooth.getNextValue();
    const bool isLinked = *parameters.getRawParameterValue ("link") < 0.5f ? false : true;
    
    const bool isSynched = *parameters.getRawParameterValue ("tempoSynch") < 0.5f ? false : true;
    const float feedback = feedbackSmooth.getNextValue();
    AudioPlayHead::CurrentPositionInfo posInfo;
    updateBPM(posInfo);

    if (isLinked) {
        delLenSmoothR = delLenSmoothL;
    }
    float delInSamplesL;
    float delInSamplesR;

    float currentTempDenom = 4.0;
    
    float beatsPerMilliSecond = 60000.0/currentBPM;
    float beatsPerMilliSecondNorm = beatsPerMilliSecond/1000.0;

    if (!isSynched) {
        delInSamplesL = delLenSmoothL.getNextValue() * getSampleRate();
        delInSamplesR = delLenSmoothR.getNextValue() * getSampleRate();

    } else {
        delInSamplesR = beatsPerMilliSecondNorm * getSampleRate();
        delInSamplesL = delInSamplesR;
    }
    
    for (int channel = 0; channel < getTotalNumOutputChannels(); ++channel)
    {
        if (channel == 0) {
            leftDelayChain.applyDelay(buffer, delInSamplesL, channel, dryMix, wetMix, feedback);
        } else if (channel == 1) {
            rightDelayChain.applyDelay(buffer, delInSamplesR, channel, dryMix, wetMix, feedback);
        }
    }
    meterSource.measureBlock (buffer);
}-