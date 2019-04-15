/*
 * cfg_to_json.c
 * 
 * Copyright 2019 chehw <htc.chehw@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <unistd.h>
#include <json-c/json.h>

static int ini_to_json(const char * cfg_file, const char * json_file);

int main(int argc, char **argv)
{
	const char * cfg_file = "conf/yolov3.cfg";
	const char * dst_file = "conf/yolov3.json";
	
	if(argc > 1) cfg_file = argv[1];
	if(argc > 2) cfg_file = argv[2];
	
	ini_to_json(cfg_file, dst_file);
	 
	return 0;
}

#define is_white_char(c) ((c) == ' ' || (c) == '\t' || (c) == '\r' || (c) == '\n')

static inline char * trim_left(char * p_begin, char * p_end)
{
	assert(p_begin);
	if(NULL == p_end) p_end = p_begin + strlen(p_begin);
	
	while(p_begin < p_end)
	{
		if(is_white_char(*p_begin)) ++p_begin;
		break;
	}
	return p_begin;
}
static inline char * trim_right(char * p_begin, char * p_end)
{
	assert(p_begin);
	if(NULL == p_end) p_end = p_begin + strlen(p_begin);
	
	while(p_begin < p_end)
	{
		char c = *--p_end;
		if(!is_white_char(c)) {
			p_end[1] = '\0';
			break;
		}
	}
	return p_begin;
}

static inline char * trim(char * p_begin, char **pp_end)
{
	char * p_end = NULL;
	if(pp_end) p_end = *pp_end;
	if(NULL == p_end) p_end = p_begin + strlen(p_begin);
	
	p_begin = trim_left(p_begin, p_end);

	
	trim_right(p_begin, p_end);
	if(pp_end) *pp_end = p_end;
	
	return p_begin;
}


enum value_type
{
	value_type_unknown,
	value_type_int,
	value_type_double,
	value_type_string,
	value_type_array,
	value_types_count
};

static inline enum value_type get_type(const char * restrict  val)
{
	if(strchr(val, ',')) return value_type_array;
	
	char c = 0;
	enum value_type type = value_type_unknown;
	
	c = *val++;
	if(c == '-') 
	{
		type = value_type_int;
		c = *val++;
	}
	
	do
	{
		if(c < 0x30 || c > 0x39)
		{
			if(c == '.') 
			{
				assert(type != value_type_double);
				type = value_type_double;
			}else
			{
				if(c == '-')
				{
					assert(type != value_type_int || type != value_type_double);
				}
				type = value_type_string;
			}
		}else
		{
			if(type != value_type_string && type != value_type_double)
			{
				type = value_type_int;
			}
		}
	}while((c = *val++));
	return type;
}

static int ini_to_json(const char * cfg_file, const char * json_file)
{
	FILE * fp = fopen(cfg_file, "r");
	assert(fp);
	
	char buffer[1024] = "";
	char * line = NULL;
	
	int line_no = 0;
	
	
	
	json_object * jconfig = json_object_new_object();
	json_object * jnet = json_object_new_object();
	json_object * jlayers = json_object_new_array();
	
	json_object_object_add(jconfig, "net", jnet);
	json_object_object_add(jconfig, "layers", jlayers);
	
	int layer_index = 0;
	json_object * jlayer = NULL;
	
	while((line = fgets(buffer, sizeof(buffer) - 1, fp)))
	{
		
		++line_no;
		int cb = strlen(line);
		char * p_end = line + cb;
		char * p_comment = strchr(line, '#');
		if(p_comment) *p_comment = '\0';
		line = trim(line, &p_end);
		
		if(line == p_end || !line[0]) continue;
		
		printf("line %.5d: [%s]\n", line_no, line); 
		if(line[0] == '[')	// section
		{
			char section[100] = "";
			
			++line;
			int i = 0;
			for(; line[i] && line[i] != ']'; ++i)
			{
				assert(i < 100);
				section[i] = line[i];
			}
			section[i] = '\0';
			assert(i > 0);
			
			if(strcasecmp(section, "net") == 0)
			{
				jlayer = jnet;
			}else
			{
				jlayer = json_object_new_object();
				json_object_array_add(jlayers, jlayer);
				json_object_object_add(jlayer, "layer_type", json_object_new_string(section));
				json_object_object_add(jlayer, "layer_index", json_object_new_int(++layer_index));
			}
			continue;
		}
		
		
		
		char * key = line;
		char * value = strchr(line, '=');
		assert(value);
		trim_right(key, value);
		*value++ = '\0';
		
		
		json_object * jparam = NULL;
	
		
		value = trim_left(value, p_end);
		enum value_type type = get_type(value);
		if(type == value_type_array)
		{
			char * tok = NULL;
			value = strtok_r(value, ", ", &tok);
			assert(value);
			
			jparam = json_object_new_array();
			while(value)
			{
				value = trim(value, NULL);
				type = get_type(value);
				
				switch(type)
				{
				case value_type_int: 
					json_object_array_add(jparam, json_object_new_int(atoi(value)));
					break;
				case value_type_double:
					json_object_array_add(jparam, json_object_new_double(atof(value)));
					break;
				case value_type_string:
					json_object_array_add(jparam, json_object_new_string(value));
					break;
				default:
					fprintf(stderr, "invalid value type: %d\n", (int)type);
					exit(1);
				}
				value = strtok_r(NULL, ", ", &tok);
			}
		}else
		{
			assert(type != value_type_unknown && type < value_types_count);
			switch(type)
			{
			case value_type_int: 
				jparam = json_object_new_int(atoi(value));
				break;
			case value_type_double:
				jparam = json_object_new_double(atof(value));
				break;
			case value_type_string:
				jparam = json_object_new_string(value);
				break;
			default:
				fprintf(stderr, "invalid value type: %d\n", (int)type);
				exit(1);
			}
		}
		
		assert(jparam);
		json_object_object_add(jlayer, key, jparam);
	}
	
	
	fclose(fp);
	
	json_object_to_file_ext(json_file, jconfig, JSON_C_TO_STRING_PRETTY);
	
	return 0;
}

