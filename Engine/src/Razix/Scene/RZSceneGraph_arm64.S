#undef SYMBOL
#ifdef __APPLE__
    #define SYMBOL(name) _##name
    .section __TEXT,__text,regular,pure_instructions
#else
    #define SYMBOL(name) name
    .section .text
#endif

.p2align 2

.macro STUB_FUNC name
.globl SYMBOL(\name)
#ifndef __APPLE__
.type SYMBOL(\name), @function
#endif
SYMBOL(\name):
    mov x0, #0
    ret
.endm

#------------------------------------------------------------
# Include Files 
#------------------------------------------------------------
#include "RZSceneGraph_asmcompat.inc"

STUB_FUNC rz_scene_graph_update_transforms
STUB_FUNC rz_scene_graph_traverse_depth_first
STUB_FUNC rz_scene_graph_traverse_breadth_first
STUB_FUNC rz_zone_serialize_sexp
STUB_FUNC rz_scene_graph_serialize
STUB_FUNC rz_sexp_tokenize
STUB_FUNC rz_sexp_tokenizer_free
STUB_FUNC rz_sexp_build_zone
STUB_FUNC rz_sexp_parse_scene_header
