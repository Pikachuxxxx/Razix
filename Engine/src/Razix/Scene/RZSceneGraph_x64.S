#undef SYMBOL
#ifdef __APPLE__
    #define SYMBOL(name) _##name
    .section __TEXT,__text,regular,pure_instructions
#else
    #define SYMBOL(name) name
    .section .text
#endif

.p2align 4

.macro STUB_FUNC f
.globl SYMBOL(\f)
#ifndef __APPLE__
.type SYMBOL(\f), @function
#endif
SYMBOL(\f):
    xor %rax, %rax
    ret
.endm

STUB_FUNC rz_scene_graph_create
STUB_FUNC rz_scene_graph_destroy
STUB_FUNC rz_scene_graph_update
STUB_FUNC rz_zone_activate
STUB_FUNC rz_zone_deactivate
STUB_FUNC rz_zone_get
STUB_FUNC rz_zone_get_const
STUB_FUNC rz_zone_find_by_name
STUB_FUNC rz_zone_find_by_grid_pos
STUB_FUNC rz_scene_graph_resolve_zone_index
STUB_FUNC rz_scene_graph_get_active_zone_index
STUB_FUNC rz_scene_graph_get_active_zone
STUB_FUNC rz_scene_graph_attach_asset
STUB_FUNC rz_scene_graph_create_node
STUB_FUNC rz_scene_graph_detach_node
STUB_FUNC rz_scene_graph_destroy_node
STUB_FUNC rz_scene_graph_get_node
STUB_FUNC rz_scene_graph_get_node_const
STUB_FUNC rz_scene_graph_find_node
STUB_FUNC rz_scene_graph_find_node_by_name
STUB_FUNC rz_scene_graph_find_node_global
STUB_FUNC rz_scene_graph_get_transform
STUB_FUNC rz_scene_graph_set_position
STUB_FUNC rz_scene_graph_set_rotation
STUB_FUNC rz_scene_graph_set_scale
STUB_FUNC rz_scene_graph_update_transforms
STUB_FUNC rz_scene_graph_traverse_depth_first
STUB_FUNC rz_scene_graph_traverse_breadth_first
STUB_FUNC rz_scene_graph_resolve_asset
STUB_FUNC rz_zone_serialize_sexp
STUB_FUNC rz_scene_graph_serialize
STUB_FUNC rz_scene_graph_manager_create
STUB_FUNC rz_scene_graph_manager_destroy
STUB_FUNC rz_scene_graph_manager_load_scene
STUB_FUNC rz_scene_graph_manager_unload_scene
STUB_FUNC rz_scene_graph_manager_queue_scene
STUB_FUNC rz_sexp_tokenize
STUB_FUNC rz_sexp_tokenizer_free
STUB_FUNC rz_sexp_build_zone
STUB_FUNC rz_sexp_parse_scene_header
