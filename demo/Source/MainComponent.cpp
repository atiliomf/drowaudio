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

#include "MainComponent.h"
#include "playback/AudioPlaybackDemo.h"
#include "network/NetworkDemo.h"
#include "fft/FFTDemo.h"

MainComponent::MainComponent()
    : bufferTransformAudioSource (&audioFilePlayer),
      trackInfoComponent (audioFilePlayer),
      dropTarget (&audioFilePlayer, &trackInfoComponent),
      transport (audioDeviceManager, audioFilePlayer),
      meterThread ("Meter Thread"),
      cpuMeter (&audioDeviceManager),
      tabbedComponent (juce::TabbedButtonBar::TabsAtTop)
{
    clock.setColour (juce::Label::textColourId, juce::Colours::white);
    clock.setJustificationType (juce::Justification::centred);

    cpuMeter.setTextColour (juce::Colours::red);
    cpuMeter.setJustificationType (juce::Justification::centred);
    cpuMeter.setBorderSize (juce::BorderSize<int>());

    tabbedComponent.addTab ("Audio Playback",
                            juce::Colours::grey,
                            new AudioPlaybackDemo (audioFilePlayer, bufferTransformAudioSource),
                            true);

    const juce::File libraryFile (juce::File::getSpecialLocation (juce::File::userDesktopDirectory).getChildFile ("dRowAudio Demo Library.xml"));
    juce::ValueTree libraryTree (readValueTreeFromFile (libraryFile));
    ITunesLibrary::getInstance()->setLibraryTree (libraryTree);
    ITunesLibrary::getInstance()->setLibraryFile (ITunesLibrary::getDefaultITunesLibraryFile());
    MusicLibraryTable* musicLibraryTable = new MusicLibraryTable();
    musicLibraryTable->setLibraryToUse (ITunesLibrary::getInstance());

    musicLibraryTable->getTableListBox().setColour (juce::ListBox::backgroundColourId, juce::Colour::greyLevel (0.2f));
    musicLibraryTable->getTableListBox().setColour (juce::ListBox::outlineColourId, juce::Colours::grey);
    musicLibraryTable->getTableListBox().setColour (juce::ListBox::textColourId, juce::Colours::darkgrey);

    tabbedComponent.addTab ("iTunes Library",
                            juce::Colours::grey, musicLibraryTable, true);

    ColumnFileBrowser* columnFileBrowser = new ColumnFileBrowser (new juce::WildcardFileFilter (audioFilePlayer.getAudioFormatManager()->getWildcardForAllFormats(),
                                                                                          "*",
                                                                                          "Audio Files"));
    tabbedComponent.addTab ("Column File Browser",
                            juce::Colours::grey,
                            columnFileBrowser,
                            true);

    fftDemo = new FFTDemo();
    tabbedComponent.addTab ("FFT Demo",
                            juce::Colours::grey,
                            fftDemo,
                            true);

#if DROWAUDIO_USE_CURL
    tabbedComponent.addTab ("CURL Demo",
                            juce::Colours::grey,
                            new NetworkDemo(),
                            true);
#endif

    audioSourcePlayer.setSource (&bufferTransformAudioSource);
    audioDeviceManager.initialise (1, 2, nullptr, true);
    audioDeviceManager.addAudioCallback (this);

    addAndMakeVisible (&trackInfoComponent);
    addAndMakeVisible (&dropTarget);
    addAndMakeVisible (&transport);
    addAndMakeVisible (&meterL);
    addAndMakeVisible (&meterR);
    addAndMakeVisible (&tabbedComponent);
    addAndMakeVisible (&cpuMeter);
    addAndMakeVisible (&clock);

    meterThread.addTimeSliceClient (&meterL);
    meterThread.addTimeSliceClient (&meterR);
    meterThread.startThread (juce::Thread::Priority::low);
    
    addAndMakeVisible (&searchBox);
    searchBox.addListener (this);
    searchBox.setTextToShowWhenEmpty ("search...", juce::Colours::grey);

    setSize (800, 600);
    
//    #if DROWAUDIO_USE_FFTREAL
//    DBG ("Using FFTREAL");
//    #endif
//    
//    #if DROWAUDIO_USE_VDSP
//    DBG ("Using VDSP");
//    #endif
//    
//    #if DROWAUDIO_USE_SOUNDTOUCH
//    DBG ("Using SOUNDTOUCH"); 
//    #endif
}

MainComponent::~MainComponent()
{
    meterThread.removeTimeSliceClient (&meterL);
    meterThread.removeTimeSliceClient (&meterR);
    meterThread.stopThread (500);

    audioSourcePlayer.setSource (nullptr);
    audioDeviceManager.removeAudioCallback (this);

    const juce::File libraryFile (juce::File::getSpecialLocation (juce::File::userDesktopDirectory).getChildFile ("dRowAudio Demo Library.xml"));
    juce::ValueTree libraryTree (ITunesLibrary::getInstance()->getLibraryTree());
    writeValueTreeToFile (libraryTree, libraryFile);
}

void MainComponent::resized()
{
//    const int w = getWidth();
//    const int h = getHeight();
//    
//    int trackInfoHeight = 120;
//    int searchHeight = 20;
//
//    trackInfoComponent.setBounds (0, 0, w - 145, trackInfoHeight + 5);
//    transport.setBounds (w - 145, 20, 100, h - 20);
//    clock.setBounds (transport.getX(), 0, transport.getWidth(), 20);
//
//    cpuMeter.setBounds (clock.getRight(), 0, w - clock.getRight(), clock.getHeight());
//    
//    int meterHeight = trackInfoHeight - cpuMeter.getHeight() - searchHeight;
//    meterL.setBounds(transport.getRight() + 5, cpuMeter.getBottom(), 15, meterHeight);
//    meterR.setBounds(meterL.getRight() + 5, cpuMeter.getBottom(), 15, meterHeight);
//
//    searchBox.setBounds (transport.getX(), meterL.getBottom() + 5, w - transport.getX() - 5, searchHeight);
//    tabbedComponent.setBounds (0, searchBox.getBottom() + 5, w, h - searchBox.getBottom() - 5);
    
    const int w = getWidth();
    const int h = getHeight();

    trackInfoComponent.setBounds (0, 0, w - 145, 100);
    transport.setBounds (w - 145, 20, 100, h - 20);
    clock.setBounds (transport.getX(), 0, transport.getWidth(), 20);

    cpuMeter.setBounds (clock.getRight(), 0, w - clock.getRight(), clock.getHeight());
    meterL.setBounds(transport.getRight() + 5, cpuMeter.getBottom(), 15, trackInfoComponent.getHeight() - cpuMeter.getHeight());
    meterR.setBounds(meterL.getRight() + 5, cpuMeter.getBottom(), 15, trackInfoComponent.getHeight() - cpuMeter.getHeight());

    tabbedComponent.setBounds (0, trackInfoComponent.getBottom(), w, h - trackInfoComponent.getBottom());
    searchBox.setBounds (transport.getX(), tabbedComponent.getY() + 5, w - transport.getX() - 5, tabbedComponent.getTabBarDepth() - 10);
}

void MainComponent::textEditorTextChanged (juce::TextEditor& editor)
{
    if (&editor == &searchBox)
    {
        auto musicLibraryTable = static_cast<MusicLibraryTable*> (tabbedComponent.getTabContentComponent (1));
        musicLibraryTable->setFilterText (searchBox.getText());

        if (tabbedComponent.getCurrentTabName() != "iTunes Library")
            tabbedComponent.setCurrentTabIndex (1, true);

        searchBox.grabKeyboardFocus();
    }
}

void MainComponent::audioDeviceIOCallbackWithContext (const float* const* inputChannelData,
                                                      int numInputChannels,
                                                      float* const* outputChannelData,
                                                      int numOutputChannels,
                                                      int numSamples,
                                                      const juce::AudioIODeviceCallbackContext& context)
{
    audioSourcePlayer.audioDeviceIOCallbackWithContext (inputChannelData,
                                                        numInputChannels,
                                                        outputChannelData,
                                                        numOutputChannels,
                                                        numSamples,
                                                        context);

    if (fftDemo->isCurrentlyShowing) 
        fftDemo->processBlock (outputChannelData[0], numSamples);

    meterL.copySamples (outputChannelData[0], numSamples);

    if (numOutputChannels > 1)
        meterR.copySamples (outputChannelData[1], numSamples);
}

void MainComponent::audioDeviceAboutToStart (juce::AudioIODevice* device)
{
    audioSourcePlayer.audioDeviceAboutToStart (device);
    fftDemo->setSampleRate (device->getCurrentSampleRate());
}

void MainComponent::audioDeviceStopped()
{
    audioSourcePlayer.audioDeviceStopped();
}
