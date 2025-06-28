

#include "unpack.h"
#include "py_defines.h"
#include "common_strings.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DBG if(0) 

#define MAX_DEPTH 64

// String interning using shared common strings system
static PyObject **interned_pyobjects_by_index = NULL; // Pre-cached PyObjects indexed by common string id
static int interning_initialized = 0;

#ifdef _MSC_VER

#ifdef _M_IX86

inline uint64_t rdtsc()
{
  uint64_t c;
  __asm {
    cpuid       // serialize processor
    rdtsc       // read time stamp counter
    mov dword ptr [c + 0], eax
    mov dword ptr [c + 4], edx
  }
  return c;
}

#elif defined(_M_X64)
#include <intrin.h>

#pragma intrinsic(__rdtsc)
inline uint64_t rdtsc()
{
  return __rdtsc();
}

#endif

#else
static __inline__ unsigned long long rdtsc(void)
{ 
  unsigned long lo, hi;
  __asm__ volatile( "rdtsc" : "=a" (lo), "=d" (hi) );
  return( lo | ( hi << 32 ) );
}

#endif

static char errmsg[256];
static PyObject* SetErrorInt(const char *message, int pos)
{
  char pstr[32];
  sprintf( pstr, "%d", pos );
  strcpy( errmsg, message );
  strcat( errmsg, pstr );
  PyErr_Format (PyExc_ValueError, "%s", errmsg);
  return NULL;
}

// Initialize string interning cache
void init_string_interning(void)
{
  if (interning_initialized) return;
  
  // Initialize shared common strings lookup
  init_common_strings_lookup();
  
  // Allocate PyObject array for all common strings
  interned_pyobjects_by_index = calloc(total_common_strings, sizeof(PyObject*));
  if (interned_pyobjects_by_index == NULL) {
    PyErr_NoMemory();
    return;
  }
  
  // Pre-create interned PyObjects for all common strings
  for (int i = 0; i < total_common_strings; i++) {
    interned_pyobjects_by_index[i] = PyUnicode_InternFromString(all_common_strings[i]);
    if (interned_pyobjects_by_index[i] == NULL) {
      PyErr_Clear();
      interned_pyobjects_by_index[i] = NULL;
    }
  }
  
  interning_initialized = 1;
}

// Get common string by index (super fast O(1) lookup!)
static inline PyObject* get_common_string_by_index(int index)
{
  if (index >= 0 && index < total_common_strings && interned_pyobjects_by_index[index] != NULL) {
    Py_INCREF(interned_pyobjects_by_index[index]);
    return interned_pyobjects_by_index[index];
  }
  return NULL;
}

// Look up a string in the interning cache (for non-common strings)
static inline PyObject* lookup_interned_string(const char *str, Py_ssize_t len)
{
  // For non-common strings, still try to find in common strings as fallback
  int index = find_common_string_index(str, len);
  if (index >= 0) {
    return get_common_string_by_index(index);
  }
  
  return NULL; // Not found in cache
}

// Cleanup interned strings (call on module cleanup)
void cleanup_string_interning(void)
{
  if (interned_pyobjects_by_index != NULL) {
    for (int i = 0; i < total_common_strings; i++) {
      if (interned_pyobjects_by_index[i] != NULL) {
        Py_DECREF(interned_pyobjects_by_index[i]);
      }
    }
    free(interned_pyobjects_by_index);
    interned_pyobjects_by_index = NULL;
  }
  
  if (all_common_strings != NULL) {
    free((void*)all_common_strings);
    all_common_strings = NULL;
  }
  
  interning_initialized = 0;
}

// First 3 bits:  000 not used, 001 map, 010 array, 011 various, 100 string, 110 tiny int
// 0x60 null
// 0x61 true
// 0x62 false
// 0x63 double
// 0x64 64 bit long
// 0x65 64 bit unsigned long
// 0x66 string 32 bit length
// 0x67 32 bit long
// 0x68 32 bit unsigned long
// 0x69 dict 32 bit length
// 0x6A list 32 bit length
// 0x6C common string index (1 byte)


PyObject *decode( char *s, char *end) {
  PyObject *parents[MAX_DEPTH];
  PyObject *keys[MAX_DEPTH];
  int curlen[MAX_DEPTH];
  int maxlen[MAX_DEPTH];
  int types[MAX_DEPTH];
  
  int depth = -1;
  PyObject *o;

  while( s < end ) {
    DBG printf(" codepoint %02x\n",*s);
  if ( *s == 0x60 ) { s += 1; Py_INCREF(Py_None); o = Py_None; }
  else if ( (*(s) & 0xE0) == 0x80 ) {  // String
    int l = *(s) & 0x1F; 
    DBG printf(" string len %d\n",l);
    s += 1;
    o = lookup_interned_string(s, l);
    if (o == NULL) {
      o = PyUnicode_FromStringAndSize( s, l );
    }
    s += l;
  }
  else if ( *(s) == 0x66 ) { 
    s++;
    uint32_t *p = (uint32_t*)s;
    uint32_t l = *p;
    DBG printf(" long string len %d\n",l);
    s+=4;
    o = lookup_interned_string(s, l);
    if (o == NULL) {
      o = PyUnicode_FromStringAndSize( s, l );
    }
    s += l;
  }
  else if ( *(s) == 0x6B ) {
    s++;
    uint32_t *p = (uint32_t*)s;
    uint32_t l = *p;
    s+=4;
    o = PyBytes_FromStringAndSize( s, l );
    s += l;
  }
  else if ( *(s) == 0x6C ) {
    s++;
    unsigned char index = *(s);
    s++;
    Py_INCREF(interned_pyobjects_by_index[index]);
    o = interned_pyobjects_by_index[index];
    //o = get_common_string_by_index(index);
    //if (o == NULL) {
      //PyErr_Format(PyExc_ValueError, "Invalid common string index: %d", index);
      //return NULL;
    //}
  }
  else if ( *(s) == 0x61 ) { s += 1; Py_INCREF(Py_True);  o = Py_True; }
  else if ( *(s) == 0x62 ) { s += 1; Py_INCREF(Py_False); o = Py_False; }
  else if ( *(s) == 0x63 ) { 
    s++;
    union { double d; uint64_t i; } mem;
    uint64_t *p = (uint64_t*)s;
    mem.i = *p;
    s += 8;
    o = PyFloat_FromDouble(mem.d);
  }
  else if ( *(s) == 0x64 ) { 
    s++;
    long long *p = (long long*)s;
    s += 8;
    o = PyLong_FromLongLong(*p);
  }
  else if ( *(s) == 0x65 ) { 
    s++;
    uint64_t *p = (uint64_t*)s;
    s += 8;
    o = PyLong_FromUnsignedLongLong(*p);
  }
  else if ( *(s) == 0x68 ) { 
    s++;
    uint32_t *p = (uint32_t*)s;
    s += 4;
    o = PyLong_FromUnsignedLong(*p);
  }
  else if ( *(s) == 0x67 ) { 
    s++;
    int32_t *p = (int32_t*)s;
    s += 4;
    o = PyLong_FromLong(*p);
  }
  else if ( (*(s) & 0xE0) == 0xC0 ) {  // tiny int
    int i = *(s) & 0x1F; 
    s += 1;
    o = PyLong_FromLong(i);
  }
  else if ( (*(s) & 0xE0) == 0x40 ) {  // List
    int l = *(s) & 0x1F; 
    s += 1;
    if ( l == 0 ) o = PyList_New(0);
    else {
      depth += 1;
      if (depth == MAX_DEPTH) return SetErrorInt("Too many nested objects 0x40, the max depth is ", MAX_DEPTH);
      DBG printf("depth %d is list\n",depth);
      parents[depth] = PyList_New(l);
      curlen[depth] = 0;
      maxlen[depth] = l;
      types[depth] = 1;
      continue;
    }
  }
  else if ( *(s) == 0x6A ) {  // List
    s++;
    uint32_t *p = (uint32_t*)s;
    uint32_t l = *p;
    //printf("6A list of len %d\n", l);
    s+=4;
    depth += 1;
    if (depth == MAX_DEPTH) return SetErrorInt("Too many nested objects 0x6A, the max depth is ", MAX_DEPTH);
    DBG printf("depth %d is list\n",depth);
    parents[depth] = PyList_New(l);
    curlen[depth] = 0;
    maxlen[depth] = l;
    types[depth] = 1;
    continue;
  }
  else if ( (*(s) & 0xE0) == 0x20 ) {  // dict
    int l = *(s) & 0x1F; 
    s += 1;
    if ( l > 0 ) {
      depth += 1;
      if (depth == MAX_DEPTH) return SetErrorInt("Too many nested objects 0x20, the max depth is ", MAX_DEPTH);
      DBG printf("depth %d is dict len %d\n",depth, l);
      parents[depth] = _PyDict_NewPresized(l);
      curlen[depth] = 0;
      maxlen[depth] = l;
      types[depth] = 2;
      keys[depth] = NULL;
      continue;
    } else {
      o = PyDict_New();
    }
  }
  else if ( *(s) == 0x69 ) {  // Dict
    s++;
    uint32_t *p = (uint32_t*)s;
    uint32_t l = *p;
    s+=4;
    if ( l > 0 ) {
      depth += 1;
      if (depth == MAX_DEPTH) return SetErrorInt("Too many nested objects 0x69, the max depth is ", MAX_DEPTH);
      DBG printf("depth %d is dict\n",depth);
      parents[depth] = _PyDict_NewPresized(l);
      curlen[depth] = 0;
      maxlen[depth] = l;
      types[depth] = 2;
      keys[depth] = NULL;
      continue;
    } else {
      o = PyDict_New();
    }
  } else {
    PyErr_Format(PyExc_ValueError, "Parser error");
    return NULL;
  }
  //PyObject_Print(o, stdout, 0); printf("\n");

end:
  if ( depth == -1 ) return o;
  if ( types[depth] == 1 ) {
    PyList_SetItem( parents[depth],curlen[depth],o );
    curlen[depth] += 1;
    if ( curlen[depth] == maxlen[depth] ) {
      o = parents[depth];
      depth -= 1;
      goto end;
    }
  } else {
    if ( keys[depth] == NULL ) {
      keys[depth] = o;
    } else {
      PyDict_SetItem( parents[depth], keys[depth], o );
      Py_DECREF(keys[depth]); Py_DECREF(o);
      keys[depth] = NULL;
      curlen[depth] += 1;
      if ( curlen[depth] == maxlen[depth] ) {
        o = parents[depth];
        depth -= 1;
        goto end;
      }
    }
  } 
  } 
  return NULL;
}

PyObject* unpack(PyObject* self, PyObject *args, PyObject *kwargs)
{
  static char *kwlist[] = {"obj", NULL};
  PyObject *arg;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &arg)) return NULL;

  if (! PyBytes_Check(arg))
  {
    PyErr_Format(PyExc_TypeError, "Expected bytes");
    return NULL;
  }

  char *p;
  Py_ssize_t l;

  if(PyBytes_AsStringAndSize(arg, &p, &l) == -1) {
    PyErr_Format(PyExc_TypeError, "Failed to convert byte object to c string. Out of memory?");
    return NULL;
  }

  // Initialize string interning on first call (removed from hot lookup path)
  if (!interning_initialized) {
    init_string_interning();
  }

  //unsigned long long cycles = rdtsc();
  //PyObject *ret = decode( p, p+l );
  //printf(" took %lld\n", rdtsc() - cycles);
  //return ret;
  return decode( p, p+l );

}

