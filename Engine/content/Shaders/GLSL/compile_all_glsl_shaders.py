import glob, os
from pathlib import Path

# List of all glsl files files that will be compiled
glsl_files = []

def compile_vert_shader(fileName):
    print("Compiling vertex shader... : " + fileName)
    command = "glslc -I ./ -I ../ -fshader-stage=vertex " + fileName + ".vert -o ../Compiled/SPIRV/" + fileName + ".vert.spv"
    status = os.popen(command).read()
    if not status:
        print ("Succefully compiled " + fileName + ".vert")
    else:
        print(status)

def compile_frag_shader(fileName):
    print("Compiling fragment shader... : " + fileName)
    command = "glslc -I ./ -I ../  -fshader-stage=fragment " + fileName + ".frag -o ../Compiled/SPIRV/" + fileName + ".frag.spv"
    status = os.popen(command).read()
    if not status:
        print ("Succefully compiled " + fileName + ".frag")
    else:
        print(status)

def compile_shaders():
    for file in glsl_files:
        name, extension = os.path.splitext(file)
        print(name)
        print(extension)
        if(extension == ".vert"):
            compile_vert_shader(name)
        elif (extension == ".frag"):
            compile_frag_shader(name)
        else:
             print("Unkown shader stage file!")

if __name__ == "__main__":
    print("--------------------------------")
    print("List of shaders to compile:")
    # First we get the list of all *.glsl files
    for path in Path('./').rglob('*.vert'):
        print(path.name)
        glsl_files.append(path.name)

    for path in Path('./').rglob('*.frag'):
        print(path.name)
        glsl_files.append(path.name)
       
    print("--------------------------------")
    # Compile the shaders
    compile_shaders()
