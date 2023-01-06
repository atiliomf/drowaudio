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

#include "LocalDirectoryListBox.h"
#include "RemoteDirectoryListBox.h"

#if DROWAUDIO_USE_CURL

//==============================================================================
static juce::StringArray getFilesForDirectory (const juce::String& fullPath)
{
    juce::StringArray files;
    files.add ("..");

    juce::File cwd (fullPath);
    if (cwd.isDirectory())
    {
        juce::Array<juce::File> childFiles;
        cwd.findChildFiles (childFiles, juce::File::findFilesAndDirectories, false);

        for (int i = 0; i < childFiles.size(); ++i)
            files.add (childFiles[i].getFileName());
    }

    return files;
}

//==============================================================================
LocalDirectoryListBoxModel::LocalDirectoryListBoxModel()
    : currentWorkingDirectory (juce::File::getSpecialLocation (juce::File::userDesktopDirectory))
{
    setContents (getFilesForDirectory (currentWorkingDirectory.getFullPathName()));
}

int LocalDirectoryListBoxModel::getNumRows()
{
    return itemList.size();
}

void LocalDirectoryListBoxModel::paintListBoxItem (int rowNumber, juce::Graphics& g,
                                                   int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
    {
        g.setColour (juce::Colours::lightblue);
        g.fillAll();
    }

    const int h = height;
    const int m = (int) (height * 0.15f);

    juce::Rectangle<float> imageRect ((float) m, (float) m, (float) (h - (2 * m)),  (float) (h - (2 * m)));

    if (currentWorkingDirectory.getChildFile (itemList[rowNumber]).isDirectory())
        juce::LookAndFeel::getDefaultLookAndFeel().getDefaultFolderImage()->drawWithin (g, imageRect, juce::RectanglePlacement (0), 1.0f);
    else
        juce::LookAndFeel::getDefaultLookAndFeel().getDefaultDocumentFileImage()->drawWithin (g, imageRect, juce::RectanglePlacement (juce::RectanglePlacement::centred), 1.0f);

    g.setColour (juce::Colours::black);
    g.drawFittedText (itemList[rowNumber],
                      height, 0, width - h - m, height,
                      juce::Justification::centredLeft, 1);
}

void LocalDirectoryListBoxModel::setContents (const juce::StringArray& newContents)
{
    itemList = newContents;
}

void LocalDirectoryListBoxModel::refresh()
{
    setContents (getFilesForDirectory (currentWorkingDirectory.getFullPathName()));
    sendChangeMessage();
}

void LocalDirectoryListBoxModel::listBoxItemDoubleClicked (int row, const juce::MouseEvent& /*e*/)
{
    if (itemList[row] == "..")
    {
        currentWorkingDirectory = (currentWorkingDirectory.getParentDirectory());
        setContents (getFilesForDirectory (currentWorkingDirectory.getFullPathName()));
    }
    else
    {
        if (currentWorkingDirectory.getChildFile (itemList[row]).isDirectory())
        {
            currentWorkingDirectory = currentWorkingDirectory.getChildFile (itemList[row]);

            if (currentWorkingDirectory.isDirectory())
                setContents (getFilesForDirectory (currentWorkingDirectory.getFullPathName()));
        }
    }

    sendChangeMessage();
}

juce::var LocalDirectoryListBoxModel::getDragSourceDescription (const juce::SparseSet<int>& currentlySelectedRows)
{
    if (currentlySelectedRows.size() > 0)
        if (currentWorkingDirectory.getChildFile (itemList[currentlySelectedRows[0]]).existsAsFile())
            return currentWorkingDirectory.getChildFile (itemList[currentlySelectedRows[0]]).getFullPathName();

    return juce::String();
}

//==============================================================================
LocalDirectoryListBox::LocalDirectoryListBox()
    : isInterestedInDrag (false)
{
    model.addChangeListener (this);
    setModel (&model);
}

LocalDirectoryListBox::~LocalDirectoryListBox()
{
    model.removeChangeListener (this);
}

void LocalDirectoryListBox::paintOverChildren (juce::Graphics& g)
{
    if (isInterestedInDrag)
    {
        g.setColour (juce::Colours::orange);
        g.drawRect (getLocalBounds(), 3);
    }
}

void LocalDirectoryListBox::changeListenerCallback (juce::ChangeBroadcaster* source)
{
    if (source == &model)
    {
        DBG ("updateContent");
        updateContent();
        repaint();
    }
}

bool LocalDirectoryListBox::isInterestedInDragSource (const SourceDetails& dragSourceDetails)
{
    return dynamic_cast<RemoteDirectoryListBox*> (dragSourceDetails.sourceComponent.get()) != nullptr;
}

void LocalDirectoryListBox::itemDragEnter (const SourceDetails& dragSourceDetails)
{
    isInterestedInDrag = isInterestedInDragSource (dragSourceDetails);
    repaint();
}

void LocalDirectoryListBox::itemDragExit (const SourceDetails&)
{
    isInterestedInDrag = false;
    repaint();
}

void LocalDirectoryListBox::itemDropped (const SourceDetails& dragSourceDetails)
{
    if (RemoteDirectoryListBox* remote = dynamic_cast<RemoteDirectoryListBox*> (dragSourceDetails.sourceComponent.get()))
    {
        DBG (dragSourceDetails.description.toString());
        const juce::String remoteFileName (dragSourceDetails.description.toString().fromLastOccurrenceOf ("/", true, false));

        CURLEasySession& session (remote->getCURLSession());
        session.setRemotePath (dragSourceDetails.description.toString());
        session.setLocalFile (juce::File (model.getCurrentWorkingDirectory().getFullPathName() + remoteFileName));
        session.beginTransfer (false);
        model.refresh();
    }

    isInterestedInDrag = false;
    repaint();
}

#endif //DROWAUDIO_USE_CURL
