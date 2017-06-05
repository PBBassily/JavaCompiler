
/*
  Macros for fixed point arithmatic operations
  Macros are used as they are much faster in execution
*/

// using 17.14 format
#define f (1<<14)

// fxd fxd operations
#define fxd_add(x,y) (x + y)
#define fxd_sub(x,y) (x - y)
#define fxd_mul(x,y) ((int)(((int64_t) x) * y / f))
#define fxd_div(x,y) ((int)(((int64_t) x) * f / y))

// fxd int operations
#define fxd_int_mul(x,n)  (x * n)
#define fxd_int_div(x,n)  (x / n)
#define fxd_int_add(x,n)	(x + n * f)
#define fxd_int_sub(x,n)  (x - n * f)

// fxd to int (nearest to zero)
#define nearest_int_pos(x) ((x + (f / 2)) / f)
#define nearest_int_neg(x) ((x - (f / 2)) / f)

// fxd to int (nearest int)
#define cnvrt_n_fxd( n )	(n * f)
#define cnvrt_fxd_n( x )	(x / f)
