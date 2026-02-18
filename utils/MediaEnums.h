#pragma once

// Подумать что делать с enum 
// Как вариант сделать static функцию которая конвертирует enum в uuid и наоборот, uuid берется из кэша который генерируется на старте
// UUIDtoEnum<EnumType>(std::string test)-><EnumType>Value
// EnumtoUUID(T enumValue)->std::string
#include <cstdint>
#include "Macros.h"

extern "C"
{
#include <libavutil/avutil.h>
}

enum ScanMode 
{
    Full,
    MetaDataOnly
};

enum class MetaDataProvider : uint8_t
{
    Local = 0,
    TMDB = 1,
    KinoPoisk = 2,
    OMDB = 3,
    IMDB = 4
};

FLAG_ENUM(MetaDataProvider)

enum class MediaType : int8_t
{
    Unknown = - 1, // Это значение использутеся только в коде, в БД такого не должно быть
    Movie = 0,
    TVShow = 1,
    TVSeason = 2,
    TVEpisode = 3
};

enum class StreamType : int8_t
{
    Unknown = -1, // Не должно быть в базе
    Video = 0,
    Audio = 1,
    Subtitles = 2
};

enum class ImageType : uint8_t
{
    Poster = 0,
    Background = 1,
    Logo = 2,
    Portrait = 3,
    Still = 4
};

enum class CreditType : uint8_t
{
    Actor = 0,
    Production = 1
};

enum class Gender 
{
    NotSet = 0,
    Female = 1,
    Male = 2,
    NonBinary = 3
};

enum class Language : uint8_t
{
    en = 1,
    aa,
    ab,
    af,
    am,
    ar,
    as,
    ay,
    az,
    ba,
    be,
    bg,
    bh,
    bi,
    bn,
    bo,
    br,
    ca,
    co,
    cs,
    cy,
    da,
    de,
    dz,
    el,
    eo,
    es,
    et,
    eu,
    fa,
    fi,
    fj,
    fo,
    fr,
    fy,
    ga,
    gd,
    gl,
    gn,
    gu,
    ha,
    hi,
    hr,
    hu,
    hy,
    ia,
    ie,
    ik,
    in,
    is,
    it,
    iw,
    ja,
    ji,
    jw,
    ka,
    kk,
    kl,
    km,
    kn,
    ko,
    ks,
    ku,
    ky,
    la,
    ln,
    lo,
    lt,
    lv,
    mg,
    mi,
    mk,
    ml,
    mn,
    mo,
    mr,
    ms,
    mt,
    my,
    na,
    ne,
    nl,
    no,
    oc,
    om,
    pa,
    pl,
    ps,
    pt,
    qu,
    rm,
    rn,
    ro,
    ru,
    rw,
    sa,
    sd,
    sg,
    sh,
    si,
    sk,
    sl,
    sm,
    sn,
    so,
    sq,
    sr,
    ss,
    st,
    su,
    sv,
    sw,
    ta,
    te,
    tg,
    th,
    ti,
    tk,
    tl,
    tn,
    to,
    tr,
    ts,
    tt,
    tw,
    uk,
    ur,
    uz,
    vi,
    vo,
    wo,
    xh,
    yo,
    zh,
    zu,
    xx // нет языка или не знаем
};

// Добавить LanguageEnum

inline StreamType AVMediaTypeToStreamType(AVMediaType mediaType)
{
    switch (mediaType) 
    {
        case AVMediaType::AVMEDIA_TYPE_VIDEO:
            return StreamType::Video;
        case AVMediaType::AVMEDIA_TYPE_AUDIO:
            return StreamType::Audio;
        case AVMediaType::AVMEDIA_TYPE_SUBTITLE:
            return StreamType::Subtitles;
        default:
            return StreamType::Unknown;
    }
}
