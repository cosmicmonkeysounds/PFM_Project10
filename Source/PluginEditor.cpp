/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


GUIBuffer::GUIBuffer() : Thread( "GUI Buffer" )
{
    startThread();
    startTimerHz( 20 );
}

GUIBuffer::~GUIBuffer()
{
    notify();
    stopThread( 100 );
}

void GUIBuffer::run()
{
    while( true )
    {
        wait( -1 );
        
        if( threadShouldExit() )
            return;
        
    }
}

void GUIBuffer::timerCallback()
{
    
}

void GUIBuffer::paint( Graphics& g )
{
    g.fillAll( Colours::black );
}

void GUIBuffer::resized()
{
    
}

//==============================================================================
Pfmcpp_project10AudioProcessorEditor::Pfmcpp_project10AudioProcessorEditor (Pfmcpp_project10AudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    addAndMakeVisible( guiBuffer );
    
    setSize (400, 300);
}

Pfmcpp_project10AudioProcessorEditor::~Pfmcpp_project10AudioProcessorEditor()
{
}

//==============================================================================
void Pfmcpp_project10AudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);
}

void Pfmcpp_project10AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    guiBuffer.setBounds( getLocalBounds() );
}
