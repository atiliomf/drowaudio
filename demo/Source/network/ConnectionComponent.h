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

#ifndef CONNECTION_COMPONENT_H
#define CONNECTION_COMPONENT_H

#include "../DemoHeader.h"

#if DROWAUDIO_USE_CURL

class ConnectionComponent : public juce::Component,
                            public juce::Button::Listener
{
public:
    ConnectionComponent();

    void setCURLSessionToControl (CURLEasySession* sessionToControl);

    //==============================================================================
    /** Used to receive callbacks when a connection is changed. */
    class Listener
    {
    public:
        /** Destructor */
        virtual ~Listener() {}

        /** Called when the button is clicked. */
        virtual void connectionChanged (ConnectionComponent* /*connectionComponent*/) {}
    };

    void addListener (Listener* newListener);

    void removeListener (Listener* listener);

    //==============================================================================
    /** @internal */
    void resized() override;
    /** @internal */
    void buttonClicked (juce::Button* button) override;

private:
    //==============================================================================
    CURLEasySession* curlSession;
    juce::ListenerList <Listener> listeners;

    juce::Label urlLabel, hostnameLabel, usernameLabel, passwordLabel, protocolLabel;
    juce::TextEditor urlEditor, hostnameEditor, usernameEditor, passwordEditor;
    juce::ComboBox protocolBox;
    juce::TextButton connectButton;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConnectionComponent)
};

#endif //DROWAUDIO_USE_CURL
#endif //CONNECTION_COMPONENT_H
