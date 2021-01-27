/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

ValueHolder::ValueHolder()
{
    startTimerHz(20);
}

ValueHolder::~ValueHolder()
{
    stopTimer();
}

void ValueHolder::timerCallback()
{
    if( juce::Time::currentTimeMillis() - peakTime > holdTime )
        resetCurrentValue();
}

void ValueHolder::setThreshold(float newThreshold)
{
    threshold = newThreshold;
}

void ValueHolder::updateHeldValue(float input)
{

    if( input > threshold )
    {
        peakTime = juce::Time::currentTimeMillis();
        if( input > currentValue )
            currentValue = input;
    }

}

void ValueHolder::setHoldTime(int ms)
{
    holdTime = ms;
}

float ValueHolder::getCurrentValue() const
{
    return currentValue;
}

bool ValueHolder::isOverThreshold() const
{
    return currentValue > threshold;
}

//==============================================================================

void Meter::paint( juce::Graphics& g )
{
    
    g.fillAll( juce::Colours::black );
    
    auto bounds = this->getLocalBounds();
    float heightOfWindow = bounds.getHeight();
    
    auto levelMappedHeight = jmap( currentLevel,
                                   NEGATIVE_INFINITY_DB, MAX_DB,
                                   heightOfWindow, 0.f );
    
    g.setColour( Colours::pink );
    g.fillRect( bounds.withHeight(heightOfWindow).withY(levelMappedHeight) );
}

void Meter::resized()
{
    
    ticks.clear();
    
    int meterBoxHeight = getHeight();
    float y = NEGATIVE_INFINITY_DB;
    
    for( int i = 0; i <= (int)numberOfSteps; ++i )
    {
        Tick tick;
        tick.db = y;
        tick.y = juce::jmap( (int)y,
                             (int)NEGATIVE_INFINITY_DB, (int)MAX_DB,
                             meterBoxHeight, 0 );
        
        ticks.push_back( tick );
        y += dbStepSize;
    }
    
}

void Meter::update( float newLevel )
{
    currentLevel = newLevel;
    
    if( onUpdate )
        onUpdate( newLevel );
    
    repaint();
}


//==============================================================================


void DB_Scale::paint( juce::Graphics& g )
{
    g.setColour( juce::Colours::white );
    
    for( auto tick : ticks )
        g.drawSingleLineText( juce::String(tick.db), 5, tick.y + yOffset );

}

void DB_Scale::resized()
{

}


//==============================================================================


void TextMeter::paint( Graphics& g )
{
    g.setColour( juce::Colours::darkblue );
    g.fillAll();
    
    juce::String valueString;
    
    if( valueHolder.isOverThreshold() )
    {
        valueString = juce::String( valueHolder.getCurrentValue(), decimalPlaces );
        g.setColour( juce::Colours::hotpink );
    }
        
    else
    {
        valueString = juce::String( inputValue, decimalPlaces );
        g.setColour( juce::Colours::whitesmoke );
    }
        
    auto r = getLocalBounds();
    juce::Font f{juce::Font::getDefaultMonospacedFontName(), 13.f, juce::Font::FontStyleFlags::bold};
    int xCentreOffset = r.getCentreX() - (f.getStringWidth(valueString)/2);
    
    g.setFont( f );
    g.drawSingleLineText( valueString, xCentreOffset, r.getCentreY() );
}

void TextMeter::resized()
{
    
}

void TextMeter::update( float newValue )
{
    inputValue = newValue;
    valueHolder.updateHeldValue( newValue );
}


//==============================================================================


Pfmcpp_project10AudioProcessorEditor::Pfmcpp_project10AudioProcessorEditor (Pfmcpp_project10AudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    
    startTimerHz( 20 );
    
    addAndMakeVisible( testMeter );
    addAndMakeVisible( testScale );
    addAndMakeVisible( testTextMeter );
    
    testMeter.onUpdate = [this] (float newValue)
    {
        testTextMeter.update( newValue );
    };
    
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
    
    auto bounds = getBounds();
    
    testMeter.setBounds( bounds.getWidth()/2,
                         JUCE_LIVE_CONSTANT(75),
                         50,
                         JUCE_LIVE_CONSTANT(bounds.getHeight()-150) );
    
    auto meterRect = testMeter.getBounds();
    const int textBoxHeight = 40;

    testTextMeter.setBounds( meterRect.getX(),
                             meterRect.getY() - textBoxHeight,
                             meterRect.getWidth(),
                             textBoxHeight );
    
    testScale.ticks   = testMeter.ticks;
    testScale.yOffset = testMeter.getY();
    
    testScale.setBounds( testMeter.getRight(), 0, 30, getHeight() );
 
}

void Pfmcpp_project10AudioProcessorEditor::timerCallback()
{
    if( processor.fifo.pull(buffer) )
    {

        auto leftRMSLevel = buffer.getRMSLevel( 0, 0, buffer.getNumSamples() );
        
#if SINE_OSC_TEST
        leftRMSLevel = buffer.getMagnitude( 0, 0, buffer.getNumSamples() );
#endif
        
        auto leftRMSdB    = juce::Decibels::gainToDecibels( leftRMSLevel );
        testMeter.update( leftRMSdB );
    }
    
    testTextMeter.repaint();
}
