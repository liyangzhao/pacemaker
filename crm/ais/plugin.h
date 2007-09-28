/*
 * Copyright (C) 2004 Andrew Beekhof <andrew@beekhof.net>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <openais/service/logsys.h>

extern char *local_uname;
extern int local_uname_len;
extern int plugin_log_level;
extern uint32_t local_nodeid;

LOGSYS_DECLARE_SUBSYS("crm", LOG_LEVEL_DEBUG);

static inline const char *level2char(int level)
{
    switch(level) {
	case LOG_CRIT: return "CRIT";
	case LOG_ERR: return "ERR";
	case LOG_WARNING: return "WARN";
	case LOG_NOTICE: return "notice";
	case LOG_INFO: return "info";
    }
    return "debug";
}

#define do_ais_log(level, fmt, args...) do {				\
	if(plugin_log_level < (level)) {				\
	    continue;							\
	} else if((level) > LOG_DEBUG) {				\
	    log_printf(LOG_DEBUG, "debug%d: %s: " fmt,			\
		       level-LOG_INFO, __PRETTY_FUNCTION__ , ##args);	\
	} else {							\
	    log_printf(level, "%s: %s: " fmt, level2char(level),	\
		       __PRETTY_FUNCTION__ , ##args);			\
	}								\
    } while(0)

#define ais_perror(fmt, args...) log_printf(				\
	LOG_ERR, "%s: " fmt ": (%d) %s",				\
	__PRETTY_FUNCTION__ , ##args, errno, strerror(errno))

#define ais_crit(fmt, args...)    do_ais_log(LOG_CRIT,    fmt , ##args)
#define ais_err(fmt, args...)     do_ais_log(LOG_ERR,     fmt , ##args)
#define ais_warn(fmt, args...)    do_ais_log(LOG_WARNING, fmt , ##args)
#define ais_notice(fmt, args...)  do_ais_log(LOG_NOTICE,  fmt , ##args)
#define ais_info(fmt, args...)    do_ais_log(LOG_INFO,    fmt , ##args)
#define ais_debug(fmt, args...)   do_ais_log(LOG_DEBUG,   fmt , ##args)
#define ais_debug_2(fmt, args...) do_ais_log(LOG_DEBUG+1, fmt , ##args)
#define ais_debug_3(fmt, args...) do_ais_log(LOG_DEBUG+2, fmt , ##args)
#define ais_debug_4(fmt, args...) do_ais_log(LOG_DEBUG+3, fmt , ##args)
#define ais_debug_5(fmt, args...) do_ais_log(LOG_DEBUG+4, fmt , ##args)
#define ais_debug_6(fmt, args...) do_ais_log(LOG_DEBUG+5, fmt , ##args)

#define ais_malloc0(malloc_obj, length) do {				\
	malloc_obj = malloc(length);					\
	if(malloc_obj == NULL) {					\
	    abort();							\
	}								\
	memset(malloc_obj, 0, length);					\
    } while(0)

#define ais_free(obj) do {			\
	if(obj) {				\
	    free(obj);				\
	    obj = NULL;				\
	}					\
    } while(0)

#define AIS_ASSERT(expr) if((expr) == FALSE) {				\
	ais_crit("Assertion failure line %d: %s", __LINE__, #expr);	\
	abort();							\
    }

#define AIS_CHECK(expr, failure_action) if((expr) == FALSE) {		\
	ais_err("Non fatal assertion failure line %d: %s", __LINE__, #expr); \
	failure_action;							\
    }

static inline char *ais_strdup(const char *src)
{
	char *dup = NULL;
	if(src == NULL) {
	    return NULL;
	}
	ais_malloc0(dup, strlen(src) + 1);
	return strcpy(dup, src);
}

static inline gboolean ais_str_eq(const char *a, const char *b) 
{
    if(a == NULL || b == NULL) {
	return FALSE;
	
    } else if(a == b) {
	return TRUE;
	
    } else if(strcasecmp(a, b) == 0) {
	return TRUE;
    }
    return FALSE;
}
