#include <stdio.h>
#include "algo.h"

/**
 * binary floor search
 */
void * bfsearch(const void * key, 
	       const void * base,
	       size_t nel,
	       size_t width,
	       int (*compar) (const void *, const void *))
{
	int l, r, oldl, oldr;
	int m = 0;
	void *pitem;
	l = 0;
	r = nel - 1;
	oldl = l;
	oldr = r;
	while(l<=r) {
		m=(l+r)/2;
		pitem = (void *)(((char *)base) + width * m);
		int ret = compar(key, pitem);
		if ( ret > 0) {
			l = m + 1;
		} else if(ret < 0) {
			r = m - 1;
		} else {
			return pitem;
		}
	}
	
	pitem = (void *)(((char *)base) + width * m);
	int ret = compar(pitem, key);
	if ( ret > 0 && m - 1 >=0 ) {
		pitem = (void *)(((char *)base) + width * (m - 1));
		return pitem;
	}
	if (ret > 0 && m <= 0) {
		return NULL;
	}
	
	return pitem;
}

/**
 * binary ceil search
 */
void * bcsearch(const void * key, 
	       const void * base,
	       size_t nel,
	       size_t width,
	       int (*compar) (const void *, const void *))
{
	int l, r, oldl, oldr;
	int m = 0;
	void *pitem;
	l = 0;
	r = nel - 1;
	oldl = l;
	oldr = r;
	while(l<=r) {
		m=(l+r)/2;
		pitem = (void *)(((char *)base) + width * m);
		int ret = compar(key, pitem);
		if ( ret > 0) {
			l = m + 1;
		} else if(ret < 0) {
			r = m - 1;
		} else {
			return pitem;
		}
	}
	
	pitem = (void *)(((char *)base) + width * m);
	int ret = compar(pitem, key);
	if ( ret < 0 && m + 1 <= oldr ) {
		pitem = (void *)(((char *)base) + width * (m + 1));
		return pitem;
	}
	if (ret < 0 && m >= oldr) {
		return NULL;
	}
	
	return pitem;
}

#if 0
void * bsearch(const void *key, const void *base0, 
	       size_t nmemb, size_t size,
	       int (*compar)(const void *, const void *)) 
{
        const char *base = base0;
        size_t lim;
        int cmp;
        const void *p; 

	assert(key != NULL);
	assert(base0 != NULL);
	assert(compar != NULL);
	assert(nmemb > 0);
	assert(size > 0);

        for (lim = nmemb; lim != 0; lim >>= 1) {
                p = base + (lim >> 1) * size;
                cmp = (*compar)(key, p); 
                if (cmp == 0)
                        return __UNCONST(p);
                if (cmp > 0) {  /* key > p: move right */
                        base = (const char *)p + size;
                        lim--;
                }               /* else move left */
        }   
        return (NULL);
}

int comp(const void *a, const void *b)
{
	return *((int *)a) - *((int *)b);
}

int main(int argc, const char *argv[])
{
	int a[] = {1,3,4,6,9,10,12,18};
	int size = sizeof(a)/sizeof(a[0]);
	int j;
	for (j=a[0]-1; j<=a[size-1]+1; j++) {
		void *p = bfsearch(&j, &a, size, sizeof(int), comp);
		if (p != NULL ) {
			printf("search=%d, value=%d\n", j, *((int*)p));
		} else {
			printf("search=%d, not found\n", j);
		}
	}
	return 0;
}
#endif
