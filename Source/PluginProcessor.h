#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "signalsmith-stretch.h"

// こえ.sys ── たかさ と かたち。ふたつのつまみで、こえが変わる。
class KoeSysProcessor : public juce::AudioProcessor,
                        private juce::AudioProcessorValueTreeState::Listener,
                        private juce::AsyncUpdater
{
public:
    KoeSysProcessor();
    ~KoeSysProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override                    { return true; }

    const juce::String getName() const override        { return "koe.sys"; }
    bool acceptsMidi() const override                  { return false; }
    bool producesMidi() const override                 { return false; }
    double getTailLengthSeconds() const override       { return 0.1; }

    int getNumPrograms() override                      { return 1; }
    int getCurrentProgram() override                   { return 0; }
    void setCurrentProgram (int) override              {}
    const juce::String getProgramName (int) override   { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

    // UI 表示用
    std::atomic<float> uiInLevel { 0.0f }, uiOutLevel { 0.0f };
    std::atomic<float> uiLatencyMs { 0.0f };

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
    void parameterChanged (const juce::String& id, float) override;
    void handleAsyncUpdate() override;
    void applySpeedMode();

    double sr = 48000.0;
    signalsmith::stretch::SignalsmithStretch<float> stretch;
    juce::AudioBuffer<float> wet;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KoeSysProcessor)
};
