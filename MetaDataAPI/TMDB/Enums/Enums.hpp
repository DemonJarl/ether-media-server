#pragma once
#include <algorithm>
#include <cstdint>
#include <string>
#include <cctype>
namespace TMDBAPI
{

#define FLAG_ENUM(_enumName)\
inline _enumName operator|(_enumName lhs, _enumName rhs)\
{\
    return static_cast<_enumName>(static_cast<std::underlying_type<_enumName>::type>(lhs) | static_cast<std::underlying_type<_enumName>::type>(rhs));\
}\
inline _enumName operator&(_enumName lhs, _enumName rhs)\
{\
    return static_cast<_enumName>(static_cast<std::underlying_type<_enumName>::type>(lhs) & static_cast<std::underlying_type<_enumName>::type>(rhs));\
}\

enum class MediaType
{
    Movie,
    TV,
    Person,
    Collection // Для полноты
};

inline MediaType strToMediaType(const std::string& str)
{
	std::string s = str;
	transform(s.begin(), s.end(), s.begin(),
              ::tolower);
	MediaType ret = MediaType::Movie;
	if (s == "tv")
		return MediaType::TV;
	if (s == "person")
		return MediaType::Person;
	if (s == "collection")
		return MediaType::Collection;
	return ret;
}

enum class TVSeriesStatus : uint8_t
{
    ReturningSeries,
    Planned,
    InProduction,
    Ended,
    Cancelled,
    Pilot
};

enum class TVSeriesType : uint8_t
{
    Documentary,
    News,
    Miniseries,
    Reality,
    Scripted,
    TalkShow,
    Video
};

enum class MediaImageType
{
    Backdrops,
    Posters,
    Logos
};

enum class Gender
{
	NotSet = 0,
	Female = 1,
	Male = 2,
	NonBinary = 3
};

enum class Department
{ 
	None = 0,
	Production = 1,
	CostumeMakeUp = 2,
	Lighting = 3,
	Actors = 4,
	Crew = 5,
	Directing = 6,
	Writing = 7,
	Editing = 8,
	Sound = 9,
	VisualEffects = 10,
	Camera = 11,
	Art = 12
};

enum class BackdropSize
{
	w300 = 0,
	w780,
	w1280,
	Original
};

enum class StillSize
{
	w92 = 0,
	w185,
	w300,
	Original
};

enum class ProfileSize
{
	w45 = 0,
	w185,
	h632,
	Original
};


enum class LogoSize
{
	w45,
	w92,
	w154,
	w185,
	w300,
	w500,
	Original
};

enum class PosterSize
{
	w92,
	w154,
	w185,
	w342,
	w500,
	w780,
	Original
};

inline std::string imageSizeToString(ProfileSize size)
{
	switch (size) 
	{
        case ProfileSize::w45:
			return "w45";
        case ProfileSize::w185:
			return "w185";
        case ProfileSize::h632:
			return "h632";
        case ProfileSize::Original:
			return "original";
	}
}

inline std::string imageSizeToString(PosterSize size)
{
	switch (size) 
	{
        case PosterSize::w92:
			return "w98";
        case PosterSize::w154:
			return "w154";
        case PosterSize::w185:
			return "w185";
        case PosterSize::w342:
			return "w342";
        case PosterSize::w500:
			return "w500";
        case PosterSize::w780:
			return "w780";
        case PosterSize::Original:
			return "original";
	}
}

inline std::string imageSizeToString(StillSize size)
{
	switch (size) 
	{
        case StillSize::w92:
			return "w92";
        case StillSize::w185:
			return "w185";
        case StillSize::w300:
			return "w300";
        case StillSize::Original:
			return "original";
	}
}

inline std::string imageSizeToString(LogoSize size)
{
	switch (size) 
	{
        case LogoSize::w45:
			return "w45";
        case LogoSize::w92:
			return "w92";
        case LogoSize::w154:
			return "w154";
        case LogoSize::w185:
			return "w185";
        case LogoSize::w300:
			return "w300";
        case LogoSize::w500:
			return "w500";
        case LogoSize::Original:
			return "original";
	}
}

inline std::string imageSizeToString(BackdropSize size)
{
	switch (size) 
	{
        case BackdropSize::w300:
			return "w300";
        case BackdropSize::w780:
			return "w780";
        case BackdropSize::w1280:
			return "w1280";
        case BackdropSize::Original:
			return "original";
	}
}


inline Department stringToDepartment(const std::string& departament)
{
	std::string s = departament;
	transform(s.begin(), s.end(), s.begin(),
              ::tolower);
	if(s == "production")
	{
		return Department::Production;
	}
	if(s == "actors")
	{
		return Department::Actors;
	}
	if(s == "art")
	{
		return Department::Art;
	}
	if(s == "camera")
	{
		return Department::Camera;
	}
	if(s == "costume & make-up")
	{
		return Department::CostumeMakeUp;
	}
	if(s == "crew")
	{
		return Department::Crew;
	}
	if(s == "directing")
	{
		return Department::Directing;
	}
	if(s == "editing")
	{
		return Department::Editing;
	}
	if(s == "lighting")
	{
		return Department::Lighting;
	}
	if(s == "sound")
	{
		return Department::Sound;
	}
	if(s == "visualeffects")
	{
		return Department::VisualEffects;
	}
	if(s == "sound")
	{
		return Department::Sound;
	}
	if(s == "writing")
	{
		return Department::Writing;
	}
	return Department::None;
}

// АД 
enum class Job
{

};

enum class TVAppendToResponse
{
    None = 1 << 0,
    AccountStates = 1 << 1,
    AggregateCredits = 1 << 2,
    AlternativeTitles = 1 << 3,
    Changes = 1 << 4,
    ContentRatings = 1 << 5,
    Credits = 1 << 6,
    EpisodeGroups = 1 << 7,
    ExternalIds = 1 << 8,
    Images = 1 << 9,
    Keywords = 1 << 10,
    Latest = 1 << 11,
    Lists = 1 << 12,
    Recommendations = 1 << 13,
    Reviews = 1 << 14,
    ScreenedTheatrically = 1 << 15,
    Similar = 1 << 16,
    Translations = 1 << 17,
    Videos = 1 << 18,
    WatchProviders = 1 << 19,
	All = AccountStates | AggregateCredits | AlternativeTitles | Changes | ContentRatings
	| Credits | EpisodeGroups | ExternalIds | Images | Keywords | Latest | Lists | Recommendations
	| Reviews | ScreenedTheatrically | Similar | Translations | Videos | WatchProviders
};
FLAG_ENUM(TVAppendToResponse);
enum class MovieAppendToResponse
{
    None = 1 << 0,
    AccountStates = 1 << 1,
    AggregateCredits = 1 << 2,
    AlternativeTitles = 1 << 3,
    Changes = 1 << 4,
    Credits = 1 << 5,
    ExternalIds = 1 << 6,
    Images = 1 << 7,
    Keywords = 1 << 8,
    Latest = 1 << 9,
    Lists = 1 << 10,
    Recommendations = 1 << 11,
    ReleaseDates = 1 << 12,
    Reviews = 1 << 13,
    ScreenedTheatrically = 1 << 14,
    Similar = 1 << 15,
    Translations = 1 << 16,
    Videos = 1 << 17,
	All = AccountStates | AggregateCredits | AlternativeTitles | Changes
	| Credits | ExternalIds | Images | Keywords | Latest | Lists | Recommendations
	| Reviews | ScreenedTheatrically | Similar | Translations | Videos 
};
FLAG_ENUM(MovieAppendToResponse);
enum class TVSeasonAppendToResponse
{
    None = 1 << 0,
    AccountStates = 1 << 1,
    Changes = 1 << 2,
    Credits = 1 << 3,
    ExternalIds = 1 << 4,
    Images = 1 << 5,
    Translations = 1 << 6,
    Videos = 1 << 7,
    WatchProviders = 1 << 8,
    AggregateCredits = 1 << 9,
	All = AccountStates | AggregateCredits  | Changes 
	| Credits | ExternalIds | Images
	| Translations | Videos | WatchProviders
};
FLAG_ENUM(TVSeasonAppendToResponse);

enum class PeopleAppendToResponse
{
    None = 1 << 0,
    Changes = 1 << 1,
    CombinedCredits = 1 << 2,
    ExternalIds = 1 << 3,
    Images = 1 << 4,
    Latest = 1 << 5,
    MovieCredits = 1 << 6,
    TVCredits = 1 << 7,
    TaggedImages = 1 << 8,
    Translations = 1 << 9,
	All = CombinedCredits | ExternalIds  | Changes 
	| Latest | Images | TVCredits
	| Translations | MovieCredits | TaggedImages 
};
FLAG_ENUM(PeopleAppendToResponse);


enum class EpisodeAppendToResponse
{
    None = 1 << 0,
    AccountStates = 1 << 1,
    Changes = 1 << 2,
    Credits = 1 << 3,
    ExternalIds = 1 << 4,
    Images = 1 << 5,
    Translations = 1 << 6,
    Videos = 1 << 7,
	All = AccountStates | Changes  | Credits 
	| ExternalIds | Images | Translations | Videos  
};
FLAG_ENUM(EpisodeAppendToResponse);

enum class APIError
{
    // https://developer.themoviedb.org/docs/errors
    Success = 1,
    InvalidSevice = 2,
    AuthenticationFailedPermissions = 3,
    InvalidFormat = 4,
    InvalidParameters = 5,
    InvalidID = 6,
    InvalidAPIKey = 7,
    DuplicateEntry = 8,
    ServiceOffline = 9,
    SuspendedAPIKey = 10,
    InternalError = 11,
    ItemUpdatedSuccessfully = 12,
    ItemDeletedSuccessfully = 13,
    AuthenticationFailed = 14,
    Failed = 15,
    DeviceDenied = 16,
    SessionDenied = 17,
    ValidationDenied = 18,
    InvalidAcceptHeader = 19,
    InvalidDateRange = 20,
    EntryNotFound = 21,
    InvalidPage = 22,
    InvalidDate = 23,
    RequestTimeOut = 24,
    RequestCountOverLimit = 25,
    NeedUsernamePassword = 26,
    TooManyAppendToResponse = 27,
    InvalidTimezone = 28,
    ConfirmationRequired = 29,
    InvalidUsernamePassword = 30,
    AccountDisabled = 31,
    EmailNotVerified = 32,
    InvalidRequestToken = 33,
    ResourceNotFound = 34,
    InvalidToken = 35,
    NoWritePermissionForToken = 36,
    SessionNotFound = 37,
    NoPermissionToEdit = 38,
    ResourceIsPrivate = 39,
    NothingToUpdate = 40,
    RequestTokenNotUprovedByUser = 41,
    RequestMethodNotSupported = 42,
    NoConnectionToBackendServer = 44,
    TheIDIsInvalid = 45,
    APIUnderMaintenance = 46,
    InvalidInput = 47,
    // C++ API errors
    JSONParsingError = 48,
    TransportError = 49
};

enum class Languages
{
	Xx,
	Aa,
	Af,
	Ak,
	An,
	As,
	Av,
	Ae,
	Ay,
	Az,
	Ba,
	Bm,
	Bn,
	Bi,
	Bo,
	Bs,
	Br,
	Ca,
	Cs,
	Ch,
	Ce,
	Cu,
	Cv,
	Kw,
	Co,
	Cr,
	Cy,
	Da,
	De,
	Dv,
	Dz,
	En,
	Eo,
	Et,
	Eu,
	Fo,
	Fj,
	Fi,
	Fr,
	Fy,
	Ff,
	Gd,
	Ga,
	Gl,
	Gv,
	Gn,
	Gu,
	Ht,
	Ha,
	Sh,
	Hz,
	Ho,
	Hr,
	Hu,
	Ig,
	Io,
	Ii,
	Iu,
	Ie,
	Ia,
	Id,
	Ik,
	Is,
	It,
	Jv,
	Ja,
	Kl,
	Kn,
	Ks,
	Ka,
	Kr,
	Kk,
	Km,
	Ki,
	Rw,
	Ky,
	Kv,
	Kg,
	Ko,
	Kj,
	Ku,
	Lo,
	La,
	Lv,
	Li,
	Ln,
	Lt,
	Lb,
	Lu,
	Lg,
	Mh,
	Ml,
	Mr,
	Mg,
	Mt,
	Mo,
	Mn,
	Mi,
	Ms,
	My,
	Na,
	Nv,
	Nr,
	Nd,
	Ng,
	Ne,
	Nl,
	Nn,
	Nb,
	No,
	Ny,
	Oc,
	Oj,
	Or,
	Om,
	Os,
	Pa,
	Pi,
	Pl,
	Pt,
	Qu,
	Rm,
	Ro,
	Rn,
	Ru,
	Sg,
	Sa,
	Si,
	Sk,
	Sl,
	Se,
	Sm,
	Sn,
	Sd,
	So,
	St,
	Es,
	Sq,
	Sc,
	Sr,
	Ss,
	Su,
	Sw,
	Sv,
	Ty,
	Ta,
	Tt,
	Te,
	Tg,
	Tl,
	Th,
	Ti,
	To,
	Tn,
	Ts,
	Tk,
	Tr,
	Tw,
	Ug,
	Uk,
	Ur,
	Uz,
	Ve,
	Vi,
	Vo,
	Wa,
	Wo,
	Xh,
	Yi,
	Za,
	Zu,
	Ab,
	Zh,
	Ps,
	Am,
	Ar,
	Be,
	Bg,
	Cn,
	Mk,
	Ee,
	El,
	Fa,
	He,
	Hi,
	Hy,
	Yo,
};
// Ад
inline std::string languageToStr(Languages language)
{
    switch (language) 
    {
        case Languages::Xx: return "xx";
        case Languages::Aa: return "aa";
        case Languages::Af: return "af";
        case Languages::Ak: return "ak";
        case Languages::An: return "an";
        case Languages::As: return "as";
        case Languages::Av: return "av";
        case Languages::Ae: return "ae";
        case Languages::Ay: return "ay";
        case Languages::Az: return "az";
        case Languages::Ba: return "ba";
        case Languages::Bm: return "bm";
        case Languages::Bn: return "bn";
        case Languages::Bi: return "bi";
        case Languages::Bo: return "bo";
        case Languages::Bs: return "bs";
        case Languages::Br: return "br";
        case Languages::Ca: return "ca";
        case Languages::Cs: return "cs";
        case Languages::Ch: return "ch";
        case Languages::Ce: return "ce";
        case Languages::Cu: return "cu";
        case Languages::Cv: return "cv";
        case Languages::Kw: return "kw";
        case Languages::Co: return "co";
        case Languages::Cr: return "cr";
        case Languages::Cy: return "cy";
        case Languages::Da: return "da";
        case Languages::De: return "de";
        case Languages::Dv: return "dv";
        case Languages::Dz: return "dz";
        case Languages::En: return "en";
        case Languages::Eo: return "eo";
        case Languages::Et: return "et";
        case Languages::Eu: return "eu";
        case Languages::Fo: return "fo";
        case Languages::Fj: return "fj";
        case Languages::Fi: return "fi";
        case Languages::Fr: return "fr";
        case Languages::Fy: return "fy";
        case Languages::Ff: return "ff";
        case Languages::Gd: return "gd";
        case Languages::Ga: return "ga";
        case Languages::Gl: return "gl";
        case Languages::Gv: return "gv";
        case Languages::Gn: return "gn";
        case Languages::Gu: return "gu";
        case Languages::Ht: return "ht";
        case Languages::Ha: return "ha";
        case Languages::Sh: return "sh";
        case Languages::Hz: return "hz";
        case Languages::Ho: return "ho";
        case Languages::Hr: return "hr";
        case Languages::Hu: return "hu";
        case Languages::Ig: return "ig";
        case Languages::Io: return "io";
        case Languages::Ii: return "ii";
        case Languages::Iu: return "iu";
        case Languages::Ie: return "ie";
        case Languages::Ia: return "ia";
        case Languages::Id: return "id";
        case Languages::Ik: return "ik";
        case Languages::Is: return "is";
        case Languages::It: return "it";
        case Languages::Jv: return "jv";
        case Languages::Ja: return "ja";
        case Languages::Kl: return "kl";
        case Languages::Kn: return "kn";
        case Languages::Ks: return "ks";
        case Languages::Ka: return "ka";
        case Languages::Kr: return "kr";
        case Languages::Kk: return "kk";
        case Languages::Km: return "km";
        case Languages::Ki: return "ki";
        case Languages::Rw: return "rw";
        case Languages::Ky: return "ky";
        case Languages::Kv: return "kv";
        case Languages::Kg: return "kg";
        case Languages::Ko: return "ko";
        case Languages::Kj: return "kj";
        case Languages::Ku: return "ku";
        case Languages::Lo: return "lo";
        case Languages::La: return "la";
        case Languages::Lv: return "lv";
        case Languages::Li: return "li";
        case Languages::Ln: return "ln";
        case Languages::Lt: return "lt";
        case Languages::Lb: return "lb";
        case Languages::Lu: return "lu";
        case Languages::Lg: return "lg";
        case Languages::Mh: return "mh";
        case Languages::Ml: return "ml";
        case Languages::Mr: return "mr";
        case Languages::Mg: return "mg";
        case Languages::Mt: return "mt";
        case Languages::Mo: return "mo";
        case Languages::Mn: return "mn";
        case Languages::Mi: return "mi";
        case Languages::Ms: return "ms";
        case Languages::My: return "my";
        case Languages::Na: return "na";
        case Languages::Nv: return "nv";
        case Languages::Nr: return "nr";
        case Languages::Nd: return "nd";
        case Languages::Ng: return "ng";
        case Languages::Ne: return "ne";
        case Languages::Nl: return "nl";
        case Languages::Nn: return "nn";
        case Languages::Nb: return "nb";
        case Languages::No: return "no";
        case Languages::Ny: return "ny";
        case Languages::Oc: return "oc";
        case Languages::Oj: return "oj";
        case Languages::Or: return "or";
        case Languages::Om: return "om";
        case Languages::Os: return "os";
        case Languages::Pa: return "pa";
        case Languages::Pi: return "pi";
        case Languages::Pl: return "pl";
        case Languages::Pt: return "pt";
        case Languages::Qu: return "qu";
        case Languages::Rm: return "rm";
        case Languages::Ro: return "ro";
        case Languages::Rn: return "rn";
        case Languages::Ru: return "ru";
        case Languages::Sg: return "sg";
        case Languages::Sa: return "sa";
        case Languages::Si: return "si";
        case Languages::Sk: return "sk";
        case Languages::Sl: return "sl";
        case Languages::Se: return "se";
        case Languages::Sm: return "sm";
        case Languages::Sn: return "sn";
        case Languages::Sd: return "sd";
        case Languages::So: return "so";
        case Languages::St: return "st";
        case Languages::Es: return "es";
        case Languages::Sq: return "sq";
        case Languages::Sc: return "sc";
        case Languages::Sr: return "sr";
        case Languages::Ss: return "ss";
        case Languages::Su: return "su";
        case Languages::Sw: return "sw";
        case Languages::Sv: return "sv";
        case Languages::Ty: return "ty";
        case Languages::Ta: return "ta";
        case Languages::Tt: return "tt";
        case Languages::Te: return "te";
        case Languages::Tg: return "tg";
        case Languages::Tl: return "tl";
        case Languages::Th: return "th";
        case Languages::Ti: return "ti";
        case Languages::To: return "to";
        case Languages::Tn: return "tn";
        case Languages::Ts: return "ts";
        case Languages::Tk: return "tk";
        case Languages::Tr: return "tr";
        case Languages::Tw: return "tw";
        case Languages::Ug: return "ug";
        case Languages::Uk: return "uk";
        case Languages::Ur: return "ur";
        case Languages::Uz: return "uz";
        case Languages::Ve: return "ve";
        case Languages::Vi: return "vi";
        case Languages::Vo: return "vo";
        case Languages::Wa: return "wa";
        case Languages::Wo: return "wo";
        case Languages::Xh: return "xh";
        case Languages::Yi: return "yi";
        case Languages::Za: return "za";
        case Languages::Zu: return "zu";
        case Languages::Ab: return "ab";
        case Languages::Zh: return "zh";
        case Languages::Ps: return "ps";
        case Languages::Am: return "am";
        case Languages::Ar: return "ar";
        case Languages::Be: return "be";
        case Languages::Bg: return "bg";
        case Languages::Cn: return "cn";
        case Languages::Mk: return "mk";
        case Languages::Ee: return "ee";
        case Languages::El: return "el";
        case Languages::Fa: return "fa";
        case Languages::He: return "he";
        case Languages::Hi: return "hi";
        case Languages::Hy: return "hy";
        case Languages::Yo: return "yo";
    }
}
inline Languages strToLanguage(const std::string& language)
{
	if (language == "en")
		return Languages::En;
	if (language == "ru")
		return Languages::Ru;
	if (language == "ja")
		return Languages::Ja;
	if (language == "kr")
		return Languages::Kr;
	if (language == "ko")
		return Languages::Ko;
	return Languages::Xx;
}

enum class Countries
{
	AD,
	AE,
	AF,
	AG,
	AI,
	AL,
	AM,
	AN,
	AO,
	AQ,
	AR,
	AS,
	AT,
	AU,
	AW,
	AZ,
	BA,
	BB,
	BD,
	BE,
	BF,
	BG,
	BH,
	BI,
	BJ,
	BM,
	BN,
	BO,
	BR,
	BS,
	BT,
	BU,
	BV,
	BW,
	BY,
	BZ,
	CA,
	CC,
	CD,
	CF,
	CG,
	CH,
	CI,
	CK,
	CL,
	CM,
	CN,
	CO,
	CR,
	CS,
	CU,
	CV,
	CX,
	CY,
	CZ,
	DE,
	DJ,
	DK,
	DM,
	DO,
	DZ,
	EC,
	EE,
	EG,
	EH,
	ER,
	ES,
	ET,
	FI,
	FJ,
	FK,
	FM,
	FO,
	FR,
	GA,
	GB,
	GD,
	GE,
	GF,
	GH,
	GI,
	GL,
	GM,
	GN,
	GP,
	GQ,
	GR,
	GS,
	GT,
	GU,
	GW,
	GY,
	HK,
	HM,
	HN,
	HR,
	HT,
	HU,
	ID,
	IE,
	IL,
	IN,
	IO,
	IQ,
	IR,
	IS,
	IT,
	JM,
	JO,
	JP,
	KE,
	KG,
	KH,
	KI,
	KM,
	KN,
	KP,
	KR,
	KW,
	KY,
	KZ,
	LA,
	LB,
	LC,
	LI,
	LK,
	LR,
	LS,
	LT,
	LU,
	LV,
	LY,
	MA,
	MC,
	MD,
	ME,
	MG,
	MH,
	MK,
	ML,
	MM,
	MN,
	MO,
	MP,
	MQ,
	MR,
	MS,
	MT,
	MU,
	MV,
	MW,
	MX,
	MY,
	MZ,
	NA,
	NC,
	NE,
	NF,
	NG,
	NI,
	NL,
	NO,
	NP,
	NR,
	NU,
	NZ,
	OM,
	PA,
	PE,
	PF,
	PG,
	PH,
	PK,
	PL,
	PM,
	PN,
	PR,
	PS,
	PT,
	PW,
	PY,
	QA,
	RE,
	RO,
	RS,
	RU,
	RW,
	SA,
	SB,
	SC,
	SD,
	SE,
	SG,
	SH,
	SI,
	SJ,
	SK,
	SL,
	SM,
	SN,
	SO,
	SR,
	SS,
	ST,
	SU,
	SV,
	SY,
	SZ,
	TC,
	TD,
	TF,
	TG,
	TH,
	TJ,
	TK,
	TL,
	TM,
	TN,
	TO,
	TP,
	TR,
	TT,
	TV,
	TW,
	TZ,
	UA,
	UG,
	UM,
	US,
	UY,
	UZ,
	VA,
	VC,
	VE,
	VG,
	VI,
	VN,
	VU,
	WF,
	WS,
	XC,
	XG,
	XI,
	XK,
	YE,
	YT,
	YU,
	ZA,
	ZM,
	ZR,
	ZW,
};

#undef FLAG_ENUM
}