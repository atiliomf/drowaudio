/*
    ==============================================================================

    This file is part of the dRowAudio JUCE module
    Copyright 2004-13 by dRowAudio.

    ------------------------------------------------------------------------------

    dRowAudio is provided under the terms of The MIT License (MIT):

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

    ==============================================================================
*/

#ifndef AUDIO_PLAYBACK_DEMO_H
#define AUDIO_PLAYBACK_DEMO_H

#include "LoopComponent.h"
#include "DistortionDemo.h"
#include "BufferTransformAudioSource.h"

class AudioPlaybackDemo : public juce::Component,
                          public juce::Slider::Listener
{
public:
    AudioPlaybackDemo (AudioFilePlayerExt& audioFilePlayer,
                       BufferTransformAudioSource& bufferTransformAudioSource);

    //==============================================================================
    void resized() override;
    void paint (juce::Graphics& g) override;
    void sliderValueChanged (juce::Slider* slider) override;

private:
    //==============================================================================
    enum PlayerControls
    {
        lowEQ,
        midEQ,
        highEQ,
        rate,
        tempo,
        pitch,
        numControls
    };

    AudioFilePlayerExt& audioFilePlayer;

    LoopComponent loopComponent;

    juce::GroupComponent filterGroup, rateGroup;
    juce::Slider resolutionSlider, zoomSlider;
    juce::Label resolutionLabel, zoomLabel;

    juce::OwnedArray<juce::Slider> playerControls;
    juce::OwnedArray<juce::Label> playerControlLabels;

    juce::TimeSliceThread backgroundThread;
    juce::AudioThumbnailCache audioThumbnailCache;
#if JUCE_MAC
    ColouredAudioThumbnail audioThumbnail;
#else
    AudioThumbnail audioThumbnail;
#endif
    std::unique_ptr<AudioThumbnailImage> audioThumbnailImage;
    std::unique_ptr<PositionableWaveDisplay> positionableWaveDisplay;
    std::unique_ptr<DraggableWaveDisplay> draggableWaveDisplay;

    DistortionDemo distortionDemo;

    friend class LoopComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPlaybackDemo)
};

#endif //AUDIO_PLAYBACK_DEMO_H
