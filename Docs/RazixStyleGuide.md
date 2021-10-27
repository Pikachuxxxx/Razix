This document describes the style code rules in **Razix Engine**. The code is written primarily in C++ with some GLSL/HLSL as well.


Table of contents
=================

* [Comments in C++ & GLSL](#comments-in-c--glsl)
* [Naming conventions in C++ & GLSL](#naming-conventions-in-c--glsl)
* [Naming conventions for files and directories](#naming-conventions-for-files-and-directories)
* [C++ rules](#c-rules)

Comments in C++ & GLSL
======================

**All comments are C++ like:**

	// Some comment

**Use ``\\tex:$numLevels = 1 + floor(log2(max(w, h, d)))$`` style comments for math equation in Latex. And for doxygen comments in C++ use JavaDoc * styling**

Ex: 1

    /**
    * A class that manages Textures/Image resources for the Engine
    * @brief It manages creation and conversion of Image resources, also stores in a custom Engine Format depending on how it's being used
    */
Ex: 2

    /**
    * Calculates the Mip Map count based on the Width and Height of the texture
    *
    * @param width     The width of the Texture
    * @param height    The height of the texture
    */

**For Variables use ``/* info about variable */`` with proper spacing normalization**

Ex:

    std::string     m_Name;             /* The name of the texture resource         */
    std::string     m_VirtualPath;      /* The virtual path of the texture          */
    uint32_t        m_Width;            /* The width of the texture                 */
    uint32_t        m_Height;           /* The height of the texture                */
    Type            m_TextureType;      /* The type of this texture                 */
    Format          m_InternalFormat;   /* The internal format of the texture data  */
    Wrapping        m_WrapMode;         /* Wrap mode of the texture                 */
    Filtering       m_FilterMode;       /* Filtering mode of the texture data       */
    bool            m_FlipX;            /* Flip the texture on X-axis during load   */
    bool            m_FlipY;            /* Flip the texture on Y-axis during load   */

**For all inline functions use ``/* info */`` single line comments** no need of @brief, @param or other stuff, just needs a simple single line comment

Naming conventions in C++ & GLSL
================================

**Public Variables and Member Functions including inline functions as well as getters and setters** are lower camelCase. Exception for `RZEngine` and `RZApplication` classes.

	int someVariableA, someVariableB;

    uint32_t calculateMipMapCount(uint32_t width, uint32_t height);

    std::string getName() const { return m_Name; }
    uint32_t getWidth() const { return m_Width; }
    uint32_t getHeight() const { return m_Height; }

**Static and Virtual Functions** are Pascalcase.

    static Texture* Create2D(const std::string& name, uint32_t width, uint32_t height, void* data, Format format, Wrapping wrapMode, Filtering filterMode);

**Any function starting with `OnSomeFunction` is PascalCase**

**No hungarian notations** with 2 exceptions.
    - All private and protected member variables have the `m_` prefix. That applies to classes and structs.
    - All global variables have the `g_` prefix.
    - All static variables have the `s_` prefix.
    - All constant variables have the `k_` prefix.

    In GLSL there are more exceptions:
    - All uniforms (buffers, textures, images, push constants) and storage buffers have the `u_` prefix.
    - All input globals the `in_` prefix.
    - All output globals the `out_` prefix.
    - All shared storage the `s_` prefix.
    - All storage or uniform block names have the  `b_` prefix.

All **types** are PascalCase and that includes classes, structs, typedefs and enums.

	enum MyEnum {...};
	class MyClass {...};
	using MyType = int;

All **Class Names** should start with `RZ` (denoting Razix Engine)

    RZTexture
    RZTSingleton<>
    RZPoolAllocator
    RZOS
    RZTexture

All **macros and defines** should be SCREAMING_SNAKE_CASE and they should have an `RAZIX_` prefix.

	RAZIX_CORE_ASSERT(...);

	#define RAZIX_RENDER_API_OPENGL


Naming conventions for files and directories
============================================

**Filenames and directories should be PascalCase**. The extensions of the files are lowercase.

All Razix Related files should start with *FIleName*. Only custom types use Razix prefix for file names ex. `RazixSingleton` etc.

    src/Razix/Core/Singleton.h


C++ rules
=========

**Always use strongly typed enums**.

    enum class Format
    {
       R8, R16, R32, RG8, RG16, RG32, RGB8, RGB16, RGB32, RGBA8, RGBA16, RGBA32,
       R8F, R16F, R32F, RG8F, RG16F, RG32F, RGB8F, RGB16F, RGB32F, RGBA8F, RGBA16F, RGBA32F
    };

**Never use `typedef`** to define types. Use `using` instead.

	using U32 = uint32_t;

**Never use C-style casting**. Use static_cast, reinterpret_cast or const_cast for most cases and constructor-like cast for fundamental types.

**Avoid using `auto`**. It's only allowed in some templates and in iterators. auto makes reading code difficult.

**Includes should always have the full file path in a alphabetical order of directories and file names**.

    #include "Razix/Core/Core.h"
    #include "Razix/Core/OS/Window.h"
    #include "Razix/Core/SmartPointers.h"

    #include "Razix/Events/ApplicationEvent.h"
    #include "Razix/Events/KeyEvent.h"
    #include "Razix/Events/MouseEvent.h"

    #include "Razix/Utilities/Timestep.h"
    #include "Razix/Utilities/Timer.h"

**Access types in a class have a specified order**. First `public` then `protected` and last `private`. First Variables followed by Methods

**Leave space after every access specifier**

    class MyClass
    {
    public:

    protected:

    private:

    };

**Types/Macros declared in class should appear first under `public`**

    class Texture
           {
               // Texture Types
           public:
               /* The type of the texture */
               enum class Type
               {
                   COLOR, DEPTH, CUBE
               };

           public:
                /* Default constructor, texture resource is done on demand */
                Texture() = default;
                /* Virtual destructor enables the API implementation to delete it's resources */
                virtual ~Texture() {}

Always **use `nullptr`**.

Always try to **comment parts of the source code in header with Doxygen style and implementation with desing/decision information**.


**Never use curly braces and place it below with a tab** on single line expressions.

    if(something)
        doSomething()

**Namespaces, Control Loops** position the braces on same line with a space.

    if(something) {
        isSet = true;
        doSomething();
    }

    namespace Razix {
        namespace Graphics {
        }
    }

**Types, Functions, Lambdas and Scopes** position the braces on a new line.

    uint32_t Texture::calculateMipMapCount(uint32_t width, uint32_t height)
    {
        //tex: The texture mip levels are $numLevels = 1 + floor(log2(max(w, h, d)))$
          uint32_t levels = 1;
          while ((width | height) >> levels)
              levels++;

          return levels;
    }

**Empty functions have braces on same line with a single space**

    MyClass::SomeFunction() {}
