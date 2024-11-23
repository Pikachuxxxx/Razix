#ifndef RZSTL_SMART_POINTER_H
#define RZSTL_SMART_POINTER_H

#include "RZSTL/ref_counter.h"

#include <EASTL/shared_ptr.h>

#undef CUSTOM_SMART_PTR
//#define CUSTOM_SMART_PTR

// https://stackoverflow.com/questions/44413478/abstract-class-and-unique-pointer

namespace Razix {
    namespace rzstl {

        using nullptr_t = decltype(nullptr);

        // IDK: What about unique and shared pointer?
        //Custom reference based smart pointers
        /// <summary>
        /// Manages the counter for different types of smart pointers and global reference count
        /// </summary>
        class RefCounterManager
        {
        public:
            RefCounterManager();
            ~RefCounterManager();

            /// <summary>
            /// Tells whether or not the object is being referenced at all or is a free f32ing object
            /// </summary>
            inline bool IsReferenced() const { return m_RefCounterInit.GetRefCount() < 1; }

            /// <summary>
            ///
            /// </summary>
            /// <returns></returns>
            bool InitRef();

            /// <summary>
            ///
            /// </summary>
            /// <returns>  Returns false if refcount is at zero and didn't get increased </returns>
            bool Reference();

            /// <summary>
            ///
            /// </summary>
            /// <returns></returns>
            bool Unreference();

            /// <summary>
            ///
            /// </summary>
            /// <returns></returns>
            bool WeakReference();

            /// <summary>
            ///
            /// </summary>
            /// <returns></returns>
            bool WeakUnreference();

            /// <summary>
            ///
            /// </summary>
            /// <returns></returns>
            int GetReferenceCount() const;

            /// <summary>
            ///
            /// </summary>
            /// <returns></returns>
            int GetWeakReferenceCount() const;

        private:
            ///	The global reference counter for all objects
            ReferenceCounter m_RefCounter;
            /// Initializer counter for individual objects, this is what is used by the objects upon their creation
            ReferenceCounter m_RefCounterInit;
            /// Reference counter for Weak pointers
            ReferenceCounter m_WeakRefCounter;
        };
        //--------------------------------------------------------------------------------------------------------//
        /// <summary>
        /// Contains the reference to an Object
        /// </summary>
        /// <typeparam name="T"> Any class object of type T </typeparam>
        template<class T>
        class Reference
        {
        public:
            Reference() noexcept
                : m_Ptr(nullptr) {}

            Reference(rzstl::nullptr_t) noexcept
                : m_Ptr(nullptr), m_Counter(nullptr) {}

            Reference(T* ptr) noexcept
                : m_Ptr(nullptr), m_Counter(nullptr)
            {
                if (ptr) refPointer(ptr);
            }

            Reference(const Reference<T>& other) noexcept
                : m_Ptr(nullptr), m_Counter(nullptr) { ref(other); }

            Reference(Reference<T>&& rhs) noexcept
                : m_Ptr(nullptr), m_Counter(nullptr) { ref(rhs); }

            template<typename U>
            inline Reference(const Reference<U>& moving) noexcept
            {
                U* movingPtr = moving.get();

                T* castPointer = static_cast<T*>(movingPtr);

                unref();

                if (castPointer != nullptr) {
                    if (moving.get() == m_Ptr)
                        return;

                    if (moving.GetCounter() && moving.get()) {
                        m_Ptr     = castPointer;
                        m_Counter = moving.GetCounter();
                        m_Counter->Reference();
                    }
                }
            }

            ~Reference() noexcept { unref(); }

            inline T* get() const { return m_Ptr; }

            inline RefCounterManager* GetCounter() const { return m_Counter; }

            inline T* release() noexcept
            {
                T* tmp = nullptr;
                if (m_Counter->Unreference()) {
                    delete m_Counter;
                    m_Counter = nullptr;
                }
                eastl::swap(tmp, m_Ptr);
                m_Ptr = nullptr;

                return tmp;
            }

            inline void reset(T* p_ptr = nullptr)
            {
                unref();

                m_Ptr     = p_ptr;
                m_Counter = nullptr;

                if (m_Ptr != nullptr) {
                    m_Counter = new RefCounterManager();
                    m_Counter->InitRef();
                }
            }

            inline void       operator=(Reference const& rhs) { ref(rhs); }
            inline Reference& operator=(Reference&& rhs) noexcept
            {
                ref(rhs);
                return *this;
            }
            inline Reference& operator=(T* newData)
            {
                reset(newData);
                return *this;
            }
            template<typename U>
            inline Reference& operator=(const Reference<U>& moving)
            {
                U* movingPtr = moving.get();

                T* castPointer = dynamic_cast<T*>(movingPtr);

                unref();

                if (castPointer != nullptr) {
                    if (moving.GetCounter() && moving.get()) {
                        m_Ptr     = moving.get();
                        m_Counter = moving.GetCounter();
                        m_Counter->Reference();
                    }
                }
                return *this;
            }
            inline Reference& operator=(rzstl::nullptr_t)
            {
                reset();
                return *this;
            }
            inline T*                 operator->() const { return const_cast<T*>(reinterpret_cast<const T*>(this)); }
            inline T&                 operator&() const { return *m_Ptr; }
            inline T&                 operator[](int index) const { return m_Ptr[index]; }
            inline explicit constexpr operator bool() const { return m_Ptr != nullptr; }
            inline constexpr bool     operator==(const T* p_ptr) const { return m_Ptr == p_ptr; }
            inline constexpr bool     operator!=(const T* p_ptr) const { return m_Ptr != p_ptr; }
            inline constexpr bool     operator<(const Reference<T>& p_r) const { return m_Ptr < p_r.m_Ptr; }
            inline constexpr bool     operator==(const Reference<T>& p_r) const { return m_Ptr == p_r.m_Ptr; }
            inline constexpr bool     operator!=(const Reference<T>& p_r) const { return m_Ptr != p_r.m_Ptr; }
            inline void               swap(Reference& other) noexcept
            {
                eastl::swap(m_Ptr, other.m_Ptr);
                eastl::swap(m_Counter, other.m_Counter);
            }
            template<typename U>
            inline Reference<U> As() const
            {
                return Reference<U>(*this);
            }

        private:
            /// <summary>
            /// Creates a reference from another reference
            /// </summary>
            /// <param name="pFrom"></param>
            inline void ref(const Reference& pFrom)
            {
                // Check if it's the same thing
                if (pFrom.m_Ptr == m_Ptr)
                    return;

                // Unref any previous references
                unref();

                m_Counter = nullptr;
                m_Ptr     = nullptr;

                // Check if
                if (pFrom.GetCounter() && pFrom.get()) {
                    m_Ptr     = pFrom.get();
                    m_Counter = pFrom.GetCounter();
                    m_Counter->Reference();
                }
            }

            /// <summary>
            /// Creates a reference from another pointer
            /// </summary>
            /// <param name="pFrom"></param>
            inline void refPointer(T* ptr)
            {
                m_Ptr     = ptr;
                m_Counter = new rzstl::RefCounterManager();
                m_Counter->InitRef();
            }

            /// <summary>
            /// Dereferences the smart pointer properly
            /// </summary>
            inline void unref()
            {
                // Check if the counter exists
                if (m_Counter != nullptr) {
                    // Unreference the pointer from the counter
                    if (m_Counter->Unreference()) {
                        // delete the pointer
                        delete m_Ptr;
                        // Check is the pointer is weakly referenced and remove it also
                        if (m_Counter->GetWeakReferenceCount() == 0)
                            delete m_Counter;

                        // Remove any further references after deleting the pointers
                        m_Ptr     = nullptr;
                        m_Counter = nullptr;
                    }
                }
            }

            /// Pointer to the Object being referred to
            T* m_Ptr = nullptr;
            /// Reference counter to track the Referencing nature of the object (pointer cause to transfer this object with the parent ref, all around)
            rzstl::RefCounterManager* m_Counter = nullptr;
        };
        //--------------------------------------------------------------------------------------------------------//
        /// <summary>
        /// Creates a weakly referenced smart pointer
        /// </summary>
        /// <typeparam name="T"> Any class object of type T </typeparam>
        template<class T>
        class WeakReference
        {
        public:
            WeakReference() noexcept
                : m_Ptr(nullptr), m_Counter(nullptr)
            {
            }

            WeakReference(rzstl::nullptr_t) noexcept
                : m_Ptr(nullptr), m_Counter(nullptr)
            {
            }

            WeakReference(const WeakReference<T>& rhs) noexcept
                : m_Ptr(rhs.m_Ptr), m_Counter(rhs.m_Counter)
            {
                add_ref();
            }

            explicit WeakReference(T* ptr) noexcept
                : m_Ptr(ptr)
            {
                m_Counter = new rzstl::RefCounterManager();
                m_Counter->WeakReference();
            }

            template<class U>
            WeakReference(const WeakReference<U>& rhs) noexcept
                : m_Ptr(rhs.m_Ptr), m_Counter(rhs.m_Counter)
            {
                add_ref();
            }

            WeakReference(const Reference<T>& rhs) noexcept
                : m_Ptr(rhs.get()), m_Counter(rhs.m_Counter)
            {
                add_ref();
            }

            ~WeakReference() noexcept
            {
                if (m_Counter->WeakUnreference()) {
                    delete m_Ptr;
                }
            }

            void add_ref()
            {
                m_Counter->WeakReference();
            }

            bool expired() const
            {
                return m_Counter ? m_Counter->GetReferenceCount() <= 0 : true;
            }

            Reference<T> lock() const
            {
                if (expired())
                    return Reference<T>();
                else
                    return Reference<T>(m_Ptr);
            }

            inline T* operator->() const
            {
                return &*m_Ptr;
            }
            inline T& operator*() const
            {
                return *m_Ptr;
            }

            inline T& operator[](int index)
            {
                assert(m_Ptr);
                return m_Ptr[index];
            }

            inline explicit operator bool() const
            {
                return m_Ptr != nullptr;
            }
            inline bool operator==(const T* p_ptr) const
            {
                return m_Ptr == p_ptr;
            }
            inline bool operator!=(const T* p_ptr) const
            {
                return m_Ptr != p_ptr;
            }
            inline bool operator<(const WeakReference<T>& p_r) const
            {
                return m_Ptr < p_r.m_Ptr;
            }
            inline bool operator==(const WeakReference<T>& p_r) const
            {
                return m_Ptr == p_r.m_Ptr;
            }
            inline bool operator!=(const WeakReference<T>& p_r) const
            {
                return m_Ptr != p_r.m_Ptr;
            }

        private:
            T*                        m_Ptr;
            rzstl::RefCounterManager* m_Counter = nullptr;
        };
        //--------------------------------------------------------------------------------------------------------//
        /// <summary>
        /// IDK what this does
        /// </summary>
        /// <typeparam name="T"></typeparam>
        template<class T>
        class Owned
        {
        public:
            explicit Owned(rzstl::nullptr_t)
            {
                m_Ptr = nullptr;
            }

            Owned(T* ptr = nullptr)
            {
                m_Ptr = ptr;
            }

            template<typename U>
            explicit Owned(U* ptr)
            {
                m_Ptr = dynamic_cast<T*>(ptr);
            }

            ~Owned()
            {
                delete m_Ptr;
            }

            Owned(Owned const&)            = delete;
            Owned& operator=(Owned const&) = delete;

            inline Owned(Owned&& moving) noexcept
            {
                moving.swap(*this);
            }

            inline Owned& operator=(Owned&& moving) noexcept
            {
                moving.swap(*this);
                return *this;
            }

            template<typename U>
            inline Owned(Owned<U>&& moving)
            {
                Owned<T> tmp(moving.release());
                tmp.swap(*this);
            }
            template<typename U>
            inline Owned& operator=(Owned<U>&& moving)
            {
                Owned<T> tmp(moving.release());
                tmp.swap(*this);
                return *this;
            }

            inline Owned& operator=(rzstl::nullptr_t)
            {
                reset();
                return *this;
            }

            // Const correct access owned object
            T* operator->() const
            {
                return m_Ptr;
            }
            T& operator*() const
            {
                return *m_Ptr;
            }

            // Access to smart pointer state
            T* get() const
            {
                return m_Ptr;
            }

            explicit operator bool() const
            {
                return m_Ptr;
            }

            // Modify object state
            inline T* release()
            {
                T* result = nullptr;
                eastl::swap(result, m_Ptr);
                return result;
            }

            inline void reset()
            {
                T* tmp = release();
                delete tmp;
            }

            inline void swap(Owned& src) noexcept
            {
                eastl::swap(m_Ptr, src.m_Ptr);
            }

        private:
            T* m_Ptr = nullptr;
        };

        template<typename T>
        void swap(Owned<T>& lhs, Owned<T>& rhs) noexcept
        {
            lhs.swap(rhs);
        }
#ifdef CUSTOM_SMART_PTR

        template<class T>
        using Ref = Reference<T>;

        template<typename T, typename... Args>
        rzstl::Ref<T> CreateRef(Args&&... args)
        {
            auto ptr = new T(eastl::forward<Args>(args)...);

            return Reference<T>(ptr);
        }

        template<class T>
        using UniqueRef = Owned<T>;

        template<typename T, typename... Args>
        UniqueRef<T> CreateUniqueRef(Args&&... args)
        {
            auto ptr = new T(eastl::forward<Args>(args)...);
            return Owned<T>(ptr);
        }

        template<class T>
        using WeakRef = WeakReference<T>;
#elif defined(CUSTOM_SMART_PTR_EASTL)
        template<class T>
        using Ref = eastl::shared_ptr<T>;

        template<typename T, typename... Args>
        Ref<T> CreateRef(Args&&... args)
        {
            return eastl::make_shared<T>(eastl::forward<Args>(args)...);
        }

        template<class T>
        using WeakRef = eastl::weak_ptr<T>;

        template<class T>
        using UniqueRef = eastl::unique_ptr<T>;

        template<typename T, typename... Args>
        UniqueRef<T> CreateUniqueRef(Args&&... args)
        {
            return eastl::make_unique<T>(eastl::forward<Args>(args)...);
        }
#else
        template<class T>
        using Ref = std::shared_ptr<T>;

        template<typename T, typename... Args>
        Ref<T> CreateRef(Args&&... args)
        {
            return std::make_shared<T>(std::forward<Args>(args)...);
        }

        template<class T>
        using WeakRef = std::weak_ptr<T>;

        template<class T>
        using UniqueRef = std::unique_ptr<T>;

        template<typename T, typename... Args>
        UniqueRef<T> CreateUniqueRef(Args&&... args)
        {
            return std::make_unique<T>(std::forward<Args>(args)...);
        }
#endif
    }    // namespace rzstl
}    // namespace Razix

//#ifdef CUSTOM_SMART_PTR
//namespace std {
//    template<typename T>
//    struct hash<Razix::Reference<T>>
//    {
//        sz operator()(const Razix::Reference<T>& x) const
//        {
//            return hash<T*>()(x.get());
//        }
//    };
//}    // namespace std
//#endif
#endif
