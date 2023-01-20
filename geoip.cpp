#include <cstdlib>
#include <syslog.h>
#include <HalonMTA.h>
#include <maxminddb.h>

using namespace std;

MMDB_s mmdb;
int mmdb_status = MMDB_FILE_OPEN_ERROR;

HALON_EXPORT
int Halon_version()
{
	return HALONMTA_PLUGIN_VERSION;
}

HALON_EXPORT
bool Halon_init(HalonInitContext* hic)
{
	HalonConfig* cfg;
	HalonMTA_init_getinfo(hic, HALONMTA_INIT_CONFIG, nullptr, 0, &cfg, nullptr);

	const char* path = HalonMTA_config_string_get(HalonMTA_config_object_get(cfg, "path"), nullptr);
	if (!path)
	{
		syslog(LOG_CRIT, "maxmind-geoip: no database path specified");
		return false;
	}

	mmdb_status = MMDB_open(path, MMDB_MODE_MMAP, &mmdb);
	if (mmdb_status != MMDB_SUCCESS)
	{
		syslog(LOG_CRIT, "maxmind-geoip: %s", MMDB_strerror(mmdb_status));
		return false;
	}

	return true;
}

HALON_EXPORT
void Halon_cleanup()
{
	if (mmdb_status == MMDB_SUCCESS)
		MMDB_close(&mmdb);
}

void geoip(HalonHSLContext* hhc, HalonHSLArguments* args, HalonHSLValue* ret)
{
	HalonHSLValue* x;
	char* ip = nullptr;

	x = HalonMTA_hsl_argument_get(args, 0);
	if (x && HalonMTA_hsl_value_type(x) == HALONMTA_HSL_TYPE_STRING)
		HalonMTA_hsl_value_get(x, HALONMTA_HSL_TYPE_STRING, &ip, nullptr);
	else
		return;

	HalonHSLValue *k, *v;
	int gai_error, mmdb_error;
	MMDB_lookup_result_s lookup = MMDB_lookup_string(&mmdb, ip, &gai_error, &mmdb_error);

	if (gai_error != 0)
	{
		HalonMTA_hsl_value_array_add(ret, &k, &v);
		HalonMTA_hsl_value_set(k, HALONMTA_HSL_TYPE_STRING, "error", 0);
		HalonMTA_hsl_value_set(v, HALONMTA_HSL_TYPE_STRING, "getaddrinfo error", 0);
		return;
	}

	if (mmdb_error != MMDB_SUCCESS)
	{
		HalonMTA_hsl_value_array_add(ret, &k, &v);
		HalonMTA_hsl_value_set(k, HALONMTA_HSL_TYPE_STRING, "error", 0);
		HalonMTA_hsl_value_set(v, HALONMTA_HSL_TYPE_STRING, MMDB_strerror(mmdb_error), 0);
		return;
	}

	HalonMTA_hsl_value_set(ret, HALONMTA_HSL_TYPE_ARRAY, nullptr, 0);
	if (!lookup.found_entry)
		return;

	MMDB_entry_data_s entry_data;
	int mmdb_value_status;

	mmdb_value_status = MMDB_get_value(&lookup.entry, &entry_data, "country", "names", "en", NULL);
	if (mmdb_value_status == MMDB_SUCCESS && entry_data.has_data)
	{
		HalonMTA_hsl_value_array_add(ret, &k, &v);
		HalonMTA_hsl_value_set(k, HALONMTA_HSL_TYPE_STRING, "country", 0);
		HalonMTA_hsl_value_set(v, HALONMTA_HSL_TYPE_STRING, entry_data.utf8_string, entry_data.data_size);
	}

	mmdb_value_status = MMDB_get_value(&lookup.entry, &entry_data, "country", "iso_code", NULL);
	if (mmdb_value_status == MMDB_SUCCESS && entry_data.has_data)
	{
		HalonMTA_hsl_value_array_add(ret, &k, &v);
		HalonMTA_hsl_value_set(k, HALONMTA_HSL_TYPE_STRING, "iso_code", 0);
		HalonMTA_hsl_value_set(v, HALONMTA_HSL_TYPE_STRING, entry_data.utf8_string, entry_data.data_size);
	}
}

HALON_EXPORT
bool Halon_hsl_register(HalonHSLRegisterContext* hhrc)
{
	HalonMTA_hsl_register_function(hhrc, "geoip", &geoip);
	HalonMTA_hsl_module_register_function(hhrc, "geoip", &geoip);
	return true;
}
