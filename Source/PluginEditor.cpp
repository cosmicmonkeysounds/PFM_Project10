/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

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

DecayingValueHolder::DecayingValueHolder()
{
    startTimerHz( timerHz );
}

DecayingValueHolder::~DecayingValueHolder()
{
    stopTimer();
}

void DecayingValueHolder::timerCallback()
{
    
    if( juce::Time::currentTimeMillis() - elapsedTime > holdTime )
    {
        if( decayRateDB < 1.f )
            dx += decayRateDB;
        
        else
            dx = (float)std::pow( decayRateDB, exponent );
        
        currentValue -= dx;
        ++exponent;
        
        if( currentValue <= NEGATIVE_INFINITY_DB )
        {
            currentValue = NEGATIVE_INFINITY_DB;
            exponent = 1;
            dx = 0.f;
        }
    }
    
}

void DecayingValueHolder::updateHeldValue( float newValue )
{
    if( newValue > currentValue )
    {
        currentValue = newValue;
        elapsedTime = juce::Time::currentTimeMillis();
        exponent = 1;
        dx = 0.f;
    }
}

void DecayingValueHolder::setDecayRate( float newRate )
{
    decayRateDB = newRate / (float)timerHz;
}

void DecayingValueHolder::setHoldTime( int newHoldTime )
{
    holdTime = newHoldTime;
}

//==============================================================================

void Meter::paint( juce::Graphics& g )
{
    
    g.fillAll( juce::Colours::black );
    
    auto bounds = this->getLocalBounds();
    float heightOfWindow = bounds.getHeight();
    
    float levelMappedHeight = juce::jmap( currentLevel,
                                          NEGATIVE_INFINITY_DB, MAX_DB,
                                          heightOfWindow, 0.f );
    
    g.setColour( juce::Colours::pink );
    g.fillRect( bounds.withHeight(heightOfWindow).withY(levelMappedHeight) );
    
    g.setColour( juce::Colours::white );
    
    float decayY = juce::jmap( decayingValueHolder.getCurrentValue(),
                               NEGATIVE_INFINITY_DB, MAX_DB,
                               heightOfWindow, 0.f );
    
    g.fillRect( getLocalBounds().withHeight(4).withY(decayY) );
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
    decayingValueHolder.updateHeldValue( newLevel );
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

MacroMeterWidget::MacroMeterWidget()
    : averager( 10, 0.f )
{
    addAndMakeVisible( instantMeter );
    addAndMakeVisible( averageMeter );
    addAndMakeVisible( textMeter );
}

void MacroMeterWidget::paint( juce::Graphics& g )
{
    g.setColour( juce::Colours::black );
    g.fillAll();
}

void MacroMeterWidget::resized()
{
    auto r = getLocalBounds();
    
    r = r.withSizeKeepingCentre( r.getWidth() * 0.9f,
                                 r.getHeight() );
    
    const int textBoxHeight = 40;
    textMeter.setBounds( r.removeFromTop(textBoxHeight) );
    
    const int innerPadding = 3;
    const float instantMeterWidth = r.getWidth() * 0.75f;
    
    instantMeter.setBounds( r.removeFromLeft(instantMeterWidth) );
    averageMeter.setBounds( r.withTrimmedLeft(innerPadding) );
}

void MacroMeterWidget::update( float newValue )
{
    instantMeter.update( newValue );
    textMeter.update( newValue );
    averager.add( newValue );
    averageMeter.update( averager.getAverage() );
}

std::vector<Tick> MacroMeterWidget::getTicks()
{
    return instantMeter.ticks;
}

int MacroMeterWidget::getMeterY()
{
    int macroHeight = getBounds().getY();
    int meterHeight = instantMeter.getBounds().getY();
    return macroHeight + meterHeight;
}

//==============================================================================


StereoMeterWidget::StereoMeterWidget( juce::String l ) : label(l)
{
    addAndMakeVisible( leftMeterWidget );
    addAndMakeVisible( rightMeterWidget );
    addAndMakeVisible( dbScale );
}

void StereoMeterWidget::paint( juce::Graphics& g )
{
    g.setColour( juce::Colours::black );
    g.fillAll();
    
    g.setColour( juce::Colours::white );
    {
        using j = juce::Justification;
        g.drawText( "L",   labelArea, j::centredLeft );
        g.drawText( label, labelArea, j::centred );
        g.drawText( "R",   labelArea, j::centredRight );
    }
}

void StereoMeterWidget::resized()
{
    auto r = getLocalBounds();
    const int labelHeight = 30;
    const int meterWidth = r.getWidth() / 3;
    
    labelArea = r.removeFromBottom( labelHeight )
                 .withSizeKeepingCentre( r.getWidth() - meterWidth, labelHeight );
    
    leftMeterWidget.setBounds( r.removeFromLeft(meterWidth) );
    
    dbScale.ticks   = leftMeterWidget.getTicks();
    dbScale.yOffset = leftMeterWidget.getMeterY();
    
    dbScale.setBounds( r.removeFromLeft(meterWidth)
                        .withHeight(getHeight()) );
    
    rightMeterWidget.setBounds( r );
}

void StereoMeterWidget::update( float newLeftValue, float newRightValue )
{
    leftMeterWidget.update( newLeftValue );
    rightMeterWidget.update( newRightValue );
    repaint();
}


//==============================================================================


HistogramDisplay::HistogramDisplay( std::size_t bufferSize, juce::String l )
    : buffer( bufferSize, NEGATIVE_INFINITY_DB ), label(l)
{

}

void HistogramDisplay::paint( juce::Graphics& g )
{
    g.setColour( juce::Colours::black );
    g.fillAll();
    
    auto& yData = buffer.getData();
    std::size_t readIndex = buffer.getReadIndex();
    std::size_t size = buffer.getSize();
    
    float minY = (float)getLocalBounds().getHeight();
    float maxY = 10.f;
    float yPos = 0.f;
    
    Path path;
    
    path.startNewSubPath( -2.f, minY + 5.f );
    
    for( int i = 0; i < size; ++i )
    {
        
        yPos = juce::jmap( yData[readIndex],
                           NEGATIVE_INFINITY_DB, MAX_DB,
                           minY, maxY );
        
        path.lineTo( i, yPos);
        
        if( ++readIndex > size - 1 )
            readIndex = 0;

    }
    
    float endX = (float)getLocalBounds().getWidth() + 2.f;
    path.lineTo( endX, yPos );
    path.lineTo( endX, minY + 2.f );
    
    path.closeSubPath();
    
    g.setColour( juce::Colours::white );
    g.strokePath( path, juce::PathStrokeType{1.5f} );
    
    juce::Colour green{ juce::Colours::green.withMultipliedAlpha(0.75f) };
    juce::Colour yellow{ juce::Colours::yellow.withMultipliedAlpha(0.75f) };
    juce::Colour red{ juce::Colours::red.withMultipliedAlpha(0.75f) };
    
    juce::ColourGradient gradient = juce::ColourGradient::vertical( green, minY,
                                                                    green.darker(), juce::jmap(-5.9f,
                                                                                               NEGATIVE_INFINITY_DB, MAX_DB,
                                                                                               minY, maxY) );
    
    gradient.addColour( (double)juce::jmap(-3.f,
                                           NEGATIVE_INFINITY_DB, MAX_DB,
                                           0.f, 1.f), yellow );
    
    gradient.addColour( (double)juce::jmap(-0.1f,
                                           NEGATIVE_INFINITY_DB, MAX_DB,
                                           0.f, 1.f), yellow.darker() );
    
    gradient.addColour( (double)juce::jmap(0.f,
                                           NEGATIVE_INFINITY_DB, MAX_DB,
                                           0.f, 1.f), red );
    
    gradient.addColour( (double)juce::jmap(12.f,
                                           NEGATIVE_INFINITY_DB, MAX_DB,
                                           0.f, 1.f), red.darker() );
    
    g.setGradientFill( gradient );
    g.fillPath( path );
    
    g.setColour( juce::Colours::white );
    g.setFont( 16.f );
    g.drawText( label, getLocalBounds().removeFromBottom(20), juce::Justification::centred );

}

void HistogramDisplay::resized()
{
    buffer.resize( getWidth(), NEGATIVE_INFINITY_DB );
}

void HistogramDisplay::update( float newValue )
{
    buffer.write( newValue );
    repaint();
}


//==============================================================================


HistogramWidget::HistogramWidget()
{
    addAndMakeVisible( rmsDisplay );
    addAndMakeVisible( peakDisplay );
}

void HistogramWidget::paint( juce::Graphics& g )
{
    g.setColour( juce::Colours::black );
    g.fillAll();
}

void HistogramWidget::resized()
{
    auto r = getLocalBounds();
    const int halfWidth = r.getWidth() / 2;
    const int padding = 10;
    
    rmsDisplay.setBounds( r.removeFromLeft(halfWidth).reduced(padding) );
    peakDisplay.setBounds( r.reduced(padding) );
}

void HistogramWidget::update( float rms, float peak )
{
    rmsDisplay.update( rms );
    peakDisplay.update( peak );
    repaint();
}


//==============================================================================


Pfmcpp_project10AudioProcessorEditor::Pfmcpp_project10AudioProcessorEditor (Pfmcpp_project10AudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    startTimerHz( 20 );
    
    addAndMakeVisible( rmsWidget );
    addAndMakeVisible( peakWidget );
    addAndMakeVisible( histogramDisplays );
    
    setSize (800, 640);
}

Pfmcpp_project10AudioProcessorEditor::~Pfmcpp_project10AudioProcessorEditor()
{
    stopTimer();
}

void Pfmcpp_project10AudioProcessorEditor::paint (Graphics& g)
{
    g.setColour( juce::Colours::ivory );
    g.fillAll();
}

void Pfmcpp_project10AudioProcessorEditor::resized()
{
    auto r = getLocalBounds().withSizeKeepingCentre(getWidth() * 0.95, getHeight() * 0.95);
    const int padding = 10;
    
    auto meterBounds = r.removeFromTop(r.getHeight()/2).reduced(0, padding);
    
    rmsWidget.setBounds(meterBounds.removeFromLeft(150));
    
    peakWidget.setBounds( meterBounds.removeFromLeft(150)
                                     .translated(50, 0) );
    
    histogramDisplays.setBounds( r );
}

void Pfmcpp_project10AudioProcessorEditor::timerCallback()
{
    if( processor.fifo.pull(buffer) )
    {

        int numSamples = buffer.getNumSamples();
        
        //==============================================================================
        
        auto leftRMSLevel  = buffer.getRMSLevel( 0, 0, numSamples );
        auto rightRMSLevel = buffer.getRMSLevel( 1, 0, numSamples );
        
        auto leftRMSdB    = juce::Decibels::gainToDecibels( leftRMSLevel );
        auto rightRMSdB   = juce::Decibels::gainToDecibels( rightRMSLevel );
        
        rmsWidget.update( leftRMSdB, rightRMSdB );
        
        //==============================================================================
        
        auto leftMagnitudeLevel  = buffer.getMagnitude( 0, 0, numSamples );
        auto rightMagnitudeLevel = buffer.getMagnitude( 1, 0, numSamples );
        
        auto leftMagnitudeDB  = juce::Decibels::gainToDecibels( leftMagnitudeLevel );
        auto rightMagnitudeDB = juce::Decibels::gainToDecibels( rightMagnitudeLevel );
        
        peakWidget.update( leftMagnitudeDB, rightMagnitudeDB );
        
        //==============================================================================
        
        auto averageRMSdB  = (leftRMSdB + rightRMSdB) / 2.f;
        auto averagePeakDB = (leftMagnitudeDB + rightMagnitudeDB) / 2.f;
        
        histogramDisplays.update( averageRMSdB, averagePeakDB );
        
        //==============================================================================
        
    }
    
}
