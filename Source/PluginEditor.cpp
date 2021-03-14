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

Meter::Meter()
{
    remakeGradient();
}

void Meter::paint( juce::Graphics& g )
{
    g.fillAll( juce::Colours::black );
    
    auto r       = getLocalBounds().toFloat();
    float w      = r.getWidth();
    float h      = r.getHeight();
    float yLevel = juce::jmap( currentLevel, NEGATIVE_INFINITY_DB, MAX_DB, h, 0.f );
    float yDecay = juce::jmap( decayingValueHolder.getCurrentValue(), NEGATIVE_INFINITY_DB, MAX_DB, h, 0.f );
    
    juce::Rectangle<float> meter {0, yLevel, w, h - yLevel };
    g.setFillType( juce::FillType{gradient} );
    //g.setGradientFill( gradient );
    g.fillRect( meter );
    
    
    g.setColour( juce::Colours::white );
    g.fillRect( r.withHeight(4).withY(yDecay) );
}

void Meter::resized()
{
    remakeGradient();
}

void Meter::update( float newLevel )
{
    currentLevel = newLevel;
    decayingValueHolder.updateHeldValue( newLevel );
    repaint();
}

void Meter::updateThreshold( float newThreshold )
{
    threshold = newThreshold;
    remakeGradient();
}

void Meter::remakeGradient()
{
    gradient.clearColours();
    
    juce::Colour green{ juce::Colours::green };
    juce::Colour yellow{ juce::Colours::yellow };
    juce::Colour red{ juce::Colours::red };
    
    gradient.addColour( 0.1, green );
    
    float yellowPercent = juce::jmap( 0.75f, 0.f, 1.f, NEGATIVE_INFINITY_DB, threshold );
    gradient.addColour( (double)juce::jmap(yellowPercent, NEGATIVE_INFINITY_DB, MAX_DB, 0.f, 1.f), yellow );

    float darkYellowPercent = juce::jmap( 0.90f, 0.f, 1.f, NEGATIVE_INFINITY_DB, threshold );
    gradient.addColour( (double)juce::jmap(darkYellowPercent, NEGATIVE_INFINITY_DB, threshold, 0.f, 1.f), yellow.darker() );

    float redPercent = juce::jmap( 0.98f, 0.f, 1.f, NEGATIVE_INFINITY_DB, threshold );
    gradient.addColour( (double)juce::jmap(redPercent, NEGATIVE_INFINITY_DB, threshold, 0.f, 1.f), red );
    gradient.addColour( 1.0, red.darker() );
    
    gradient.point1 = juce::Point{0.f, 0.f};
    gradient.point2 = juce::Point{0.f, threshold};
}


//==============================================================================


void DB_Scale::paint( juce::Graphics& g )
{
    const int centreX  = getLocalBounds().getCentreX();
    const int fontSize = 12;
    juce::Font f ("monospace", fontSize, juce::Font::plain);
    
    g.setFont(f);
    g.setColour( juce::Colours::white );
    
    for( auto tick : ticks )
    {
        const juce::String db(tick.db);
        const int xPos = centreX - (f.getStringWidth(db) / 2);
        g.drawSingleLineText( db, xPos, tick.y + (fontSize / 2) + yOffset );
    }
}

void DB_Scale::resized()
{
    ticks.clear();
    
    int meterBoxHeight = displayBounds.getHeight();
    float y = NEGATIVE_INFINITY_DB;
    
    for( int i = 0; i <= (int)numberOfSteps; ++i )
    {
        Tick tick;
        tick.db = y;
        tick.y = juce::jmap( (int)y, (int)NEGATIVE_INFINITY_DB, (int)MAX_DB, meterBoxHeight, 0 );
        ticks.push_back( tick );
        y += dbStepSize;
    }
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
    auto r = getLocalBounds().withSizeKeepingCentre( getWidth() * 0.9f, getHeight() );
    
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

int MacroMeterWidget::getMeterY()
{
    int macroHeight = getLocalBounds().getY();
    int meterHeight = instantMeter.getBounds().getY();
    return macroHeight + meterHeight;
}

void MacroMeterWidget::updateThreshold( float newThreshold )
{
    instantMeter.updateThreshold( newThreshold );
    averageMeter.updateThreshold( newThreshold );
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
    rightMeterWidget.setBounds( r.removeFromRight(meterWidth) );
    
    juce::Rectangle<int> dbScaleBounds = getLocalBounds().withTrimmedLeft(meterWidth).withTrimmedRight(meterWidth);
    dbScale.displayBounds = r.withTrimmedTop(leftMeterWidget.getMeterY());
    dbScale.yOffset = leftMeterWidget.getMeterY();
    dbScale.setBounds( dbScaleBounds );
}

void StereoMeterWidget::update( float newLeftValue, float newRightValue )
{
    leftMeterWidget.update( newLeftValue );
    rightMeterWidget.update( newRightValue );
    repaint();
}

void StereoMeterWidget::updateThreshold( float newThreshold )
{
    leftMeterWidget.updateThreshold( newThreshold );
    rightMeterWidget.updateThreshold( newThreshold );
}

juce::Rectangle<int> StereoMeterWidget::getDbScaleBounds()
{
    return dbScale.displayBounds;
}

//==============================================================================


HistogramDisplay::HistogramDisplay( std::size_t bufferSize, juce::String l )
    : buffer( bufferSize, NEGATIVE_INFINITY_DB ), label(l)
{
    remakeGradient();
}

void HistogramDisplay::paint( juce::Graphics& g )
{
    g.setColour( juce::Colours::black );
    g.fillAll();
    
    auto& yData           = buffer.getData();
    std::size_t size      = buffer.getSize();
    std::size_t readIndex = buffer.getReadIndex();
    
    float minY = (float)getHeight();
    float maxY = 10.f;
    float yPos = 0.f;
    float xPos = 0.f;
    
    auto bounds     = getLocalBounds();
    auto lowerLeft  = bounds.getBottomLeft().toFloat() + juce::Point<float>{0, 2.f};
    auto lowerRight = bounds.getBottomRight().toFloat() + juce::Point<float>{0, 2.f};
    
    juce::Path path;
    path.startNewSubPath( lowerLeft );

    while( xPos < (float)size )
    {
        yPos = juce::jmap( yData[readIndex], NEGATIVE_INFINITY_DB, MAX_DB, minY, maxY );
        path.lineTo( xPos, yPos);

        if( ++readIndex > size - 1 )
            readIndex = 0;

        xPos += 1.f;
    }
    
    path.lineTo( lowerRight );
    path.closeSubPath();
    
    g.setColour( juce::Colours::white );
    g.strokePath( path, juce::PathStrokeType{1.5f} );
    
    g.setGradientFill( gradient );
    g.fillPath( path );
    
    g.setColour( juce::Colours::white );
    g.setFont( 16.f );
    g.drawText( label, bounds.removeFromBottom(20), juce::Justification::centred );
}

void HistogramDisplay::resized()
{
    buffer.resize( getWidth(), NEGATIVE_INFINITY_DB );
    gradient.point1 = { 0, (float)getHeight() };
    gradient.point2 = { 0, 0 };
}

void HistogramDisplay::update( float newValue )
{
    buffer.write( newValue );
    repaint();
}

void HistogramDisplay::remakeGradient()
{
    juce::Colour green{ juce::Colours::green.withMultipliedAlpha(0.75f) };
    juce::Colour yellow{ juce::Colours::yellow.withMultipliedAlpha(0.75f) };
    juce::Colour red{ juce::Colours::red.withMultipliedAlpha(0.75f) };
    
    gradient.addColour( 0.0, green );
    
    float percentYellow = juce::jmap( 0.8f, 0.f, 1.f, NEGATIVE_INFINITY_DB, threshold );
    gradient.addColour( (double)juce::jmap(percentYellow, NEGATIVE_INFINITY_DB, threshold, 0.f, 1.f), yellow );
    
    float percentDarkYellow = juce::jmap( 0.9f, 0.f, 1.f, NEGATIVE_INFINITY_DB, threshold );
    gradient.addColour( (double)juce::jmap(percentDarkYellow, NEGATIVE_INFINITY_DB, threshold, 0.f, 1.f), yellow.darker() );
    
    float percentRed = juce::jmap( 0.91f, 0.f, 1.f, NEGATIVE_INFINITY_DB, threshold );
    gradient.addColour( (double)juce::jmap(percentRed, NEGATIVE_INFINITY_DB, threshold, 0.f, 1.f), red );
    gradient.addColour( 1.0, red.darker() );
    
    gradient.point1 = juce::Point{0.f, 0.f};
    gradient.point2 = juce::Point{0.f, threshold};
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

void Goniometer::paint( juce::Graphics& g )
{
    g.drawImageAt( background, 0, 0 );
    
    g.setColour( juce::Colours::skyblue );
    g.strokePath( path, juce::PathStrokeType{2.f} );
}

void Goniometer::resized()
{
    auto r = getLocalBounds();
    
    auto pixelFormat = juce::Image::PixelFormat::SingleChannel;
    background = { pixelFormat, r.getWidth(), r.getHeight(), true };
    
    juce::Graphics g( background );
    
    //==============================================================================
    
    auto rf = r.toFloat();
    
    float minXY      = juce::jmin( rf.getWidth(), rf.getHeight() );
    float padding    = minXY * 0.1f;
    float circleSize = minXY - padding;
    
    circleBounds = rf.withSizeKeepingCentre(circleSize, circleSize);
    float radius = circleBounds.getWidth() * 0.5f;
    
    //==============================================================================
    
    float labelSize       = 20.f;
    float halfLabelSize   = labelSize / 2.f;
    float radiusPlusLabel = radius + labelSize;
    
    //==============================================================================
    
    float lineThickness = 1.f;
    g.setColour( juce::Colours::white );
    g.drawEllipse( circleBounds, lineThickness + 1.f );
    
    //==============================================================================
    
    juce::Point<float> centre = circleBounds.getCentre();
    auto centreLeft           = centre.translated( -radius, 0.f );
    auto centreRight          = centre.translated( radius, 0.f );

    juce::Point<float> labelOffset{ -labelSize, 0.f };
    juce::Line<float> diameterLine{ centreLeft + labelOffset, centreRight };
    
    //==============================================================================

    float angleInRadians = juce::degreesToRadians( 45.f );
    auto angleTransform  = juce::AffineTransform::rotation( angleInRadians, circleBounds.getCentreX(), circleBounds.getCentreY() );
    
    //==============================================================================
    
    g.setFont( juce::Font{18.f} );
    
    for( auto& label : labels )
    {
        
        float xOffset = juce::jmap( diameterLine.getStartX() - centre.x,
                                    radiusPlusLabel, -radiusPlusLabel,
                                    -(halfLabelSize - 3.f), halfLabelSize - 3.f );

        float yOffset = juce::jmap( diameterLine.getStartY() - centre.y,
                                    0.f, radius,
                                    0.f, -halfLabelSize );
        
        float labelXPos = diameterLine.getStartX() + (xOffset - halfLabelSize);
        float labelYPos = diameterLine.getStartY() + (yOffset - halfLabelSize);
        
        g.drawText( label, labelXPos, labelYPos, labelSize, labelSize, juce::Justification::centred );
        g.drawLine( diameterLine.withShortenedStart(labelSize), lineThickness );
        
        diameterLine.applyTransform( angleTransform );
    }
    
    
    float pathYOffset  = (r.getHeight() - circleBounds.getHeight()) * 0.5f;
    float pathXOffset  = (r.getWidth()  - circleBounds.getWidth())  * 0.5f;
    
    pathOffsetPoint = juce::Point<float> {pathXOffset, pathYOffset};
}

void Goniometer::update( const juce::AudioBuffer<float>& newBuffer )
{
    const float* leftChannel  = newBuffer.getReadPointer(0);
    const float* rightChannel = newBuffer.getReadPointer(1);
    
    path.clear();
    path.startNewSubPath( leftRightToMidSidePoint(leftChannel[0], rightChannel[0]) );
    
    int increment = std::floor( newBuffer.getNumSamples() / 256 );
    
    for( int sample = 1; sample < newBuffer.getNumSamples(); sample += increment )
        path.lineTo( leftRightToMidSidePoint(leftChannel[sample], rightChannel[sample]) );
    
    path.closeSubPath();
    repaint();
}

juce::Point<float> Goniometer::leftRightToMidSidePoint(float leftSample, float rightSample)
{
    const float mid           = (leftSample + rightSample) * minus3dB;
    const float sides         = (leftSample - rightSample) * minus3dB;
    const float midInPixels   = juce::jmap( mid,   1.f, -1.f, 0.f, circleBounds.getHeight() );
    const float sidesInPixels = juce::jmap( sides, 1.f, -1.f, 0.f, circleBounds.getWidth() );
    
    return pathOffsetPoint + juce::Point<float> {sidesInPixels, midInPixels};
}

//==============================================================================

CorrelationMeter::CorrelationMeter( double sampleRate )
{
    auto c = juce::dsp::IIR::Coefficients<float>::makeLowPass (sampleRate, 100.f);
    for( auto& filter : filters )
        filter = juce::dsp::IIR::Filter<float>(c);
}

juce::Rectangle<int> CorrelationMeter::trimRect( juce::Rectangle<int> r, const float& value )
{
    if( value >= 0.f )
    {
        r.removeFromLeft( r.getWidth() * 0.5 );
        int sliceFromRight = juce::jmap( value, 0.f, 1.f, (float)r.getWidth(), 0.f );
        r.removeFromRight( sliceFromRight );
    }
    
    else
    {
        r.setWidth( r.getWidth() * 0.5 );
        int sliceFromLeft = juce::jmap( value, -1.f, 0.f, 0.f, (float)r.getWidth() );
        r.removeFromLeft( sliceFromLeft );
    }
    
    return r;
}

void CorrelationMeter::paint(juce::Graphics& g)
{
    g.setColour( juce::Colours::pink );
    g.fillRect( trimRect(averageMeterBounds, averager.getAverage()) );
    g.fillRect( trimRect(instantMeterBounds, instantCorrelation) );
    
    auto r = getLocalBounds().reduced( getLocalBounds().getWidth() * 0.03 , 0 );
    g.setColour( juce::Colours::whitesmoke );
    g.setFont( juce::Font{16.f} );
    g.drawText( "+1", r, juce::Justification::right);
    g.drawText( "-1", r, juce::Justification::left);
    g.drawRect( meterBounds );
    g.drawLine( meterBounds.getCentreX(), 0, meterBounds.getCentreX(), meterBounds.getHeight() );
}

void CorrelationMeter::resized()
{
    auto r             = getLocalBounds();
    meterBounds        = r.withSizeKeepingCentre( r.getWidth() * 0.8, r.getHeight() );
    const int padding  = 4;
    auto meterCopy     = meterBounds.reduced( 0, padding );
    averageMeterBounds = meterCopy.removeFromTop( meterBounds.getHeight() * 0.2 );
    instantMeterBounds = meterCopy.withTrimmedTop( padding );
}

#include <limits>

void CorrelationMeter::update( juce::AudioBuffer<float>& buffer )
{
    auto leftChannel  = buffer.getReadPointer(0);
    auto rightChannel = buffer.getReadPointer(1);
    
    float sum      = 0.f;
    int numSamples = buffer.getNumSamples();
    
    for( int sample = 0; sample < numSamples; ++sample )
    {
        float left  = leftChannel[sample];
        float right = rightChannel[sample];
        
        float numerator   = filters[0].processSample( left * right );
        float denominator = std::sqrt( filters[1].processSample(left*left) * filters[2].processSample(right*right) );
        float c_t         = numerator / denominator;
        
        if( std::isnan(c_t) || std::isinf(c_t) || std::abs(c_t) <= std::numeric_limits<float>::epsilon() )
            c_t = 0.f;
        
        sum += c_t;
    }
    
    instantCorrelation = sum / (float)numSamples;
    averager.add( instantCorrelation );
    repaint();
}

//==============================================================================

StereoImageMeter::StereoImageMeter( double sampleRate ) : correlationMeter( sampleRate )
{
    addAndMakeVisible( goniometer );
    addAndMakeVisible( correlationMeter );
}

void StereoImageMeter::paint(juce::Graphics& g)
{
    g.fillAll( juce::Colours::black );
}

void StereoImageMeter::resized()
{
    auto r           = getLocalBounds();
    auto gonioBounds = r.removeFromTop(r.getHeight() * 0.85).reduced(padding);
    
    goniometer.setBounds       ( gonioBounds );
    correlationMeter.setBounds ( r.translated(0, -padding) );
}

void StereoImageMeter::update(juce::AudioBuffer<float>& buffer)
{
    goniometer.update( buffer );
    correlationMeter.update( buffer );
}

//==============================================================================

Pfmcpp_project10AudioProcessorEditor::Pfmcpp_project10AudioProcessorEditor (Pfmcpp_project10AudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    startTimerHz( 20 );
    
    addAndMakeVisible( rmsWidget );
    addAndMakeVisible( peakWidget );
    addAndMakeVisible( histogramWidget );
    addAndMakeVisible( stereoImageMeter );
    
    setLookAndFeel( &lookAndFeel );
    
    // FOR SOME REASON, THIS GETS RID OF THE WHOLE SLIDER
    //rmsThresholdSlider.setTextBoxStyle( juce::Slider::NoTextBox, true, 1, 1 );
    
    rmsThresholdSlider.setSliderStyle( juce::Slider::SliderStyle::LinearVertical );
    peakThresholdSlider.setSliderStyle( juce::Slider::SliderStyle::LinearVertical );
    rmsThresholdSlider.setRange( (double)NEGATIVE_INFINITY_DB, (double)MAX_DB );
    peakThresholdSlider.setRange( (double)NEGATIVE_INFINITY_DB, (double)MAX_DB );
    
    rmsThresholdSlider.onValueChange = [this]()
    {
        DBG ("RMS");
        float t = rmsThresholdSlider.getValue();
        rmsWidget.updateThreshold( t );
        histogramWidget.rmsDisplay.threshold = t;
    };
    
    peakThresholdSlider.onValueChange = [this]()
    {
        DBG ("PEAK");
        float t = peakThresholdSlider.getValue();
        peakWidget.updateThreshold( t );
        histogramWidget.peakDisplay.threshold = t;
    };
    
    addAndMakeVisible( rmsThresholdSlider );
    addAndMakeVisible( peakThresholdSlider );

    setSize (800, 640);
}

Pfmcpp_project10AudioProcessorEditor::~Pfmcpp_project10AudioProcessorEditor()
{
    setLookAndFeel( nullptr );
    stopTimer();
}

void Pfmcpp_project10AudioProcessorEditor::paint (Graphics& g)
{
    g.setColour( juce::Colours::ivory );
    g.fillAll();
}

void Pfmcpp_project10AudioProcessorEditor::resized()
{
    const int padding = 10;
    auto r = getLocalBounds().withSizeKeepingCentre(getWidth() - padding, getHeight() - padding);
    
    auto top = r.removeFromTop(r.getHeight() * 0.6).withTrimmedBottom(padding);
    const int meterWidth = 150;
    
    rmsWidget.setBounds( top.removeFromLeft(meterWidth) );
    rmsThresholdSlider.setBounds( rmsWidget.getDbScaleBounds().translated(padding/2, padding/2) );
    
    peakWidget.setBounds( top.removeFromRight(meterWidth) );
    peakThresholdSlider.setBounds( peakWidget.getDbScaleBounds().translated(padding/2, padding/2) );
    
    stereoImageMeter.setBounds( top.reduced( padding, 0 ) );

    histogramWidget.setBounds( r );
}

void Pfmcpp_project10AudioProcessorEditor::timerCallback()
{
    if( processor.fifo.numberAvailable() > 0 )
    {

        juce::AudioBuffer<float> temp;
        while( processor.fifo.numberAvailable() > 0 )
            processor.fifo.pull( temp );
        
        buffer = temp;
        
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
        
        auto averageRMSdB  = (leftRMSdB + rightRMSdB) * 0.5f;
        auto averagePeakDB = (leftMagnitudeDB + rightMagnitudeDB) * 0.5f;
        
        histogramWidget.update( averageRMSdB, averagePeakDB );
        
        stereoImageMeter.update( buffer );
        
    }
}
