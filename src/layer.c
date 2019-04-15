/*
 * layer.c
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
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "libdarknet-c.h"
#include "utils.h"

#include <json-c/json.h>


darknet_layer_t * convolutional_layer_new(darknet_layer_t * layer, darknet_size_params_t * size_params);


darknet_layer_t * darknet_layer_new(enum darknet_layer_type type, darknet_size_params_t * size_param)
{
	darknet_layer_t * layer = NULL;
	switch(type)
	{
	case darknet_layer_type_convolutional:
		layer = convolutional_layer_new(NULL, size_param);
		break;
	default:
		fprintf(stderr, "Not Impl. \n");
		exit(1);
	}
	return layer;
}

darknet_layer_t * darknet_layer_new_from_config(json_object * jlayer, darknet_size_params_t * size_param)
{
	const char * type_str = json_get_value(jlayer, string, layer_type);
	enum darknet_layer_type type = darknet_layer_type_from_string(type_str);
	
	darknet_layer_t * layer = darknet_layer_new(type, size_param);
	assert(layer);
	
	layer->parse_config(layer, jlayer);
	layer->resize(layer, size_param);
	
	return layer;
}




