#include "PluginProcessor.h"
#include "PluginEditor.h"

KoeSysProcessor::KoeSysProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "params", createLayout())
{
    apvts.addParameterListener ("speed", this);
}

KoeSysProcessor::~KoeSysProcessor()
{
    apvts.removeParameterListener ("speed", this);
}

juce::AudioProcessorValueTreeState::ParameterLayout KoeSysProcessor::createLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "pitch", 1 }, "Takasa",
        juce::NormalisableRange<float> (-12.0f, 12.0f, 0.1f), 4.5f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "formant", 1 }, "Katachi",
        juce::NormalisableRange<float> (-12.0f, 12.0f, 0.1f), 1.5f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "mix", 1 }, "Mix",
        juce::NormalisableRange<float> (0.0f, 1.0f), 1.0f));

    layout.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { "speed", 1 }, "Hayasa",
        juce::StringArray { juce::String::fromUTF8 ("おと優先"),
                            juce::String::fromUTF8 ("はやい"),
                            juce::String::fromUTF8 ("さいそく") }, 1));

    return layout;
}

void KoeSysProcessor::parameterChanged (const juce::String& id, float)
{
    if (id == "speed")
        triggerAsyncUpdate();
}

void KoeSysProcessor::handleAsyncUpdate()
{
    suspendProcessing (true);
    applySpeedMode();
    suspendProcessing (false);
}

void KoeSysProcessor::applySpeedMode()
{
    // 窓の長さと間隔。短いほど速いが、低い声の情報が減る
    static constexpr double blockSec[3]    = { 0.120, 0.050, 0.030 };
    static constexpr double intervalSec[3] = { 0.030, 0.0125, 0.0075 };
    const int m = juce::jlimit (0, 2, (int) apvts.getRawParameterValue ("speed")->load());
    stretch.configure (2, (int) (sr * blockSec[m]), (int) (sr * intervalSec[m]));
    const int lat = stretch.inputLatency() + stretch.outputLatency();
    setLatencySamples (lat);
    uiLatencyMs.store ((float) (lat * 1000.0 / sr));
}

void KoeSysProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sr = sampleRate;
    applySpeedMode();
    wet.setSize (2, juce::jmax (16, samplesPerBlock));
}

bool KoeSysProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet()
        && (layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
         || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo());
}

void KoeSysProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numCh = juce::jmin (2, buffer.getNumChannels());
    const int n = buffer.getNumSamples();
    if (n == 0) return;

    const float pitch = apvts.getRawParameterValue ("pitch")->load();
    const float form  = apvts.getRawParameterValue ("formant")->load();
    const float mix   = apvts.getRawParameterValue ("mix")->load();

    // 声向け：高すぎる成分は音程シフトから守る（tonalityLimit）
    stretch.setTransposeSemitones (pitch, 8000.0f / (float) sr);
    stretch.setFormantFactor (std::pow (2.0f, form / 12.0f));

    uiInLevel.store (buffer.getMagnitude (0, n));

    if (wet.getNumSamples() < n) wet.setSize (2, n, false, false, true);

    // モノ入力はステレオに複製してから通す
    const float* ins[2];
    ins[0] = buffer.getReadPointer (0);
    ins[1] = numCh > 1 ? buffer.getReadPointer (1) : buffer.getReadPointer (0);
    float* outs[2] = { wet.getWritePointer (0), wet.getWritePointer (1) };

    stretch.process (ins, n, outs, n);

    for (int ch = 0; ch < numCh; ++ch)
    {
        auto* d = buffer.getWritePointer (ch);
        const auto* w = wet.getReadPointer (ch);
        for (int i = 0; i < n; ++i)
            d[i] = d[i] + mix * (w[i] - d[i]);
    }

    uiOutLevel.store (buffer.getMagnitude (0, n));
}

void KoeSysProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void KoeSysProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessorEditor* KoeSysProcessor::createEditor()
{
    return new KoeSysEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new KoeSysProcessor();
}
