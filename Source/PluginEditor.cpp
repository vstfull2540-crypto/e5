#include "PluginEditor.h"
#include "BinaryData.h"

EQAIMixVocalsFullAudioProcessorEditor::EQAIMixVocalsFullAudioProcessorEditor (EQAIMixVocalsFullAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    background = juce::ImageCache::getFromMemory (PluginAssets::EQ_AI_Mix_Vocals_Full_UI_png,
                                                   PluginAssets::EQ_AI_Mix_Vocals_Full_UI_pngSize);
    setSize (1152, 768);
    auto setup = [this] (juce::Slider& s, const juce::String& id)
    {
        styleSlider (s);
        addAndMakeVisible (s);
    };
    setup (ai, "ai"); setup (clarity, "clarity"); setup (warmth, "warmth");
    setup (deess, "deess"); setup (mud, "mud"); setup (air, "air");
    setup (dynamic, "dynamic"); setup (output, "output"); setup (threshold, "threshold");

    aiAtt = std::make_unique<SliderAttachment> (p.parameters, "ai", ai);
    clarityAtt = std::make_unique<SliderAttachment> (p.parameters, "clarity", clarity);
    warmthAtt = std::make_unique<SliderAttachment> (p.parameters, "warmth", warmth);
    deessAtt = std::make_unique<SliderAttachment> (p.parameters, "deess", deess);
    mudAtt = std::make_unique<SliderAttachment> (p.parameters, "mud", mud);
    airAtt = std::make_unique<SliderAttachment> (p.parameters, "air", air);
    dynamicAtt = std::make_unique<SliderAttachment> (p.parameters, "dynamic", dynamic);
    outputAtt = std::make_unique<SliderAttachment> (p.parameters, "output", output);
    thresholdAtt = std::make_unique<SliderAttachment> (p.parameters, "threshold", threshold);
    autoAtt = std::make_unique<ButtonAttachment> (p.parameters, "auto", autoButton);
    autoButton.onClick = [this] { audioProcessor.runAutoMix(); };
    addAndMakeVisible (autoButton);
    autoButton.setColour (juce::ToggleButton::textColourId, juce::Colours::white);
    autoButton.setColour (juce::ToggleButton::tickColourId, juce::Colour (0xff00baff));
    preset.addItem ("Pop Vocal Clean", 1);
    preset.addItem ("Male Vocal", 2);
    preset.addItem ("Female Vocal", 3);
    preset.addItem ("Rap Vocal", 4);
    preset.addItem ("Rock Vocal", 5);
    preset.addItem ("Podcast", 6);
    preset.setSelectedId (1);
    addAndMakeVisible (preset);
    preset.onChange = [this]
    {
        const int id = preset.getSelectedId();
        const float c = id == 3 ? 0.68f : (id == 4 ? 0.58f : 0.50f);
        const float w = id == 2 ? 0.72f : (id == 5 ? 0.42f : 0.60f);
        audioProcessor.parameters.getParameter ("clarity")->setValueNotifyingHost (
            audioProcessor.parameters.getParameter ("clarity")->convertTo0to1 (c));
        audioProcessor.parameters.getParameter ("warmth")->setValueNotifyingHost (
            audioProcessor.parameters.getParameter ("warmth")->convertTo0to1 (w));
    };
}

void EQAIMixVocalsFullAudioProcessorEditor::styleSlider (juce::Slider& s)
{
    s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 58, 16);
    s.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xff00baff));
    s.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xff253b50));
    s.setColour (juce::Slider::thumbColourId, juce::Colours::white);
    s.setColour (juce::Slider::textBoxTextColourId, juce::Colours::white);
    s.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xdd101820));
    s.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}

void EQAIMixVocalsFullAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff080d14));
    if (background.isValid())
        g.drawImage (background, getLocalBounds().toFloat());
    g.setColour (juce::Colours::white.withAlpha (0.75f));
    g.setFont (12.0f);
    g.drawText ("LIVE PROCESSING • ADJUSTABLE STARTING POINT • NO CLOUD REQUIRED",
                16, getHeight() - 18, getWidth() - 32, 14, juce::Justification::centred);
}

void EQAIMixVocalsFullAudioProcessorEditor::resized()
{
    const float sx = getWidth() / 1536.0f;
    const float sy = getHeight() / 1024.0f;
    auto place = [this, sx, sy] (juce::Component& c, float x, float y, float w, float h)
    {
        c.setBounds (juce::roundToInt (x * sx), juce::roundToInt (y * sy),
                     juce::roundToInt (w * sx), juce::roundToInt (h * sy));
    };
    // Interactive controls are aligned over their matching controls in the PNG artwork.
    place (preset, 1145, 30, 205, 42);
    place (ai, 30, 570, 120, 135);
    place (clarity, 205, 570, 120, 135);
    place (warmth, 375, 570, 120, 135);
    place (deess, 545, 570, 120, 135);
    place (mud, 715, 570, 120, 135);
    place (air, 885, 570, 120, 135);
    place (dynamic, 1055, 570, 120, 135);
    place (threshold, 1350, 555, 120, 135);
    place (output, 1365, 790, 130, 145);
    place (autoButton, 24, 500, 135, 38);
}
