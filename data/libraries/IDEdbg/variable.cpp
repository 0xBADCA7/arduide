/*
  variable.cpp

  This file is part of arduide, The Qt-based IDE for the open-source Arduino electronics prototyping platform.

  Copyright (C) 2010-2016 
  Authors : Denis Martinez
	    Martin Peres

This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "variable.h"
#include "IDEdbgPrivate.h"

const char* variable_type_to_string(variable_type type)
{
	const char* ret;
	switch(type)
	{
		case _error:{ret="error"; break;}
		case _int:{ret="int"; break;}
		case _unsigned_int:{ret="unsigned int"; break;}
		case _char:{ret="char"; break;}
		case _unsigned_char:{ret="unsigned char"; break;}
		case _float:{ret="float"; break;}
		case _double:{ret="double"; break;}
		case _char_pointer:{ret="char*"; break;}
		case _void_pointer:{ret="void*"; break;}
	}
	return ret;
}

variable_type variable_type_from_string(const char* type)
{
	if(strcmp(type, "int")==0)
		return _int;
	else if(strcmp(type, "unsigned int")==0)
		return _unsigned_int;
	else if(strcmp(type, "char")==0)
		return _char;
	else if(strcmp(type, "unsigned char")==0)
		return _unsigned_char;
	else if(strcmp(type, "float")==0)
		return _float;
	else if(strcmp(type, "double")==0)
		return _double;
	else if(strcmp(type, "char*")==0)
		return _char_pointer;
	else if(strcmp(type, "void*")==0)
		return _void_pointer;
	else
		return _error;
}

variable* variable_create(int line, const char* name, variable_type type, int size, void* data)
{
	variable* var = (variable*)malloc(sizeof(variable));

	var->line=line;
	var->name=strdup(name);
	var->type=type;
	var->size=size;
	var->data=data;

	return var;
}

void variable_set_value(variable* var, void* data, size_t size)
{
	memcpy(var->data, data, size);
}

void variable_set_value(variable* var, char* data)
{
	if(!var)
		return;

	switch(var->type)
	{
		case _unsigned_int:
		case _int:
		{
			int i=atoi(data);
			variable_set_value(var, &i, sizeof(int));
			break;
		}
		case _unsigned_char:
		case _char:
		{
			variable_set_value(var, data, sizeof(char));
			break;
		}
		case _float:
		case _double:
		{
			double d=atof(data);
			variable_set_value(var, &d, sizeof(double));
			break;
		}
		case _char_pointer:
		{
			variable_set_value(var, data, strlen(data));
			break;
		}
		case _error:
		case _void_pointer:
		{
			break;
		}
	}
}

void variable_free(variable* var)
{
	if(var)
	{
		free(var->name);
		free(var);
	}
}

void variable_free(void* var)
{
	variable_free((variable*) var);
}
void print_variable(variable* var)
{
	if(var==NULL)
		return;

	const char* s_type=variable_type_to_string(var->type);

	DbgPrintf("<var l=\"%i\" id=\"%s\" t=\"%s\"  v=\"",
			  var->line, var->name, s_type);
	switch(var->type)
	{
		case _int:
		{
			Serial.print(*((int*)var->data), DEC);
			break;
		}
		case _unsigned_int:
		{
			Serial.print(*((unsigned int*)var->data), DEC);
			break;
		}
		case _char:
		{
			Serial.write(*((char*)var->data));
			break;
		}
		case _unsigned_char:
		{
			Serial.write(*((unsigned char*)var->data));
			break;
		}
		case _float:
		{
			Serial.print(*((float*)var->data), 4);
			break;
		}
		case _double:
		{
			Serial.print(*((double*)var->data), 4);
			break;
		}
		case _char_pointer:
		{
			Serial.print(*((char**)var->data));
			break;
		}
		case _error:
		case _void_pointer:
		{
			Serial.print("0x");
			Serial.print((int)var->data, HEX);
			break;
		}
	}
	Serial.print("\" />");
}

char* show_variable(void* data)
{
	variable* var=(variable*)data;

	if(var==NULL)
		return NULL;

	const char* s_type=variable_type_to_string(var->type);

	int size=strlen(s_type)+1+strlen(var->name)+3+1+1;

	// Get the pattern
	char* ret;
	const char* pattern;
	switch(var->type)
	{
		case _int:
		{
			size+=2+10;
			ret=(char*)malloc(size);
			snprintf(ret, size, "%s %s = %i;", s_type, var->name, *((int*)var->data));
			break;
		}
		case _unsigned_int:
		{
			size+=2+10;
			ret=(char*)malloc(size);
			snprintf(ret, size, "%s %s = %u;", s_type, var->name, *((unsigned int*)var->data));
			break;
		}
		case _char:
		{
			size+=2+1;
			ret=(char*)malloc(size);
			snprintf(ret, size, "%s %s = %c;", s_type, var->name, *((char*)var->data));
			break;
		}
		case _unsigned_char:
		{
			size+=2+1;
			ret=(char*)malloc(size);
			snprintf(ret, size, "%s %s = %c;", s_type, var->name, *((unsigned char*)var->data));
			break;
		}
		case _float:
		{
			size+=2+15;
			ret=(char*)malloc(size);
			snprintf(ret, size, "%s %s = %f;", s_type, var->name, *((float*)var->data));
			break;
		}
		case _double:
		{
			size+=2+15;
			ret=(char*)malloc(size);
			snprintf(ret, size, "%s %s = %e;", s_type, var->name, *((double*)var->data));
			break;
		}
		case _char_pointer:
		{
			size+=2+strlen(*((char**)var->data))+2;
			ret=(char*)malloc(size);
			snprintf(ret, size, "%s %s = '%s';", s_type, var->name, *((char**)var->data));
			break;
		}
		case _error:
		case _void_pointer:
		{
			size+=2+strlen((char*)var->data)+2;
			ret=(char*)malloc(size);
			snprintf(ret, size, "%s %s = %p;", s_type, var->name, var->data);
			break;
		}
	}

	return ret;
}