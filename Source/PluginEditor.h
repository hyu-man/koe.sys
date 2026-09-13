#pragma once

#include "PluginProcessor.h"

namespace hyu
{
    const juce::Colour paper  { 0xfff7f0e4 };
    const juce::Colour paper2 { 0xfffff9ee };
    const juce::Colour ink    { 0xff8a3a2e };
    const juce::Colour pink   { 0xffff6b8a };
    const juce::Colour rose   { 0xffd67070 };

    juce::Typeface::Ptr dotFont();
}

class KoeLook : public juce::LookAndFeel_V4
{
public:
    KoeLook();
    juce::Typeface::Ptr getTypefaceForFont (const juce::Font&) override { return hyu::dotFont(); }
    void drawRotarySlider (juce::Graphics&, int x, int y, int w, int h,
                           float pos, float startAngle, float endAngle, juce::Slider&) override;
    void drawComboBox (juce::Graphics&, int w, int h, bool down,
                       int bx, int by, int bw, int bh, juce::ComboBox&) override;
};

// /halt/ の子。声の大きさで、くちが動く。
class KoeKid : public juce::Component,
               private juce::Timer
{
public:
    explicit KoeKid (KoeSysProcessor& p) : proc (p) { startTimerHz (20); }
    void paint (juce::Graphics&) override;

private:
    void timerCallback() override { ++tick; repaint(); }
    KoeSysProcessor& proc;
    int tick = 0;
};

class KoeSysEditor : public juce::AudioProcessorEditor,
                     private juce::Timer
{
public:
    explicit KoeSysEditor (KoeSysProcessor&);
    ~KoeSysEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    KoeSysProcessor& proc;
    KoeLook look;
    KoeKid kid;

    juce::Slider pitchSlider, formSlider, mixSlider;
    juce::ComboBox speedBox;
    juce::Label pitchLabel, formLabel, mixLabel, speedLabel, latLabel, title, subtitle;

    juce::AudioProcessorValueTreeState::SliderAttachment pitchAtt, formAtt, mixAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> speedAtt;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KoeSysEditor)
};
