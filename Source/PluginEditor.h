/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

#define NEGATIVE_INFINITY_DB -66.f
#define MAX_DB                12.f

//==============================================================================


struct ValueHolder : Timer
{
    
    ValueHolder();
    ~ValueHolder() override;
    
    void timerCallback() override;
    
    void setThreshold(float newThreshold);
    void updateHeldValue(float input);
    void setHoldTime(int ms);
    float getCurrentValue() const;
    bool isOverThreshold() const;
    
private:
    
    int holdTime{100};
    
    float currentValue{ NEGATIVE_INFINITY_DB },
          threshold{0.f};
    
    juce::int64 peakTime = juce::Time::currentTimeMillis();
    
    void resetCurrentValue() { currentValue = threshold; }
};


//==============================================================================


struct DecayingValueHolder : Timer
{
    DecayingValueHolder();
    ~DecayingValueHolder() override;
    
    void timerCallback() override;
    
    void updateHeldValue(float);
    void setDecayRate(float);
    void setHoldTime(int);
    int getHoldTime();
    
    float getCurrentValue() { return currentValue; }
    
    void reset();

private:
    
    const int timerHz{20};
    int exponent{1}, holdTime{250};
    float currentValue{NEGATIVE_INFINITY_DB},
          decayRateDB{ 6.f / (float)timerHz },
          dx{0.f};
    
    juce::int64 elapsedTime = juce::Time::currentTimeMillis();
};


//==============================================================================


struct Tick
{
    int y{0};
    float db{0.f};
};


//==============================================================================


class Meter : public juce::Component
{
public:
    Meter();
    ~Meter() override = default;

    void paint( juce::Graphics& ) override;
    void resized() override;
    
    void update(float);
    void updateThreshold(float);
    void remakeGradient();
    void updateDecayRate(float);
    void updateTickTime(float);
    
    void resetTick();
    void toggleTick(bool);

private:
    
    float currentLevel{0.f};
    float threshold{MAX_DB};
    
    DecayingValueHolder decayingValueHolder;
    juce::ColourGradient gradient;
    
    friend class MacroMeterWidget;
    bool shouldDrawTick{true};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( Meter )
};


//==============================================================================


class DB_Scale : public juce::Component
{
public:
    DB_Scale() = default;
    ~DB_Scale() override = default;
    
    void paint( juce::Graphics& ) override;
    void resized() override;

    juce::Rectangle<int> displayBounds;
    std::vector<Tick> ticks;
    int yOffset{0};
    
private:
    const float dbStepSize = 6.f;
    const float numberOfSteps = (MAX_DB - NEGATIVE_INFINITY_DB) / dbStepSize;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( DB_Scale )
};


//==============================================================================


class TextMeter : public juce::Component
{
public:
    
    TextMeter() = default;
    ~TextMeter() override = default;
    
    void paint(Graphics&) override;
    void resized() override;
    void update(float);
    ValueHolder valueHolder;
    
private:
    
    const int decimalPlaces = 2;
    float inputValue = NEGATIVE_INFINITY_DB;
    
    
};


//==============================================================================


template<typename T>
struct Averager
{
    Averager( size_t numElements, T initialValue )
    {
        resize( numElements, initialValue );
    }

    void clear( T initialValue )
    {
        elementsToAverage.assign( getSize(), initialValue );
        writeIndex.store(0);
        sumOfElements.store( initialValue * getSize() );
        average.store( initialValue );
    }
    
    void resize( size_t newSize, T initialValue )
    {
        elementsToAverage.resize( newSize );
        clear( initialValue );
    }

    void add( T t )
    {
        T currSum = sumOfElements.load();
        int currWriteIndex = writeIndex.load();
        
        currSum -= elementsToAverage[currWriteIndex];
        currSum += t;
        elementsToAverage[currWriteIndex] = t;
        
        if( ++currWriteIndex == getSize() )
            currWriteIndex = 0;
        
        writeIndex.store( currWriteIndex );
        sumOfElements.store( currSum );
        average.store( (float)currSum / (float)getSize() );
    }
        
    float getAverage() const { return average.load(); }
    size_t getSize() const { return elementsToAverage.size(); }
    
    
private:
    
//    juce::int64 currTime = juce::Time::currentTimeMillis();
//    float duration {100.f};
    
    std::vector<T> elementsToAverage;
    std::atomic<T> sumOfElements{ static_cast<T>(0) };
    std::atomic<int> writeIndex;
    std::atomic<float> average{ 0.f };
    
};


//==============================================================================


struct MacroMeterWidget : juce::Component
{
    
    MacroMeterWidget();

    void paint(juce::Graphics&) override;
    void resized() override;
    
    void update(float);
    void updateThreshold(float);
    void updateDecayRate(float);
    void updateTickTime(float);
    void updateAveragerDuration(int);
    void setDrawType(const juce::String&);
    
    void resetTick();
    void toggleTick(bool);
    
    std::vector<Tick> getTicks();
    int getMeterY();
    
    
private:

    Meter instantMeter, averageMeter;
    TextMeter textMeter;
    Averager<float> averager;
    
    juce::String drawType{"Both"};
};


//==============================================================================


struct StereoMeterWidget : juce::Component
{
    StereoMeterWidget(juce::String);
    
    void paint(juce::Graphics&) override;
    void resized() override;
    
    void update(float, float);
    
    juce::Rectangle<int> getDbScaleBounds();
    
    void updateThreshold(float);
    void updateDecayRate(float);
    void updateTickTime(float);
    void updateAveragerDuration(int);
    
    void resetTick();
    void toggleTick(bool);
    
    void setDrawType(const juce::String&);
    juce::String drawType;
    
private:
    MacroMeterWidget leftMeterWidget, rightMeterWidget;
    DB_Scale dbScale;
    juce::Rectangle<int> labelArea;
    juce::String label;
    
};


//==============================================================================


template <typename T>
struct CircularBuffer
{
    using DataType = std::vector<T>;
    
    CircularBuffer( std::size_t newSize, T fillValue )
    {
        resize( newSize, fillValue );
    }
    
    void resize( std::size_t newSize, T fillValue )
    {
        dataHolder.resize( newSize );
        clear( fillValue );
    }
    
    void clear( T fillValue )
    {
        dataHolder.assign( getSize(), fillValue );
        writeIndex.store(0);
    }
    
    void write( T itemToAdd )
    {
        std::size_t writeInd = writeIndex.load();
        dataHolder[writeInd] = itemToAdd;
        
        if( ++writeInd > getSize() - 1 )
            writeInd = 0;
        
        writeIndex.store( writeInd );
    }
    
    DataType& getData() { return dataHolder; }
    std::size_t getSize() const { return dataHolder.size(); }
    std::size_t getReadIndex() const { return writeIndex.load(); }
    
private:
    DataType dataHolder;
    std::atomic<std::size_t> writeIndex;
};


//==============================================================================


struct HistogramDisplay : juce::Component
{
    HistogramDisplay(std::size_t, juce::String);
    
    void paint(juce::Graphics&) override;
    void resized() override;
    
    void update(float);
    void remakeGradient();
    void updateThreshold(float);

private:
    CircularBuffer<float> buffer;
    juce::String label;
    juce::ColourGradient gradient;
    float threshold{0.f};
};


//==============================================================================


struct HistogramWidget : juce::Component
{
    HistogramWidget();
    
    void paint(juce::Graphics&) override;
    void resized() override;
    
    void update(float, float);
    
    const std::size_t bufferSize{64};
    HistogramDisplay rmsDisplay{bufferSize, "RMS"}, peakDisplay{bufferSize, "PEAK"};
    
    void setLayout(const juce::String&);
    
    juce::String layout;
};


//==============================================================================


struct Goniometer : public juce::Component
{    
    void paint(juce::Graphics&) override;
    void resized() override;
    
    void update(const juce::AudioBuffer<float>&);
    void setScale(float);
    
private:
    
    juce::Path path;
    juce::Point<float> pathOffsetPoint;
    juce::Image background;
    juce::Rectangle<float> circleBounds;
    std::vector<juce::String> labels = {"+S", "L", "M", "R", "-S"};
    
    const float minus3dB = juce::Decibels::decibelsToGain(-3.f);
    juce::Point<float> leftRightToMidSidePoint(float, float);
    
    float scale;
};

//==============================================================================

class CorrelationMeter : public juce::Component
{
public:
    CorrelationMeter(double);
    
    void paint(juce::Graphics&) override;
    void resized() override;
    
    void update(juce::AudioBuffer<float>&);
    
private:
    using FilterType = juce::dsp::IIR::Filter<float>;
    std::array<FilterType, 3> filters;
    float instantCorrelation;
    
    Averager<float> averager {10, 0.f};
    juce::Rectangle<int> meterBounds;
    juce::Rectangle<int> averageMeterBounds;
    juce::Rectangle<int> instantMeterBounds;

    juce::Rectangle<int> trimRect(juce::Rectangle<int> , const float&);
};

//==============================================================================

class StereoImageMeter : public juce::Component
{
public:
    StereoImageMeter(double);
    
    void paint(juce::Graphics&) override;
    void resized() override;
    
    void update(juce::AudioBuffer<float>&);
    void setScale(float);
    
private:
    const int padding = 10;
    double sampleRate;
    Goniometer goniometer;
    CorrelationMeter correlationMeter;
};

//==============================================================================

class PFMLookAndFeel : public juce::LookAndFeel_V4
{
    void drawLinearSlider( Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const Slider::SliderStyle style, juce::Slider& slider) override
    {
        auto r = slider.getBounds().toFloat();
        const float thickness = 2.5f;
        const float yPos = juce::jmap((float)slider.getValue(), NEGATIVE_INFINITY_DB, MAX_DB, r.getHeight(), 0.f);
        
        g.setColour( juce::Colours::red );
        g.drawLine( 0.f, yPos, r.getWidth(), yPos, thickness );
    }
    
    void drawComboBox( juce::Graphics& g, int width, int height, bool isButtonDown,
                       int bx, int by, int bw, int bh, juce::ComboBox& box ) override
{
//    const int boxHeight = box.getHeight() * 0.4;
//    auto boxBounds = box.getLocalBounds().withHeight(boxHeight).translated(0, boxHeight);
//
    auto boxBounds = box.getLocalBounds();
    g.setColour(box.findColour (ComboBox::backgroundColourId));
    g.fillRect(boxBounds);

    if (box.isEnabled() && box.hasKeyboardFocus (false))
    {
        g.setColour (box.findColour (ComboBox::focusedOutlineColourId));
        g.drawRect (boxBounds, 2);
    }
    else
    {
        g.setColour (box.findColour (ComboBox::outlineColourId));
        g.drawRect (boxBounds);
    }

    auto outlineThickness = box.isEnabled() ? (isButtonDown ? 1.2f : 0.5f) : 0.3f;

    auto baseColour = juce::Colours::darkslategrey.withMultipliedAlpha (box.isEnabled() ? 1.0f : 0.5f);

    drawGlassLozenge (g,
                      (float) bx + outlineThickness,        (float) by + outlineThickness,
                      (float) bw - outlineThickness * 2.0f, (float) bh - outlineThickness * 2.0f,
                      baseColour, outlineThickness, -1.0f,
                      true, true, true, true);

    if (box.isEnabled())
    {
        const float arrowX = 0.3f;
        const float arrowH = 0.2f;

        const auto x = (float) bx;
        const auto y = (float) by;
        const auto w = (float) bw;
        const auto h = (float) bh;

        Path p;
        p.addTriangle (x + w * 0.5f,            y + h * (0.45f - arrowH),
                       x + w * (1.0f - arrowX), y + h * 0.45f,
                       x + w * arrowX,          y + h * 0.45f);

        p.addTriangle (x + w * 0.5f,            y + h * (0.55f + arrowH),
                       x + w * (1.0f - arrowX), y + h * 0.55f,
                       x + w * arrowX,          y + h * 0.55f);

        g.setColour (box.findColour (ComboBox::arrowColourId));
        g.fillPath (p);
    }
}
    
};



struct ThresholdSlider : juce::Slider
{
    ThresholdSlider()
    {
        setRange( (double)NEGATIVE_INFINITY_DB, (double)MAX_DB );
        setSliderStyle( juce::Slider::SliderStyle::LinearVertical );
        setTextBoxStyle( juce::Slider::NoTextBox, true, 1, 1 );
    }
};

struct ScaleKnob : juce::Slider
{
    ScaleKnob()
    {
        setRange( 0.5, 2.0 );
        setSliderStyle( juce::Slider::SliderStyle::Rotary );
        setTextBoxStyle( juce::Slider::NoTextBox, true, 1, 1 );
    }
};

struct PFMComboBox : juce::ComboBox
{
    PFMComboBox( const juce::String& name ) : ComboBox(name)
    {
        setEditableText( false );
        setJustificationType( juce::Justification::centred );
    }
};

struct PFMButton : public juce::Button
{
    
};

//==============================================================================

class Pfmcpp_project10AudioProcessorEditor  : public AudioProcessorEditor, public Timer
{
public:
    Pfmcpp_project10AudioProcessorEditor (Pfmcpp_project10AudioProcessor&);
    ~Pfmcpp_project10AudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Pfmcpp_project10AudioProcessor& processor;

    AudioBuffer<float> buffer;
    
    StereoMeterWidget rmsWidget{"RMS"}, peakWidget{"PEAK"};
    HistogramWidget histogramWidget;
    StereoImageMeter stereoImageMeter{ processor.getSampleRate() };
    
    PFMLookAndFeel lookAndFeel;
    ThresholdSlider rmsThresholdSlider;
    ThresholdSlider peakThresholdSlider;
    
    PFMComboBox decayRateBox{"Decay Rate"};
    PFMComboBox averagerDurationBox{"Averager Duration"};
    PFMComboBox meterViewBox{"Meter View"};
    
    ScaleKnob scaleKnob;
    
    PFMComboBox tickHoldTimeBox{"Tick Hold Time"};
    juce::TextButton resetTickButton{"Reset Tick"};
    juce::ToggleButton showTickButton{"Show Tick"};
    
    PFMComboBox histogramViewBox{"Histogram View"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pfmcpp_project10AudioProcessorEditor)
};
