# EQ AI Mix Vocals Full — VST3

Windows x64 vocal-processing plug-in built with JUCE + CMake.

## Features
- AI Auto Mix starting-point control (local adaptive heuristic; no cloud or trained ML model is included)
- Vocal clarity, warmth, mud reduction, presence/air and de-essing controls
- Parametric filter chain and compressor
- Output gain, preset selector and PNG-based premium interface artwork
- VST3 format, stereo and mono I/O
- GitHub Actions workflow for Windows x64 builds

## Build locally (Windows 10/11)

Requirements: Visual Studio 2022 with Desktop development with C++, CMake 3.22+, Git and Ninja.

Open an x64 Native Tools PowerShell for VS 2022, then run:

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl
cmake --build build --target EQ_AI_Mix_Vocals_Full_VST3 --parallel
```

The JUCE framework is fetched from its official GitHub repository by CMake. The first configure requires internet access.

## GitHub build
Push this folder to a GitHub repository. The complete workflow is:
`.github/workflows/build-windows.yml`

Open **Actions → Build EQ AI Mix Vocals Full VST3 (Windows) → Run workflow**.
The compiled VST3 bundle is uploaded as an artifact.

## Important technical note
“AI Auto Mix” in this starter release is a deterministic, local signal-level heuristic that selects an adjustable processing starting point. It is not a neural network, does not identify vocal gender/style, and does not replace listening or engineering judgement. All audio processing runs locally in the plug-in.

## Install
After a successful GitHub Actions run, download the artifact and copy the `.vst3` bundle to:
`C:\Program Files\Common Files\VST3`
Then rescan plug-ins in your DAW.

## Project layout
- `Source/` — processor and editor source
- `Resources/` — PNG interface artwork
- `.github/workflows/build-windows.yml` — full Windows build workflow
- `CMakeLists.txt` — JUCE/CMake project definition


## Studio branding
The supplied Arm Nongmuang / Mue Perm Studio transparent PNG logo is included at `Resources/Arm_Nongmuang_Studio_Logo.png`. The updated plugin UI artwork also incorporates the logo.


## CI build configuration
GitHub Actions uses the `windows-2022` runner with the x64 MSVC environment and Ninja generator. This avoids relying on CMake's Visual Studio-instance discovery during generator selection.

## JUCE header generation
CMake calls `juce_generate_juce_header(EQ_AI_Mix_Vocals_Full)` so the `JuceHeader.h` included by the processor and editor is generated as part of configuration.

## Fix03 — JUCE API compatibility
The processor overrides JUCE's `getTailLengthSeconds() const` method using the exact JUCE API name. The Windows workflow uses the MSVC x64 + Ninja generator path.
