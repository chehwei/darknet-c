/*
 * network.c
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


static const char * s_layer_type_string[darknet_layer_types_count] = {
	[darknet_layer_type_unknown] = "unknown",
	[darknet_layer_type_convolutional] = "convolutional",
	[darknet_layer_type_shortcut] = "shortcut",
	[darknet_layer_type_upsample] = "upsample",
	[darknet_layer_type_route] = "route",
	[darknet_layer_type_yolo] = "yolo",
};

enum darknet_layer_type darknet_layer_type_from_string(const char * type_str)
{
	int i = 0;
	for(; i < darknet_layer_types_count; ++i)
	{
		if(strcasecmp(s_layer_type_string[i], type_str) == 0) break;
	}
	if(i < darknet_layer_types_count) return (enum darknet_layer_type)i;
	return darknet_layer_type_unknown;
}

typedef struct darknet_network_private
{
	darknet_size_params_t size_param;
	
	double momentum;
	double decay;
	double angle;
	double saturation;
	double exposure;
	double hue;
	double learning_rate;
	int burn_in;
	int max_batches;
	
	const char * policy;
	size_t steps_count;
	int * steps;
	
	size_t scales_count;
	double * scales;
	
}darknet_network_private_t;



static int darknet_network_load_config(struct darknet_network * net, const char * conf_file)
{
	darknet_network_private_t * priv = net->priv;
	assert(priv);
	
	json_object * jconfig = json_object_from_file(conf_file);
	assert(jconfig);
	
	json_bool rc = FALSE;
	json_object * jnet = NULL;
	
	rc = json_object_object_get_ex(jconfig, "net", &jnet);
	assert(rc && jnet);
	
	net->batch = json_get_value_default(jnet, int, batch, 1);
	net->subdivisions = json_get_value_default(jnet, int, subdivisons, 1);
	net->groups =json_get_value_default(jnet, int, groups, 1);
	
	net->input_size.n = 1;
	net->input_size.c = json_get_value(jnet, int, channels);
	net->input_size.h = json_get_value(jnet, int, height);
	net->input_size.w = json_get_value(jnet, int, width);
	
	priv->momentum = json_get_value(jnet, double, momentum);
	priv->decay = json_get_value(jnet, double, decay);
	priv->angle = json_get_value(jnet, double, angle);
	priv->saturation = json_get_value(jnet, double, saturation);
	priv->exposure = json_get_value(jnet, double, exposure);
	priv->hue = json_get_value(jnet, double, hue);
	priv->learning_rate = json_get_value(jnet, double, learning_rate);
	priv->burn_in = json_get_value(jnet, int, burn_in);
	priv->max_batches = json_get_value(jnet, int, max_batches);
	priv->policy = json_get_value(jnet, string, policy);
	
	json_object * jsteps = NULL;
	rc = json_object_object_get_ex(jnet, "steps", &jsteps);
	if(rc && jsteps)
	{
		json_type j_type = json_object_get_type(jsteps);
		if(j_type == json_type_array)
		{
			int count = json_object_array_length(jsteps);
			assert(count > 0);
			
			priv->steps_count = count;
			priv->steps = calloc(count, sizeof(int));
			assert(priv->steps);
			
			for(int i = 0; i < count; ++i)
			{
				json_object * jstep = json_object_array_get_idx(jsteps, i);
				priv->steps[i] = json_object_get_int(jstep);
			}
		}else
		{
			priv->steps_count = 1;
			priv->steps = calloc(1, sizeof(int));
			priv->steps[0] = json_object_get_int(jsteps);
		}
	}
	
	json_object * jscales = NULL;
	rc = json_object_object_get_ex(jnet, "scales", &jscales);
	if(rc && jscales)
	{
		json_type j_type = json_object_get_type(jscales);
		if(j_type == json_type_array)
		{
			int count = json_object_array_length(jscales);
			assert(count > 0);
			
			priv->scales_count = count;
			priv->scales = calloc(count, sizeof(double));
			assert(priv->scales);
			
			for(int i = 0; i < count; ++i)
			{
				json_object * jscale = json_object_array_get_idx(jscales, i);
				priv->scales[i] = json_object_get_int(jscale);
			}
		}else
		{
			priv->scales_count = 1;
			priv->scales = calloc(1, sizeof(double));
			priv->scales[0] = json_object_get_double(jscales);
		}
	}
	
	return 0;
}

static int 	darknet_network_forward(struct darknet_network * net, const float * X /* input */)
{
	return 0;
}

static int 	darknet_network_backward(struct darknet_network * net)
{
	return 0;
}

static int 	darknet_network_update(struct darknet_network * net)
{
	return 0;
}

static void darknet_network_reset(struct darknet_network * net)
{
	return;
}

darknet_network_t * darknet_network_init(darknet_network_t * net, const char * conf_file, void * user_data)
{
	if(NULL == net) net = calloc(1, sizeof(*net));
	assert(net);
	
	darknet_network_private_t * priv = calloc(1, sizeof(*priv));
	assert(priv);
	
	net->priv = priv;
	
	net->load_config = darknet_network_load_config;
	net->forward = darknet_network_forward;
	net->backward = darknet_network_backward;
	net->update = darknet_network_update;
	net->reset = darknet_network_reset;
	
	net->user_data = user_data;
	if(conf_file) net->load_config(net, conf_file);
	
	return net;
}

void darknet_network_cleanup(darknet_network_t * net)
{
	if(net)
	{
		for(int i = 0; i < net->layers_count; ++i)
		{
			darknet_layer_t	* layer = net->layers[i];
			if(layer)
			{
				layer->cleanup(layer);
				net->layers[i] = NULL;
			}
		}
		
		if(net->priv)
		{
			free(net->priv);
		}
		free(net);
	}
	return;
}

void darknet_network_dump(darknet_network_t * net)
{
	darknet_network_private_t * priv = net->priv;
	
#define print_field_string(p, _f_) printf("\t [%16.16s]: [%s]\n", #_f_, p->_f_)   
#define print_field_int(p, _f_)	printf("\t [%16.16s]: [%d]\n", #_f_, p->_f_)
#define print_field_doulbe(p, _f_)	printf("\t [%16.16s]: [%.6f]\n", #_f_, p->_f_)
	
	printf("==== darknet_network ====\n");
	print_field_int(net, batch);
	print_field_int(net, subdivisions);
	print_field_int(net, input_size.w);
	print_field_int(net, input_size.h);
	print_field_int(net, input_size.c);
	
	print_field_doulbe(priv, momentum);
	print_field_doulbe(priv, decay);
	print_field_doulbe(priv, angle);
	print_field_doulbe(priv, saturation);
	print_field_doulbe(priv, exposure);
	print_field_doulbe(priv, hue);
	print_field_doulbe(priv, learning_rate);
	print_field_int(priv, burn_in);
	print_field_int(priv, max_batches);
	print_field_string(priv, policy);
	
#undef net_print_field_string
#undef net_print_field_int
#undef net_print_field_float
	
	return;
}

