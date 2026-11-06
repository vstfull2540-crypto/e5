#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
    auto param (const char* id, const char* name, float min, float max, float def, float step = 0.01f)
    {
        return std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { id, 1 }, name,
            juce::NormalisableRange<float> { min, max, step }, def);
    }
}

EQAIMixVocalsFullAudioProcessor::EQAIMixVocalsFullAudioProcessor()
    : AudioProcessor (BusesProperties()
        .withInput ("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

EQAIMixVocalsFullAudioProcessor::APVTS::ParameterLayout EQAIMixVocalsFullAudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout layout;
    layout.add (param ("ai", "AI Strength", 0.0f, 1.0f, 0.85f));
    layout.add (param ("clarity", "Vocal Clarity", 0.0f, 1.0f, 0.50f));
    layout.add (param ("warmth", "Vocal Warmth", 0.0f, 1.0f, 0.60f));
    layout.add (param ("deess", "De-Esser", 0.0f, 1.0f, 0.70f));
    layout.add (param ("mud", "Mud Removal", 0.0f, 1.0f, 0.65f));
    layout.add (param ("air", "Presence & Air", 0.0f, 1.0f, 0.55f));
    layout.add (param ("dynamic", "Dynamic EQ", 0.0f, 1.0f, 0.70f));
    layout.add (param ("threshold", "Compressor Threshold", -36.0f, 0.0f, -18.0f, 0.1f));
    layout.add (param ("ratio", "Compressor Ratio", 1.0f, 8.0f, 3.0f, 0.1f));
    layout.add (param ("output", "Output Gain", -18.0f, 18.0f, 0.0f, 0.1f));
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { "auto", 1 }, "AI Auto Mix", true));
    return layout;
}

void EQAIMixVocalsFullAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock),
                                 static_cast<juce::uint32> (getTotalNumOutputChannels()) };
    lowCut.prepare (spec); mudCut.prepare (spec); bodyEq.prepare (spec);
    clarityEq.prepare (spec); airEq.prepare (spec); deEsser.prepare (spec);
    compressor.prepare (spec);
    outputGain.reset (sampleRate, 0.05);
    outputGain.setCurrentAndTargetValue (1.0f);
    auto resetFilter = [] (auto& f) { f.reset(); };
    resetFilter (lowCut); resetFilter (mudCut); resetFilter (bodyEq);
    resetFilter (clarityEq); resetFilter (airEq); resetFilter (deEsser);
}

void EQAIMixVocalsFullAudioProcessor::releaseResources() {}

bool EQAIMixVocalsFullAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto in = layouts.getMainInputChannelSet();
    const auto out = layouts.getMainOutputChannelSet();
    return (in == juce::AudioChannelSet::mono() || in == juce::AudioChannelSet::stereo())
        && in == out;
}

void EQAIMixVocalsFullAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int channels = getTotalNumInputChannels();
    for (int ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear (ch, 0, buffer.getNumSamples());

    const float ai = parameters.getRawParameterValue ("ai")->load();
    const float clarity = parameters.getRawParameterValue ("clarity")->load();
    const float warmth = parameters.getRawParameterValue ("warmth")->load();
    const float deess = parameters.getRawParameterValue ("deess")->load();
    const float mud = parameters.getRawParameterValue ("mud")->load();
    const float air = parameters.getRawParameterValue ("air")->load();
    const float dynamic = parameters.getRawParameterValue ("dynamic")->load();
    const float threshold = parameters.getRawParameterValue ("threshold")->load();
    const float ratio = parameters.getRawParameterValue ("ratio")->load();

    float rms = 0.0f;
    for (int ch = 0; ch < channels; ++ch)
        rms += buffer.getRMSLevel (ch, 0, buffer.getNumSamples());
    rms /= juce::jmax (1, channels);
    inputRms.store (rms);

    const bool autoOn = parameters.getRawParameterValue ("auto")->load() > 0.5f;
    const float autoScale = autoOn ? ai : 0.0f;
    const float clarityDb = (clarity * 5.0f + autoScale * 1.5f) * (0.5f + dynamic * 0.5f);
    const float warmthDb = warmth * 3.0f;
    const float mudDb = -(mud * 5.0f + autoScale * 1.0f);
    const float airDb = air * 5.0f + autoScale * 1.5f;
    const float essCutDb = -(deess * 7.0f);

    *lowCut.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass (currentSampleRate, 75.0);
    *mudCut.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter (currentSampleRate, 280.0, 0.9f, juce::Decibels::decibelsToGain (mudDb));
    *bodyEq.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter (currentSampleRate, 180.0, 0.8f, juce::Decibels::decibelsToGain (warmthDb));
    *clarityEq.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter (currentSampleRate, 3200.0, 0.9f, juce::Decibels::decibelsToGain (clarityDb));
    *airEq.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf (currentSampleRate, 10500.0, 0.7f, juce::Decibels::decibelsToGain (airDb));
    *deEsser.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter (currentSampleRate, 6800.0, 1.2f, juce::Decibels::decibelsToGain (essCutDb));

    compressor.setThreshold (threshold);
    compressor.setRatio (ratio);
    compressor.setAttack (12.0f);
    compressor.setRelease (100.0f);

    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);
    lowCut.process (context);
    mudCut.process (context);
    bodyEq.process (context);
    clarityEq.process (context);
    airEq.process (context);
    deEsser.process (context);
    compressor.process (context);

    const float gain = juce::Decibels::decibelsToGain (parameters.getRawParameterValue ("output")->load());
    outputGain.setTargetValue (gain);
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        const float g = outputGain.getNextValue();
        for (int ch = 0; ch < channels; ++ch)
            buffer.setSample (ch, sample, buffer.getSample (ch, sample) * g);
    }
}

void EQAIMixVocalsFullAudioProcessor::runAutoMix()
{
    // Lightweight adaptive starting-point analysis. The controls remain editable by the user.
    const float level = inputRms.load();
    const float target = juce::jlimit (0.45f, 0.95f, 0.82f - level * 0.7f);
    if (auto* p = parameters.getParameter ("ai"))
        p->setValueNotifyingHost (p->convertTo0to1 (target));
    if (auto* p = parameters.getParameter ("auto"))
        p->setValueNotifyingHost (1.0f);
}

juce::AudioProcessorEditor* EQAIMixVocalsFullAudioProcessor::createEditor()
{
    return new EQAIMixVocalsFullAudioProcessorEditor (*this);
}

void EQAIMixVocalsFullAudioProcessor::getStateInformation (juce::MemoryBlock& dest)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, dest);
}

void EQAIMixVocalsFullAudioProcessor::setStateInformation (const void* data, int size)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, size));
    if (xml != nullptr && xml->hasTagName (parameters.state.getType()))
        parameters.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EQAIMixVocalsFullAudioProcessor();
}
