#include "PluginEditor.h"
#include "BinaryData.h"

juce::Typeface::Ptr hyu::dotFont()
{
    static juce::Typeface::Ptr tf = juce::Typeface::createSystemTypefaceFor (
        BinaryData::DotGothic16Regular_ttf, BinaryData::DotGothic16Regular_ttfSize);
    return tf;
}

KoeLook::KoeLook()
{
    setColour (juce::Label::textColourId, hyu::ink);
    setColour (juce::Slider::textBoxTextColourId, hyu::ink);
    setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour (juce::ComboBox::textColourId, hyu::ink);
    setColour (juce::PopupMenu::backgroundColourId, hyu::paper2);
    setColour (juce::PopupMenu::textColourId, hyu::ink);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, hyu::pink);
    setColour (juce::PopupMenu::highlightedTextColourId, hyu::paper);
}

void KoeLook::drawComboBox (juce::Graphics& g, int w, int h, bool,
                            int, int, int, int, juce::ComboBox&)
{
    auto r = juce::Rectangle<float> (0, 0, (float) w, (float) h);
    g.setColour (hyu::paper2); g.fillRect (r);
    g.setColour (hyu::ink);    g.drawRect (r, 2.0f);
    const float bx = (float) w - 16.0f, by = (float) h * 0.5f - 3.0f;
    g.setColour (hyu::pink);
    g.fillRect (bx,        by,        9.0f, 3.0f);
    g.fillRect (bx + 1.5f, by + 3.0f, 6.0f, 3.0f);
    g.fillRect (bx + 3.0f, by + 6.0f, 3.0f, 3.0f);
}

void KoeLook::drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h,
                                float pos, float a0, float a1, juce::Slider&)
{
    auto r = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h)
                 .reduced (6.0f).withSizeKeepingCentre (
                     juce::jmin (w, h) - 12.0f, juce::jmin (w, h) - 12.0f);
    g.setColour (hyu::paper2);  g.fillRect (r);
    g.setColour (hyu::ink);     g.drawRect (r, 2.0f);

    g.setColour (hyu::rose);
    for (int i = 0; i <= 8; ++i)
    {
        const float ang = a0 + (a1 - a0) * (float) i / 8.0f;
        const auto p = r.getCentre().getPointOnCircumference (r.getWidth() * 0.58f, ang);
        g.fillRect (p.x - 1.5f, p.y - 1.5f, 3.0f, 3.0f);
    }

    const float ang = a0 + (a1 - a0) * pos;
    const auto c = r.getCentre();
    g.setColour (hyu::pink);
    for (int i = 2; i < 9; ++i)
    {
        const auto p = c.getPointOnCircumference (r.getWidth() * 0.05f * (float) i, ang);
        g.fillRect (p.x - 2.5f, p.y - 2.5f, 5.0f, 5.0f);
    }
    g.setColour (hyu::ink);
    g.fillRect (c.x - 3.0f, c.y - 3.0f, 6.0f, 6.0f);
}

// ── /halt/ の子。声で、くちがひらく ─────────────────────────────
void KoeKid::paint (juce::Graphics& g)
{
    const float px = juce::jmin ((float) getWidth() / 26.0f, (float) getHeight() / 24.0f);
    const float cx = getWidth() * 0.5f, cy = getHeight() * 0.52f;
    auto R = [&] (float x, float y, float w, float h, juce::Colour c)
    {
        g.setColour (c);
        g.fillRect (cx + x * px, cy + y * px, w * px, h * px);
    };
    const float lvl = juce::jlimit (0.0f, 1.0f, proc.uiOutLevel.load() * 3.0f);
    const bool blink = (tick % 60) < 3;

    R (-9, -9, 18, 18, hyu::ink); R (-11, -5, 22, 11, hyu::ink);
    R (-10, 1, 4, 3, hyu::pink); R (6, 1, 4, 3, hyu::pink);
    if (blink) { R (-7, -1, 5, 1, hyu::paper); R (2, -1, 5, 1, hyu::paper); }
    else       { R (-7, -3, 4, 4, hyu::paper); R (3, -3, 4, 4, hyu::paper); }
    // くち：声の大きさでひらく
    const float mh = 1.0f + 3.0f * lvl;
    R (-2, 3, 4, mh, hyu::paper);
}

// ── まとめ ──────────────────────────────────────────────────────
KoeSysEditor::KoeSysEditor (KoeSysProcessor& p)
    : AudioProcessorEditor (p), proc (p), kid (p),
      pitchAtt (p.apvts, "pitch",   pitchSlider),
      formAtt  (p.apvts, "formant", formSlider),
      mixAtt   (p.apvts, "mix",     mixSlider)
{
    setLookAndFeel (&look);

    title.setText ("koe.sys", juce::dontSendNotification);
    title.setFont (juce::FontOptions (26.0f));
    addAndMakeVisible (title);

    subtitle.setText (juce::String::fromUTF8 ("こえも、規則がきめる。"), juce::dontSendNotification);
    subtitle.setFont (juce::FontOptions (13.0f));
    subtitle.setColour (juce::Label::textColourId, hyu::rose);
    addAndMakeVisible (subtitle);

    addAndMakeVisible (kid);

    for (auto* s : { &pitchSlider, &formSlider, &mixSlider })
    {
        s->setSliderStyle (juce::Slider::RotaryVerticalDrag);
        s->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 16);
        addAndMakeVisible (*s);
    }
    pitchLabel.setText (juce::String::fromUTF8 ("たかさ"), juce::dontSendNotification);
    formLabel.setText (juce::String::fromUTF8 ("かたち"), juce::dontSendNotification);
    mixLabel.setText ("mix", juce::dontSendNotification);
    speedLabel.setText (juce::String::fromUTF8 ("はやさ"), juce::dontSendNotification);
    for (auto* l : { &pitchLabel, &formLabel, &mixLabel, &speedLabel })
    {
        l->setJustificationType (juce::Justification::centred);
        addAndMakeVisible (*l);
    }

    speedBox.addItemList (juce::StringArray { juce::String::fromUTF8 ("おと優先"),
                                              juce::String::fromUTF8 ("はやい"),
                                              juce::String::fromUTF8 ("さいそく") }, 1);
    speedBox.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (speedBox);
    speedAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        p.apvts, "speed", speedBox);                              // 選択肢を入れてから繋ぐ

    latLabel.setJustificationType (juce::Justification::centredRight);
    latLabel.setColour (juce::Label::textColourId, hyu::rose);
    addAndMakeVisible (latLabel);
    startTimerHz (2);
    timerCallback();

    setSize (460, 344);
}

void KoeSysEditor::timerCallback()
{
    latLabel.setText (juce::String::fromUTF8 ("遅延 ")
                          + juce::String (proc.uiLatencyMs.load(), 0)
                          + " ms",
                      juce::dontSendNotification);
}

KoeSysEditor::~KoeSysEditor()
{
    setLookAndFeel (nullptr);
}

void KoeSysEditor::paint (juce::Graphics& g)
{
    g.fillAll (hyu::paper);
    g.setColour (hyu::rose);
    g.drawRect (getLocalBounds(), 3);
}

void KoeSysEditor::resized()
{
    auto area = getLocalBounds().reduced (14);

    auto top = area.removeFromTop (46);
    kid.setBounds (top.removeFromRight (70));
    title.setBounds (top.removeFromLeft (140));
    subtitle.setBounds (top.withTrimmedTop (14));
    area.removeFromTop (6);

    auto bottom = area.removeFromBottom (40);
    area.removeFromBottom (4);

    auto row = area;
    const int w = row.getWidth() / 3;
    auto cell = [&row, w] { return row.removeFromLeft (w).reduced (8); };

    auto c1 = cell(); pitchLabel.setBounds (c1.removeFromTop (20)); pitchSlider.setBounds (c1);
    auto c2 = cell(); formLabel.setBounds (c2.removeFromTop (20));  formSlider.setBounds (c2);
    auto c3 = cell(); mixLabel.setBounds (c3.removeFromTop (20));   mixSlider.setBounds (c3);

    speedLabel.setBounds (bottom.removeFromLeft (64));
    speedBox.setBounds (bottom.removeFromLeft (140).reduced (0, 6));
    latLabel.setBounds (bottom.reduced (6, 0));
}
