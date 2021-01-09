#include "JackSymbolTableManager.h"

const char* var_static	= "static";
const char* var_field	= "field";
const char* var_arg		= "arg";
const char* var_local	= "local";

static void print_map(std::map<std::string, Entry>* _map)
{
	for (const auto& pair : *_map) {
		pair.second.print();
	}
}

JackSymbolTabelManager::JackSymbolTabelManager()
{
}

void JackSymbolTabelManager::add_entry_cls_table(const char* _name, const char* _type,	VAR_KIND _kind)
{
	Entry* entry = new Entry();
	entry->name = _name;
	entry->type = _type;

	if (_kind == VAR_KIND::CLS_STATIC)
	{
		entry->kind = var_static;
		entry->index = cls_static_var_index++;
	}
	else
	{
		entry->kind = var_field;
		entry->index = cls_field_var_index++;
	}

	cls_symb_tbl.insert(std::pair<std::string, Entry>(entry->name, *entry));
}

void JackSymbolTabelManager::add_entry_subrt_table(const char* _name, const char* _type, VAR_KIND _kind)
{
	Entry* entry = new Entry();
	entry->name = _name;
	entry->type = _type;

	if (_kind == VAR_KIND::SUBRT_LOCAL)
	{
		entry->kind = var_local;
		entry->index = subrt_local_var_index++;
	}
	else
	{
		entry->kind = var_arg;
		entry->index = subrt_arg_var_index++;
	}

	subrt_symb_tbl.insert(std::pair<std::string, Entry>(entry->name, *entry));
}

void JackSymbolTabelManager::set_current_cls(const char* class_name)
{
	current_cls_name = class_name;
}


void JackSymbolTabelManager::set_current_subrt(const char* subroutine_name)
{
	current_subrt_name = subroutine_name;
}


void JackSymbolTabelManager::reset_cls_var_indices()
{
	cls_static_var_index = 0;
	cls_field_var_index = 0;
}


void JackSymbolTabelManager::reset_subrt_var_indices()
{
	subrt_arg_var_index = 0;
	subrt_local_var_index = 0;
}


void JackSymbolTabelManager::reset_cls_table()
{
	cls_symb_tbl.clear();
}


void JackSymbolTabelManager::reset_subrt_table()
{
	subrt_symb_tbl.clear();
}

void JackSymbolTabelManager::print_cls_table()
{
	print_map(&cls_symb_tbl);
}

void JackSymbolTabelManager::print_subrt_table()
{
	print_map(&subrt_symb_tbl);
}
