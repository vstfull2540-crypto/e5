#pragma once
#include <JuceHeader.h>

class EQAIMixVocalsFullAudioProcessor final : public juce::AudioProcessor
{
public:
    EQAIMixVocalsFullAudioProcessor();
    ~EQAIMixVocalsFullAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}
    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    using APVTS = juce::AudioProcessorValueTreeState;
    APVTS parameters;

    static APVTS::ParameterLayout createParameterLayout();
    void runAutoMix();

private:
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> lowCut, mudCut, bodyEq, clarityEq, airEq, deEsser;
    juce::dsp::Compressor<float> compressor;
    juce::SmoothedValue<float> outputGain;
    double currentSampleRate = 44100.0;
    std::atomic<float> inputRms { 0.0f };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQAIMixVocalsFullAudioProcessor)
};
