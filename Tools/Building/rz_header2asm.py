"""
Converts headers to gen_XXX_asmcompat.inc to include in GAS/ARM assembly files to resolve struct offsets and enum values

Usage: python rz_header2asm.py RZSceneGraph.h gen_RZSceneGraph_asmcompat.inc 
"""
import sys
import clang.cindex

OUTPUT = []

def emit(line):
    OUTPUT.append(line)

def process_enum(cursor):
    enum_name = cursor.spelling
    if not enum_name:
        return

    emit(f"\n/* enum {enum_name} */")

    enum_type = cursor.enum_type
    size = enum_type.get_size()

    emit(f"#define SIZEOF_{enum_name} {size}")

    for child in cursor.get_children():
        if child.kind == clang.cindex.CursorKind.ENUM_CONSTANT_DECL:
            name = child.spelling
            value = child.enum_value
            emit(f"#define {name} {value}")

def process_struct(cursor):
    struct_name = cursor.spelling
    if not struct_name:
        return

    emit(f"\n/* struct {struct_name} */")

    struct_type = cursor.type
    size = struct_type.get_size()

    emit(f"#define SIZEOF_{struct_name} {size}")

    for child in cursor.get_children():
        if child.kind == clang.cindex.CursorKind.FIELD_DECL:
            field_name = child.spelling
            offset_bits = child.get_field_offsetof()
            offset_bytes = offset_bits // 8
            emit(f"#define OFFSETOF_{struct_name}_{field_name} {offset_bytes}")

def visit(cursor):
    if cursor.kind == clang.cindex.CursorKind.ENUM_DECL:
        process_enum(cursor)

    elif cursor.kind == clang.cindex.CursorKind.STRUCT_DECL:
        process_struct(cursor)

    for child in cursor.get_children():
        visit(child)

def main():
    if len(sys.argv) < 3:
        print("Usage:")
        print("  python rz_header2asm.py input.h output.inc")
        sys.exit(1)

    input_header = sys.argv[1]
    output_file = sys.argv[2]

    index = clang.cindex.Index.create()

    tu = index.parse(
        input_header,
        args=[
            "-x", "c++",
            "-std=c++17",
            "-T./"
            "-I./Engine/src/Razix",
            "-I./Engine/src/",
        ]
    )

    emit("/* AUTO GENERATED - DO NOT EDIT */")

    visit(tu.cursor)

    with open(output_file, "w") as f:
        f.write("\n".join(OUTPUT))

    print(f"Generated {output_file}")

if __name__ == "__main__":
    main()
