# bark.cpp

Fork of ggerganov's bark.cpp - [Original README](https://github.com/ggerganov/bark.cpp/blob/main/README.md)

## What I did

- [x] Fixed to avoid use of ggml-impl.h
- [x] Adapted to latest GGML version
- [x] Added playback (miniaudio)

## What I'am trying to do

- [ ] History prompt support

The original implementation of `bark.cpp` is the bark's 24Khz English model. We expect to support multiple encoders in the future (see [this](https://github.com/PABannier/bark.cpp/issues/36) and [this](https://github.com/PABannier/bark.cpp/issues/6)), as well as music generation model (see [this](https://github.com/PABannier/bark.cpp/issues/62)). This project is for educational purposes.

## Usage

Build and install somewhere the [latest GGML](https://github.com/ggml-org/ggml).

Build the [fixed encoder.cpp](https://github.com/claudiorumonato/encodec.cpp).

Build [bark.cpp](https://github.com/claudiorumonato/bark.cpp) as shown in build.bat.
