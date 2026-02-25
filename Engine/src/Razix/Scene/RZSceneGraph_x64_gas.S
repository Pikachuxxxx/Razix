#undef SYMBOL
#if defined(__APPLE__)
    #define SYMBOL(name) _##name
    .section __TEXT,__text,regular,pure_instructions
#elif defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
    #define SYMBOL(name) name
    .section .text
#else
    #define SYMBOL(name) name
    .section .text
#endif

.p2align 4

//------------------------------------------------------------
// Platform ABI detection
//------------------------------------------------------------

#if defined _WIN32

    // Microsoft x64 ABI
    // args: rcx, rdx, r8, r9

    // argument registers
    #define ARG0 %rcx
    #define ARG1 %rdx
    #define ARG2 %r8
    #define ARG3 %r9

    // caller-saved
    #define RET  %rax
#else
    // System V ABI (Linux, macOS)
    // args: rdi, rsi, rdx, rcx, r8, r9

    // argument registers
    #define ARG0 %rdi
    #define ARG1 %rsi
    #define ARG2 %rdx
    #define ARG3 %rcx

    // caller-saved
    #define RET  %rax

#endif

//-----------------------------------------------------------------------------
// STUB_FUNC name
//
// Declares a global function symbol and emits a minimal stub implementation
// that immediately returns 0.
//-----------------------------------------------------------------------------
.macro STUB_FUNC name
.globl SYMBOL(\name)
#if !defined(__APPLE__) && !defined(_WIN32) && !defined(__WIN32__) && !defined(__CYGWIN__)
.type SYMBOL(\name), @function
#endif
SYMBOL(\name):
    xor %rax, %rax
    ret
.endm

//-----------------------------------------------------------------------------
// FUNCTION_BEGIN name
//
// Declares a global function symbol properly for all platforms.
//-----------------------------------------------------------------------------
.macro FUNCTION_BEGIN name
    .globl SYMBOL(\name)
#if !defined(__APPLE__) && !defined(_WIN32) && !defined(__WIN32__) && !defined(__CYGWIN__)
    .type SYMBOL(\name), @function
#endif
    SYMBOL(\name):
.endm

//-----------------------------------------------------------------------------
// FUNCTION_END name
//
// Marks function size (needed for ELF, harmless elsewhere)
//-----------------------------------------------------------------------------
.macro FUNCTION_END name
#if !defined(__APPLE__) && !defined(_WIN32) && !defined(__WIN32__) && !defined(__CYGWIN__)
    .size SYMBOL(\name), . - SYMBOL(\name)
#endif
.endm

//------------------------------------------------------------
// Stack frame macros
//------------------------------------------------------------

#define FUNC_PROLOGUE \
    push %rbp;        \
    mov %rsp, %rbp

#define FUNC_EPILOGUE \
    pop %rbp;         \
    ret

//------------------------------------------------------------
// Include Files 
//------------------------------------------------------------
#include "RZSceneGraph_asmcompat.inc"

//-----------------------------------------------------------------------------
// rz_scene_graph_create
//
// Creates and initializes a scene graph inside caller-provided memory.
//
// Parameters:
//   rdi = void* where       # destination memory block
//   esi = u32 numZones      # number of zones to initialize
//
// Returns:
//   rax = rz_scene_graph*
//
// What does it do:
//   - memset(scene_graph, 0, sizeof(rz_scene_graph))
//   - sets version
//   - sets zoneCount
//   - initializes each zone
//   - initializes zones to UNLOADED state
//-----------------------------------------------------------------------------
FUNCTION_BEGIN rz_scene_graph_create
    xor ARG0, ARG0 
    ret
FUNCTION_END rz_scene_graph_create
//-----------------------------------------------------------------------------

STUB_FUNC rz_scene_graph_create_from_file
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
