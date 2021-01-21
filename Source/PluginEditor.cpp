/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

Meter::Meter()
{


}

Meter::~Meter()
{

}

void Meter::paint( juce::Graphics& g )
{
    auto bounds = this->getLocalBounds();
    auto h = bounds.getHeight();
    
    DBG( "Painting meter with level: " << currentLevel );
    
    auto height = jmap( currentLevel,
                        negativeInfinityDB, maxDB,
                        (float)h, 0.f
                       );
    
    //DBG( "Height: " << height );
    
    g.setColour( Colours::pink );
    g.fillRect( bounds.withHeight(h).withY(height) );
}

void Meter::resized()
{

}

void Meter::update( float newLevel )
{
    currentLevel = newLevel;
    repaint();
}


//==============================================================================


Pfmcpp_project10AudioProcessorEditor::Pfmcpp_project10AudioProcessorEditor (Pfmcpp_project10AudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    startTimerHz( 20 );
    
    addAndMakeVisible( testMeter );
    
    setSize (400, 300);
}

Pfmcpp_project10AudioProcessorEditor::~Pfmcpp_project10AudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void Pfmcpp_project10AudioProcessorEditor::paint (Graphics& g)
{

}

void Pfmcpp_project10AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    auto bounds = getBounds();
    
    testMeter.setBounds( bounds.getWidth()/2, 10, 30, bounds.getHeight()+5 );
}

void Pfmcpp_project10AudioProcessorEditor::timerCallback()
{
    if( processor.fifo.pull(buffer) )
    {
        testMeter.update( Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()), testMeter.negativeInfinityDB) );
    }
}
