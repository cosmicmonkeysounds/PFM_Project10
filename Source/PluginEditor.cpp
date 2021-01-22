/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

void Meter::paint( juce::Graphics& g )
{
    auto bounds = this->getLocalBounds();
    float heightOfWindow = bounds.getHeight();
    
    //DBG( "Painting meter with level: " << currentLevel );
    
    auto levelMappedHeight = jmap( currentLevel,
                                   NEGATIVE_INFINITY_DB, MAX_DB,
                                   heightOfWindow, 0.f
                            );
    
    g.setColour( Colours::pink );
    g.fillRect( bounds.withHeight(heightOfWindow).withY(levelMappedHeight) );
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

DB_Scale::DB_Scale( Meter& _owner )
    : owner( _owner )
{
    
}


void DB_Scale::paint( juce::Graphics& g )
{
    g.setColour( juce::Colours::white );
    
    auto meterBounds = owner.getLocalBounds(); //.withHeight( getLocalBounds().getHeight()/numberOfSteps );
    auto textBounds  = getLocalBounds();
    
    float delta = -(meterBounds.getHeight() / numberOfSteps);
    
    float number = NEGATIVE_INFINITY_DB;
    
    for( int i = 0; i <= (int)numberOfSteps; ++i )
    {
        g.drawText( juce::String(number), textBounds, juce::Justification::centredBottom );
        number += dbStepSize;
        textBounds.translate( 0, delta );
    }
    
    
}

void DB_Scale::resized()
{
    
}


//==============================================================================


Pfmcpp_project10AudioProcessorEditor::Pfmcpp_project10AudioProcessorEditor (Pfmcpp_project10AudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    startTimerHz( 20 );
    
    addAndMakeVisible( testMeter );
    addAndMakeVisible( testScale );
    
    setSize (800, 640);
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
    DBG( "editor bounds: " << bounds.toString() );
    
    testMeter.setBounds( bounds.getWidth()/2, 75, 30, bounds.getHeight()-150 );
    testScale.setBounds( testMeter.getBounds().translated(40, -15).withHeight(testMeter.getHeight() + 20) );
 
}

void Pfmcpp_project10AudioProcessorEditor::timerCallback()
{
    if( processor.fifo.pull(buffer) )
    {
        testMeter.update
        (
            Decibels::gainToDecibels( buffer.getRMSLevel(0, 0, buffer.getNumSamples()), NEGATIVE_INFINITY_DB )
        );
        
    }
}
