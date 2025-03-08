#pragma once
#include <map>
#include <vector>
#include <string>
#include "Const.h"

inline std::vector<std::string> mono_native_func_name = {
    "g_free", "mono_free", "mono_get_root_domain", "mono_thread_attach", "mono_thread_detach",
    "mono_thread_cleanup", "mono_object_get_class", "mono_domain_foreach", "mono_domain_set",
    "mono_domain_get", "mono_assembly_foreach", "mono_assembly_get_image", "mono_image_get_assembly",
    "mono_image_get_name", "mono_image_get_filename", "mono_image_get_table_info", "mono_image_open_from_data", "mono_image_rva_map",
    "mono_table_info_get_rows", "mono_metadata_decode_row_col", "mono_metadata_string_heap", "mono_class_get",
    "mono_class_from_typeref", "mono_class_name_from_token", "mono_class_from_name_case", "mono_class_from_name",
    "mono_class_get_name", "mono_class_get_namespace", "mono_class_get_methods", "mono_class_get_method_from_name",
    "mono_class_get_fields", "mono_class_get_parent", "mono_class_get_image", "mono_class_is_generic",
    "mono_class_vtable", "mono_class_from_mono_type", "mono_class_get_element_class", "mono_class_instance_size",
    "mono_class_num_fields", "mono_class_num_methods", "mono_field_get_name", "mono_field_get_type",
    "mono_field_get_parent", "mono_field_get_offset", "mono_field_get_flags", "mono_type_get_name",
    "mono_type_get_type", "mono_type_get_name_full", "mono_method_get_name", "mono_method_get_class",
    "mono_method_get_header", "mono_method_signature", "mono_method_get_param_names", "mono_signature_get_desc",
    "mono_signature_get_params", "mono_signature_get_param_count", "mono_signature_get_return_type",
    "mono_compile_method", "mono_free_method", "mono_jit_info_table_find", "mono_jit_info_get_method",
    "mono_jit_info_get_code_start", "mono_jit_info_get_code_size", "mono_jit_exec", "mono_method_header_get_code",
    "mono_disasm_code", "mono_vtable_get_static_field_data", "mono_method_desc_new", "mono_method_desc_from_method",
    "mono_method_desc_free", "mono_string_new", "mono_string_to_utf8", "mono_array_new", "mono_value_box",
    "mono_object_unbox", "mono_object_new", "mono_class_get_type", "mono_class_get_nesting_type", "mono_image_open",
    "mono_method_desc_search_in_image", "mono_runtime_invoke", "mono_runtime_object_init", "mono_assembly_name_new",
    "mono_assembly_loaded", "mono_assembly_open", "mono_field_static_get_value", "mono_field_static_set_value",
    "mono_class_get_field_from_name", "mono_method_get_flags", "mono_type_get_class", "mono_class_get_flags"
};

inline std::map<std::string, std::vector<int>> mono_native_func_property = {
    {"g_free", {1, TYPE_VOID}},
    {"mono_free" , {1, TYPE_VOID}},
    {"mono_get_root_domain" , {0, TYPE_VOID_P}},
    {"mono_thread_attach" , {1, TYPE_VOID_P}},
    {"mono_thread_detach" , {1, TYPE_VOID}},
    {"mono_thread_cleanup" , {0, TYPE_VOID}},
    {"mono_object_get_class" , {1, TYPE_VOID_P}},
    {"mono_domain_foreach" , {2, TYPE_VOID}},
    {"mono_domain_set" , {2, TYPE_INT32}},
    {"mono_domain_get" , {0, TYPE_VOID_P}},
    {"mono_assembly_foreach" , {2, TYPE_INT32}},
    {"mono_assembly_get_image" , {1, TYPE_VOID_P}},
    {"mono_image_get_assembly" , {1, TYPE_VOID_P}},
    {"mono_image_get_name" , {1, TYPE_CHAR_P}},
    {"mono_image_get_filename" , {1, TYPE_CHAR_P}},
    {"mono_image_get_table_info" , {2, TYPE_VOID_P}},
	{"mono_image_open_from_data", {4, TYPE_VOID_P}},
    {"mono_image_rva_map" , {2, TYPE_VOID_P}},
    {"mono_table_info_get_rows" , {1, TYPE_INT32}},
    {"mono_metadata_decode_row_col" , {3, TYPE_INT32}},
    {"mono_metadata_string_heap" , {2, TYPE_CHAR_P}},
    {"mono_class_get" , {2, TYPE_VOID_P}},
    {"mono_class_from_typeref" , {2, TYPE_VOID_P}},
    {"mono_class_name_from_token" , {2, TYPE_CHAR_P}},
    {"mono_class_from_name_case" , {3, TYPE_VOID_P}},
    {"mono_class_from_name" , {3, TYPE_VOID_P}},
    {"mono_class_get_name" , {1, TYPE_CHAR_P}},
    {"mono_class_get_flags" , {1, TYPE_INT32}},
    {"mono_class_get_namespace" , {1, TYPE_CHAR_P}},
    {"mono_class_get_methods" , {2, TYPE_VOID_P}},
    {"mono_class_get_method_from_name" , {3, TYPE_VOID_P}},
    {"mono_class_get_fields" , {2, TYPE_VOID_P}},
    {"mono_class_get_parent" , {1, TYPE_VOID_P}},
    {"mono_class_get_image" , {1, TYPE_VOID_P}},
    {"mono_class_is_generic" , {1, TYPE_INT32}},
    {"mono_class_vtable" , {2, TYPE_VOID_P}},
    {"mono_class_from_mono_type" , {1, TYPE_VOID_P}},
    {"mono_class_get_element_class" , {1, TYPE_VOID_P}},
    {"mono_class_instance_size" , {1, TYPE_INT32}},
    {"mono_class_num_fields" , {1, TYPE_INT32}},
    {"mono_class_num_methods" , {1, TYPE_INT32}},
    {"mono_field_get_name" , {1, TYPE_CHAR_P}},
    {"mono_field_get_type" , {1, TYPE_VOID_P}},
    {"mono_field_get_parent" , {1, TYPE_VOID_P}},
    {"mono_field_get_offset" , {1, TYPE_INT32}},
    {"mono_field_get_flags" , {1, TYPE_INT32}},
    {"mono_type_get_name" , {1, TYPE_CHAR_P}},
    {"mono_type_get_type" , {1, TYPE_INT32}},
    {"mono_type_get_name_full" , {2, TYPE_CHAR_P}},
    {"mono_method_get_name" , {1, TYPE_CHAR_P}},
    {"mono_method_get_class" , {1, TYPE_VOID_P}},
    {"mono_method_get_header" , {1, TYPE_VOID_P}},
    {"mono_method_signature" , {1, TYPE_VOID_P}},
    {"mono_method_get_param_names" , {2, TYPE_VOID_P}},
    {"mono_signature_get_desc" , {2, TYPE_CHAR_P}},
    {"mono_signature_get_params" , {2, TYPE_VOID_P}},
    {"mono_signature_get_param_count" , {1, TYPE_INT32}},
    {"mono_signature_get_return_type" , {1, TYPE_VOID_P}},
    {"mono_compile_method" , {1, TYPE_VOID_P}},
    {"mono_free_method" , {1, TYPE_VOID}},
    {"mono_jit_info_table_find" , {2, TYPE_VOID_P}},
    {"mono_jit_info_get_method" , {1, TYPE_VOID_P}},
    {"mono_jit_info_get_code_start" , {1, TYPE_VOID_P}},
    {"mono_jit_info_get_code_size" , {1, TYPE_INT32}},
    {"mono_jit_exec" , {4, TYPE_INT32}},
    {"mono_method_header_get_code" , {3, TYPE_VOID_P}},
    {"mono_disasm_code" , {4, TYPE_CHAR_P}},
    {"mono_vtable_get_static_field_data" , {1, TYPE_VOID_P}},
    {"mono_method_desc_new" , {2, TYPE_VOID_P}},
    {"mono_method_desc_from_method" , {1, TYPE_VOID_P}},
    {"mono_method_desc_free" , {1, TYPE_VOID}},
    {"mono_string_new" , {2, TYPE_VOID_P}},
    {"mono_string_to_utf8" , {1, TYPE_CHAR_P}},
    {"mono_array_new" , {3, TYPE_VOID_P}},
    {"mono_value_box" , {3, TYPE_VOID_P}},
    {"mono_object_unbox" , {1, TYPE_VOID_P}},
    {"mono_object_new" , {2, TYPE_VOID_P}},
    {"mono_class_get_type" , {1, TYPE_VOID_P}},
    {"mono_class_get_nesting_type" , {1, TYPE_VOID_P}},
    {"mono_method_desc_search_in_image" , {2, TYPE_VOID_P}},
    {"mono_runtime_invoke" , {4, TYPE_VOID_P}},
    {"mono_runtime_object_init" , {1, TYPE_VOID_P}},
    {"mono_assembly_name_new" , {1, TYPE_VOID_P}},
    {"mono_assembly_loaded" , {1, TYPE_VOID_P}},
    {"mono_assembly_open" , {2, TYPE_VOID_P}},
    {"mono_image_open" , {2, TYPE_VOID_P}},
    {"mono_field_static_get_value" , {3, TYPE_VOID_P}},
    {"mono_field_static_set_value" , {3, TYPE_VOID_P}},
    {"mono_class_get_field_from_name" , {2, TYPE_VOID_P}},
    {"mono_method_get_flags" , {2, TYPE_UINT32}},
    {"mono_type_get_class" , {1, TYPE_VOID_P}},
};

inline int FIELD_ATTRIBUTE_FIELD_ACCESS_MASK = 0x0007;
inline int FIELD_ATTRIBUTE_COMPILER_CONTROLLED = 0x0000;
inline int FIELD_ATTRIBUTE_PRIVATE = 0x0001;
inline int FIELD_ATTRIBUTE_FAM_AND_ASSEM = 0x0002;
inline int FIELD_ATTRIBUTE_ASSEMBLY = 0x0003;
inline int FIELD_ATTRIBUTE_FAMILY = 0x0004;
inline int FIELD_ATTRIBUTE_FAM_OR_ASSEM = 0x0005;
inline int FIELD_ATTRIBUTE_PUBLIC = 0x0006;
inline int FIELD_ATTRIBUTE_STATIC = 0x0010;
inline int FIELD_ATTRIBUTE_INIT_ONLY = 0x0020;
inline int FIELD_ATTRIBUTE_LITERAL = 0x0040;
inline int FIELD_ATTRIBUTE_NOT_SERIALIZED = 0x0080;
inline int FIELD_ATTRIBUTE_SPECIAL_NAME = 0x0200;
inline int FIELD_ATTRIBUTE_PINVOKE_IMPL = 0x2000;
inline int FIELD_ATTRIBUTE_RESERVED_MASK = 0x9500;
inline int FIELD_ATTRIBUTE_RT_SPECIAL_NAME = 0x0400;
inline int FIELD_ATTRIBUTE_HAS_FIELD_MARSHAL = 0x1000;
inline int FIELD_ATTRIBUTE_HAS_DEFAULT = 0x8000;
inline int FIELD_ATTRIBUTE_HAS_FIELD_RVA = 0x0100;


inline int METHOD_ATTRIBUTE_STATIC = 0x0010;
inline int METHOD_ATTRIBUTE_FINAL = 0x0020;
inline int METHOD_ATTRIBUTE_VIRTUAL = 0x0040;
inline int METHOD_ATTRIBUTE_HIDE_BY_SIG = 0x0080;