#pragma once
#include "PluginProcessor.h"

class EQAIMixVocalsFullAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit EQAIMixVocalsFullAudioProcessorEditor (EQAIMixVocalsFullAudioProcessor&);
    ~EQAIMixVocalsFullAudioProcessorEditor() override = default;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    EQAIMixVocalsFullAudioProcessor& audioProcessor;
    juce::Image background;
    juce::Slider ai, clarity, warmth, deess, mud, air, dynamic, output, threshold;
    juce::ToggleButton autoButton { "AI AUTO" };
    juce::ComboBox preset;
    std::unique_ptr<SliderAttachment> aiAtt, clarityAtt, warmthAtt, deessAtt, mudAtt, airAtt, dynamicAtt, outputAtt, thresholdAtt;
    std::unique_ptr<ButtonAttachment> autoAtt;
    void styleSlider (juce::Slider&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQAIMixVocalsFullAudioProcessorEditor)
};
