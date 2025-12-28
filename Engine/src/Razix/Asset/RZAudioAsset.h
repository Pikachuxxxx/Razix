#ifndef _RZ_AUDIO_ASSET_H_
#define _RZ_AUDIO_ASSET_H_

#include "Razix/AssetSystem/RZAssetBase.h"

#include "Razix/Core/Reflection/RZReflection.h"

namespace Razix {

    typedef enum RZAudioType
    {
        RZ_AUDIO_TYPE_SOUND_EFFECT = 0,
        RZ_AUDIO_TYPE_MUSIC        = 1,
        RZ_AUDIO_TYPE_VOICE        = 2
    } RZAudioType;

    struct RAZIX_ALIGN_TO(RAZIX_16B_ALIGN) RZAudioAsset
    {
        u32         AudioDataHandle;
        u32         SampleRate;
        u32         ChannelCount;
        u32         SampleCount;
        f32         Duration;
        RZAudioType Type;
        bool        bIsLooping;
        u8          _pad0[3];
    };

    RAZIX_REFLECT_TYPE_START(RZAudioAsset)
    RAZIX_REFLECT_MEMBER(AudioDataHandle)
    RAZIX_REFLECT_MEMBER(SampleRate)
    RAZIX_REFLECT_MEMBER(ChannelCount)
    RAZIX_REFLECT_MEMBER(SampleCount)
    RAZIX_REFLECT_MEMBER(Duration)
    RAZIX_REFLECT_MEMBER(Type)
    RAZIX_REFLECT_MEMBER(bIsLooping)
    RAZIX_REFLECT_TYPE_END(RZAudioAsset)

};    // namespace Razix

#endif    // _RZ_AUDIO_ASSET_H_
