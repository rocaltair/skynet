#ifndef  _ALGO_H_P318U3IS_
#define  _ALGO_H_P318U3IS_

#if defined (__cplusplus)
extern "C" {
#endif

/**
 * binary floor search
 */
void * bfsearch(const void * key, 
	       const void * base,
	       size_t nel,
	       size_t width,
	       int (*compar) (const void *, const void *));

/**
 * binary ceil search
 */
void * bcsearch(const void * key, 
	       const void * base,
	       size_t nel,
	       size_t width,
	       int (*compar) (const void *, const void *));

#if defined (__cplusplus)
}	/*end of extern "C"*/
#endif

#endif /* end of include guard:  _ALGO_H_P318U3IS_ */

