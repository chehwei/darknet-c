/*
 * libtest.c
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


int main(int argc, char **argv)
{
	const char * conf_file = "conf/yolov3.json";
	if(argc > 1) conf_file = argv[1];
	
	darknet_network_t * net = darknet_network_init(NULL, conf_file, NULL);
	assert(net);
	
	
	darknet_network_dump(net);
	
	
	darknet_network_cleanup(net);
	
	return 0;
}

