#ifdef _MSC_VER
#define printerr(args...) fprintf(stderr, __VA_ARGS__)
#else
#define printerr(args...) fprintf(stderr, ##args)
#endif

#define panic(message)                                                         \
  printerr("%s\n", message);                                                   \
  *(int *)0 = 0;

#ifdef DEBUG
#define assert(condition, message)                                             \
  if (!(condition)) {                                                          \
    panic(message)                                                             \
  }
#else
#define assert(condition)
#endif

#define internal static
#define global static
#define local static

#define u32 uint32_t
#define u64 uint64_t
#define s32 int32_t
#define s64 int64_t
#define f32 float
#define f64 double

#define bool32 u32

