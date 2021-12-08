import glob, os

# List of all glsl files files that will be compiled
glsl_files = []

def compile_vert_shader(fileName):
    print("Compiling vertex shader... : " + fileName)
    command = "glslc -fshader-stage=vertex " + fileName + ".vert -o ../Compiled/SPIRV/" + fileName + ".vert.spv"
    status = os.popen(command).read()
    if not status:
        print ("Succefully compiled " + fileName + ".vert")
    else:
        print(status)

def compile_frag_shader(fileName):
    print("Compiling fragment shader... : " + fileName)
    command = "glslc -fshader-stage=fragment " + fileName + ".frag -o ../Compiled/SPIRV/" + fileName + ".frag.spv"
    status = os.popen(command).read()
    if not status:
        print ("Succefully compiled " + fileName + ".frag")
    else:
        print(status)

def compile_shaders():
    for file in glsl_files:
        split_file = file.split(".")
        if(split_file[1] == "vert"):
            compile_vert_shader(split_file[0])
        elif (split_file[1] == "frag"):
            compile_frag_shader(split_file[0])
        else:
             print("Unkown shader stage file!")

if __name__ == "__main__":
    # First we get the list of all *.glsl files
    for file in glob.glob("*.vert"):
        print(file)
        glsl_files.append(file)

    for file in glob.glob("*.frag"):
        print(file)
        glsl_files.append(file)
    # Compile the shaders
    compile_shaders()
