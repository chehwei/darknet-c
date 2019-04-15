#ifndef _DARKNET_UTILS_H_
#define _DARKNET_UTILS_H_

#include <stdio.h>
#include <json-c/json.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef char * string;
#define json_get_value_default(jobj, type, key, default_value) ({								\
		type val = (type)default_value;												\
		json_object * jvalue = NULL;									\
		json_bool rc = FALSE;											\
		rc = json_object_object_get_ex(jobj, #key, &jvalue);			\
		if(rc && jvalue) val = (type)json_object_get_##type(jvalue);	\
		val;	})

#define json_get_value(jobj, type, key) json_get_value_default(jobj, type, key, 0)


#ifdef __cplusplus
}
#endif
#endif
