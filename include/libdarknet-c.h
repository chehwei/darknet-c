#ifndef _LIB_DARKNET_C_H_
#define _LIB_DARKNET_C_H_

#include <stdio.h>
#include <json-c/json.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct darknet_size4i
{
	int n, c, h, w;
}darknet_size4i;

typedef struct darknet_network darknet_network_t;
typedef struct darknet_size_params darknet_size_params_t;
typedef struct darknet_layer darknet_layer_t;


struct darknet_size_params
{
	darknet_network_t * net;
	int batch;
	int index;
	darknet_size4i size;
};


enum darknet_layer_type
{
	darknet_layer_type_unknown,
	darknet_layer_type_convolutional,
	darknet_layer_type_shortcut,
	darknet_layer_type_upsample,
	darknet_layer_type_route,
	darknet_layer_type_yolo,
	darknet_layer_types_count,
};
enum darknet_layer_type darknet_layer_type_from_string(const char * type_str);

enum darknet_activation
{
	darknet_activation_none,
	darknet_activation_sigmoid,
	darknet_activation_leaky,
	darknet_activation_linear,
};


struct darknet_layer
{
	enum darknet_layer_type type;
	enum darknet_activation activation;
	
	void * priv;			/* xxxxxx_layer_private_t */
	darknet_network_t * net;
	
	darknet_size4i input_size;
	darknet_size4i output_size;
	
	const float * input;	/* == prev_layer->output */
	float * output;
	
	//const float * weights;
	size_t weights_offset;
	size_t weights_count;
	/* const */float * weights;
	
	size_t bias_offset;
	size_t bias_count;
	/* const */float * bias;
	
	size_t delta_offset;
	size_t delta_count;
	/* const */float * delta;
	
	int (* parse_config)(struct darknet_layer * layer, json_object * jlayer);
	int (* resize)(struct darknet_layer * layer, darknet_size_params_t * size_param);
	
	int (* forward)(struct darknet_layer * layer);
	int (* backward)(struct darknet_layer * layer);
	int (* update)(struct darknet_layer * layer);
	
	void (* cleanup)(struct darknet_layer * layer);
};

darknet_layer_t * darknet_layer_new(enum darknet_layer_type type, darknet_size_params_t * size_param);
darknet_layer_t * darknet_layer_new_from_config(json_object * jlayer, darknet_size_params_t * size_param);



struct darknet_network
{
	void * priv;		/* darknet_network_private_t */
	void * user_data;
	
	int batch;
	int training;
	
	int subdivisions;
	int groups;
	
	darknet_size4i input_size;
	darknet_size4i output_size;
	
	float * input;
	const float * output;
	
	size_t layers_count;
	darknet_layer_t ** layers;
	
	size_t 	global_weights_count;
	float * global_weights;
	float * global_weights_update;
	
	size_t 	global_bias_count;
	float * global_bias;
	float * global_bias_update;
	
	size_t 	global_delta_count;
	float * global_delta;
	
	float * workspace;
	size_t 	workspace_size;
	
	int (* load_config)(struct darknet_network * net, const char * conf_file);
	
	int (* forward)(struct darknet_network * net, const float * X /* input */);
	int (* backward)(struct darknet_network * net);
	int (* update)(struct darknet_network * net);
	
	void (* reset)(struct darknet_network * net);
	
};

darknet_network_t * darknet_network_init(darknet_network_t * net, const char * conf_file, void * user_data);
void darknet_network_cleanup(darknet_network_t * net);
void darknet_network_dump(darknet_network_t * net);

#ifdef __cplusplus
}
#endif
#endif
