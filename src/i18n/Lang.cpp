#include "Lang.h"

#include "../Config.h"
#include "../Debug.h"
#include "../Features.h"

#if FEATURE_LANG_DE
#include "strings_de.h"
#endif
#if FEATURE_LANG_EN
#include "strings_en.h"
#endif

namespace {

// One flash table per compiled-in language. The X-macro walks the master ID
// list, so index i always belongs to Str(i) - regardless of the order in
// which the translations happen to be written down in the language file.
#if FEATURE_LANG_DE
#define X(id) static const char de_s_##id[] PROGMEM = DE_##id;
LANG_STRING_IDS(X)
#undef X
#define X(id) de_s_##id,
static const char *const DE_TABLE[] PROGMEM = { LANG_STRING_IDS(X) };
#undef X
static_assert(sizeof(DE_TABLE) / sizeof(DE_TABLE[0]) == (size_t)Str::COUNT,
			  "DE: Tabelle passt nicht zur ID-Liste");
#endif

#if FEATURE_LANG_EN
#define X(id) static const char en_s_##id[] PROGMEM = EN_##id;
LANG_STRING_IDS(X)
#undef X
#define X(id) en_s_##id,
static const char *const EN_TABLE[] PROGMEM = { LANG_STRING_IDS(X) };
#undef X
static_assert(sizeof(EN_TABLE) / sizeof(EN_TABLE[0]) == (size_t)Str::COUNT,
			  "EN: Tabelle passt nicht zur ID-Liste");
#endif

struct LangDef {
	const char        *code; // ISO code, uppercase - matches cfg.current_lang
	const char        *name; // native name for the selection field
	const char *const *table;
};

const LangDef LANGS[] = {
#if FEATURE_LANG_DE
	{ "DE", DE_NAME, DE_TABLE },
#endif
#if FEATURE_LANG_EN
	{ "EN", EN_NAME, EN_TABLE },
#endif
};

constexpr int LANG_COUNT = (int)(sizeof(LANGS) / sizeof(LANGS[0]));
static_assert(LANG_COUNT > 0, "Mindestens eine Sprache muss einkompiliert sein (FEATURE_LANG_*)");

// Active language; index into LANGS.
int s_current = 0;

bool sameCode(const char *a, const char *b)
{
	for (int i = 0; i < 2; ++i) {
		if (toupper((unsigned char)a[i]) != toupper((unsigned char)b[i])) return false;
		if (!a[i]) return true;
	}
	return true;
}

} // namespace

namespace I18n {

bool setLang(const char *code)
{
	if (!code || !code[0]) return false;
	for (int i = 0; i < LANG_COUNT; ++i) {
		if (sameCode(code, LANGS[i].code)) {
			s_current = i;
			return true;
		}
	}
	return false;
}

void begin()
{
	if (!setLang(cfg.current_lang)) {
		// Unknown/not compiled in -> first language, and write the config field
		// back so the web UI shows what is actually active.
		s_current = 0;
		strlcpy(cfg.current_lang, LANGS[0].code, sizeof(cfg.current_lang));
		LOG_WARN(F("i18n: Sprache nicht verfügbar, nutze "), String(LANGS[0].code));
	}
	LOG_INFO(F("i18n: Sprache "), String(LANGS[s_current].code));
}

const char *code() { return LANGS[s_current].code; }
const char *name() { return LANGS[s_current].name; }

int         count()        { return LANG_COUNT; }
const char *codeAt(int i)  { return (i >= 0 && i < LANG_COUNT) ? LANGS[i].code : ""; }
const char *nameAt(int i)  { return (i >= 0 && i < LANG_COUNT) ? LANGS[i].name : ""; }

const __FlashStringHelper *str(Str id)
{
	const unsigned i = (unsigned)id;
	if (i >= (unsigned)Str::COUNT) return F("");
	const char *const *table = LANGS[s_current].table;
#if defined(ESP8266)
	// Pointer table lives in flash as well -> read it through pgm_read_ptr.
	const char *p = (const char *)pgm_read_ptr(&table[i]);
#else
	const char *p = table[i];
#endif
	return reinterpret_cast<const __FlashStringHelper *>(p);
}

} // namespace I18n
