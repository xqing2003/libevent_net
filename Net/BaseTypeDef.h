#pragma once


//重定义select的 setsize
#undef FD_SETSIZE
#define FD_SETSIZE 1024

////////////////////////////////////////
//basic type define
////////////////////////////////////////

typedef unsigned int   uint;
typedef unsigned long  ulong;
//typedef long long      int64;
typedef const char* PCSTR;
typedef char* PSTR;

typedef int SessionId;

// define in different type with runtime lib
#ifdef _UNICODE
typedef char CHAR;
#else
typedef char CHAR;
#endif

typedef unsigned char byte;
typedef unsigned char uint8;
typedef unsigned short uint16;

typedef int int32;
typedef unsigned int uint32;
typedef long long int64;

// !TODO
// ///////////////////////////////////
typedef signed __int64 int64;
typedef unsigned __int64 uint64;
typedef uint64 Guid;
// ///////////////////////////////////

typedef int BOOL;

typedef unsigned long DWORD;

typedef int ErrorCode;

typedef Guid ItemID;
typedef Guid HeroID;
typedef Guid PlayerID;

#define INVALID_GUID 0xFFFFFFFFFFFFFFFF
#define INVALID_UID 0x0
//typedef uint32 SpellID;
////////////////////////////////////////
//basic macro define
////////////////////////////////////////

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) if(nullptr != (x)) { delete (x); (x) = nullptr; }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) if(nullptr != (x)) { delete [](x); (x) = nullptr; }
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#ifndef NULL
#define NULL 0
#endif

#define  invalid_id (-1)
#define  invalid_packetId (0)

#define API_DECLSPEC 
// DLL Export Part
//#ifdef _COMMON_EXPORT
//#define API_DECLSPEC    __declspec(dllexport) 
//#else  
//#define API_DECLSPEC    __declspec(dllimport) 
//#endif

// Smart Ptr Part
#define DECLARE_STRUCT_PTR(Class) \
struct Class; \
__DECLARE_CLASS_PTR__(Class)

#define DECLARE_CLASS_PTR(Class) \
class Class; \
__DECLARE_CLASS_PTR__(Class)

#define __DECLARE_CLASS_PTR__(Class) \
typedef Class* Class##_Ptr; \
typedef const Class* Class##_CPtr; \
typedef Class& Class##_Ref; \
typedef const Class& Class##_CRef; \
typedef std::shared_ptr<Class> Class##_SPtr; \
typedef std::shared_ptr<const Class> Class##_SCPtr; \
typedef std::weak_ptr<Class> Class##_WPtr; \
typedef std::weak_ptr<const Class> Class##_WCPtr; \
typedef std::unique_ptr<Class> Class##_UPtr; \
typedef std::unique_ptr<const Class> Class##_UCPtr;

#define Shared_Ptr(Class, Ptr) std::shared_ptr<Class>(Ptr)
#define Weak_Ptr(Class) std::weak_ptr<Class>

#define Make_Shared(Class) std::make_shared<Class>()
#define Make_Weak(Class, sharedptr) std::weak_ptr<Class>(sharedptr)

#define StaticCast_WeakPtr(ToClass, wptr) std::static_pointer_cast<ToClass>(wptr.lock())
#define StaticCast_SharedPtr(ToClass, sptr) std::static_pointer_cast<ToClass>(sptr)

#define DynamicCast_WeakPtr(ToClass, wptr) std::dynamic_pointer_cast<ToClass>(wptr.lock())
#define DynamicCast_SharedPtr(ToClass, sptr) std::dynamic_pointer_cast<ToClass>(sptr)

// Type Cast
//#define StaticCast(Class, Value) static_cast<Class>(Value)
//#define DynamicCast(Class, Value) dynamic_cast<Class>(Value)

#define ToInt(A) static_cast<int>(A)


// Assert
#define ASSERT(expr) if (!(expr)) {  }

