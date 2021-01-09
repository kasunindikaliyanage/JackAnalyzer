#pragma once
#include <map>
#include <iostream>


enum VAR_KIND
{
	CLS_STATIC,
	CLS_FIELD,
	SUBRT_ARG,
	SUBRT_LOCAL
};

struct Entry
{
	std::string name;
	std::string type;
	std::string kind;
	int index;

	const void print() const
	{
		std::cout << name << " " << type << " " << kind << " " << index << std::endl;
	}
};

class JackSymbolTabelManager {

	// need only to maps. One for class and one for subroutine
	std::map<std::string, Entry> cls_symb_tbl;
	std::map<std::string, Entry> subrt_symb_tbl;

	//needed only for logging purpose
	std::string current_cls_name = "";
	std::string current_subrt_name = "";

	//TODO it seems there is no need to keep global variables for below 4.
	int cls_static_var_index = 0;
	int cls_field_var_index = 0;

	int subrt_arg_var_index = 0;
	int subrt_local_var_index = 0;
public:

	JackSymbolTabelManager();

	void add_entry_cls_table(const char* _name, const char* _type, VAR_KIND _kind);

	void add_entry_subrt_table(const char* _name, const char* _type, VAR_KIND _kind);

	void set_current_cls(const char* _class_name);

	void set_current_subrt(const char* _subroutine_name);

	void reset_cls_var_indices();

	void reset_subrt_var_indices();

	void reset_cls_table();

	void reset_subrt_table();

	void print_cls_table();

	void print_subrt_table();
};