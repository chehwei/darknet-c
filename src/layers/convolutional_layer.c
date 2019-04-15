/*
 * convolutional_layer.c
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


typedef struct convolutional_layer_private
{
	int filters_count;
	int kernel_size;
	int stride;
	int padding;
	
	int use_batch_norm;
	
}convolutional_layer_private_t;


static int convolutional_layer_parse_config(darknet_layer_t * layer, json_object * jlayer)
{
	convolutional_layer_private_t * priv = layer->priv;
	assert(priv);
	
	
	
	return 0;
}
	
static int  convolutional_layer_forward(struct darknet_layer * layer)
{
	return 0;
}
static int  convolutional_layer_backward(struct darknet_layer * layer)
{
	return 0;
}

static int  convolutional_layer_update(struct darknet_layer * layer)
{
	return 0;
}

static void convolutional_layer_cleanup(struct darknet_layer * layer)
{
	return;
}

darknet_layer_t * convolutional_layer_new(darknet_layer_t * layer, darknet_size_params_t * size_params)
{
	if(NULL == layer) layer = calloc(1, sizeof(*layer));
	assert(layer);
	convolutional_layer_private_t * priv = calloc(1, sizeof(*priv));
	assert(priv);
	
	layer->net = size_params->net;
	layer->priv = priv;
	
	layer->parse_config = convolutional_layer_parse_config;
	layer->forward = convolutional_layer_forward;
	layer->backward = convolutional_layer_backward;
	layer->update = convolutional_layer_update;
	layer->cleanup = convolutional_layer_cleanup;
	
	
	return layer;
}


