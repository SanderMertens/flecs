
#if ECS_IMPL && defined _MSC_VER
#define ECS_EXPORT __declspec(dllexport)
#elif ECS_IMPL
#define ECS_EXPORT __attribute__((__visibility__("default")))
#elif defined _MSC_VER
#define ECS_EXPORT __declspec(dllimport)
#else
#define ECS_EXPORT
#endif
