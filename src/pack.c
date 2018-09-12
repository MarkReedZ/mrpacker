
#include "py_defines.h"
#include <stdio.h>

typedef struct _encoder
{
  unsigned char *start, *end, *s;
  int depth;  
  int indent;
  int skipKeys;
  int ensure_ascii;
  int sortKeys;
} Encoder;

//static void print_buffer( char* b, int len ) {
  //for ( int z = 0; z < len; z++ ) {
    //printf( "%02x ",(int)b[z]);
  //}
  //printf("\n");
//}

static int SetError(const char *message)
{
  PyErr_Format (PyExc_ValueError, "%s", message);
  return 0;
}
static int resizeBuffer(Encoder *e, size_t len)
{
  size_t curSize = e->end - e->start;
  size_t newSize = curSize * 2;
  size_t offset = e->s - e->start;

  while (newSize < curSize + len) newSize *= 2;

  e->start = (unsigned char *) realloc (e->start, newSize);
  if (e->start == NULL)
  {
    SetError ("Could not reserve memory block");
    printf("resizeBuffer failed\n");
    return 0;
  }

  e->s = e->start + offset;
  e->end = e->start + newSize;
  return 1;
}

#define resizeBufferIfNeeded(__enc, __len) \
    if ( (size_t) ((__enc)->end - (__enc)->s) < (size_t) (__len))  { resizeBuffer((__enc), (__len)); }

static inline void reverse(char* begin, char* end)
{
  char t;
  while (end > begin) {
    t = *end;
    *end-- = *begin;
    *begin++ = t;
  }
}

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

// First 3 bits:  000 not used, 001 map, 010 array, 011 various, 100 string, 110 tiny int

int encode( PyObject *o, Encoder *e ) {
  resizeBufferIfNeeded(e,2048);

  // TODO Would reordering speed this up?
  if ( o == Py_None ) {
    *(e->s++) = 0x60;
  }
  else if ( PyBool_Check(o) ) {
    if ( o == Py_True ) {
      *(e->s++) = 0x61;
    } else {
      *(e->s++) = 0x62;
    }
  }
  else if ( PyLong_Check(o) ) {
    int overflow;
    long long i = PyLong_AsLongLongAndOverflow(o, &overflow);
    //if (i == -1 && PyErr_Occurred()) return 0;
    if (overflow == 0) {

      if ( i >= 0 && i < 32 ) {
        *(e->s++) = 0xC0 | i;
      } else if ( i < 0xFFFFFFFF ) {
        *(e->s++) = 0x68;
        uint32_t *p = (uint32_t*)(e->s);
        *p = i;
        e->s += 4;
      } else if ( i < 0 && i > -0xFFFFFFFF ) {
        *(e->s++) = 0x67;
        uint32_t *p = (uint32_t*)(e->s);
        *p = i;
        e->s += 4;
      } else { 
        *(e->s++) = 0x64;
        long long *p = (long long *)(e->s);
        *p = i;
        e->s += 8;
      }

    } else {
      *(e->s++) = 0x65;
      unsigned long long ui = PyLong_AsUnsignedLongLong(o);
      unsigned long long *p = (unsigned long long *)(e->s);
      *p = ui;
      e->s += 8;
      //if (PyErr_Occurred()) return 0;
    }
  }
#if PY_MAJOR_VERSION < 3
  else if ( PyInt_Check(o) ) {
    char *s = e->s;
    int overflow;
    long i = PyInt_AS_LONG(o);
  }
#endif
  else if (PyUnicode_Check(o)) {
    Py_ssize_t l;
#if PY_MAJOR_VERSION >= 3
    const char* s = PyUnicode_AsUTF8AndSize(o, &l);
#else
    const char* s = PyUnicode_AsUTF8String(o);
    l = PyUnicode_GET_SIZE(o);
#endif

    if (s == NULL) return 0; //ERR

    if ( l < 32 ) {
      *(e->s++) = 0x80 | l;
    } else {
      resizeBufferIfNeeded(e,l); // TODO error if buf not allocated
      *(e->s++) = 0x66;
      uint32_t *p32 = (uint32_t*)(e->s);
      *p32 = l;
      e->s += 4;
    }
    memcpy(e->s, s, l);
    e->s += l;

    //if ( l <= 0 || l > UINT_MAX ) {
      //if ( l != 0 ) {
        //PyErr_SetString(PyExc_TypeError, "Bad string length");
        //return 0;
      //}
    //} else {
    //}
  }
#if PY_MAJOR_VERSION < 3
  else if (PyString_Check(o)) {
    Py_ssize_t l;
    const char* s = PyString_AS_STRING(o);
    l = PyString_GET_SIZE(o);
    if (s == NULL) return 0; //ERR

    if ( l < 32 ) {
      *(e->s++) = 0x80 | l;
    } else {
      resizeBufferIfNeeded(e,l); // TODO error if buf not allocated
      *(e->s++) = 0x66;
      uint32_t *p32 = (uint32_t*)(e->s);
      *p32 = l;
      e->s += 4;
    }
    memcpy(e->s, s, l);
    e->s += l;

  }
#endif
  else if (PyList_Check(o)) {
    Py_ssize_t size = PyList_GET_SIZE(o);

    if ( size < 32 ) {
      *(e->s++) = 0x40 | size;
    } else {
      *(e->s++) = 0x6A;
      uint32_t *p32 = (uint32_t*)(e->s);
      *p32 = size;
      e->s += 4;
    }

    e->depth += 1;
    for (Py_ssize_t i = 0; i < size; i++) {
      if (Py_EnterRecursiveCall(" while packing list object")) return 0;
      PyObject* item = PyList_GET_ITEM(o, i);
      int r = encode(item, e);
      Py_LeaveRecursiveCall();
      if (!r) return 0;
    }
    e->depth -= 1;

  }
  else if (PyTuple_Check(o)) {
    Py_ssize_t size = PyTuple_GET_SIZE(o);
    if ( size < 32 ) {
      *(e->s++) = 0x40 | size;
    } else {
      *(e->s++) = 0x6A;
      uint32_t *p32 = (uint32_t*)(e->s);
      *p32 = size;
      e->s += 4;
    }

    e->depth += 1;
    for (Py_ssize_t i = 0; i < size; i++) {
      if (Py_EnterRecursiveCall(" while packing tuple object")) return 0;
      PyObject* item = PyTuple_GET_ITEM(o, i);
      int r = encode(item, e);
      Py_LeaveRecursiveCall();
      if (!r) return 0;
    }
    e->depth -= 1;
  }
  else if (PyDict_Check(o)) {
    Py_ssize_t size = PyDict_Size(o);

    if ( size < 32 ) {
      *(e->s++) = 0x20 | size;
    } else {
      *(e->s++) = 0x69;
      uint32_t *p32 = (uint32_t*)(e->s);
      *p32 = size;
      e->s += 4;
    }

    Py_ssize_t pos = 0;
    PyObject* key;
    PyObject* item;
    //int r;
    while (PyDict_Next(o, &pos, &key, &item)) {
      encode(key, e);
      encode(item, e);
    }
  }
  else if ( PyFloat_Check(o) ) {
    union { double d; uint64_t i; } mem;
    mem.d = PyFloat_AsDouble(o);
    //if (mem.d == -1.0 && PyErr_Occurred()) return 0;
    *(e->s++) = 0x63;
#if defined(__arm__) && !(__ARM_EABI__) // arm-oabi
    mem.i = (mem.i & 0xFFFFFFFFUL) << 32UL | (mem.i >> 32UL);
#endif
    uint64_t *p64 = (uint64_t*)(e->s);
    *p64 = mem.i;
    e->s += 8;
  }
  else {
    return 0;
  }
  return 1;
}

int do_encode(PyObject *o, Encoder *enc ) {
  int len = 65536;
  unsigned char *s = (unsigned char *) malloc (len);
  if (!s) {
    SetError("Could not reserve memory block");
    return 0;
  }

  enc->start = s;
  enc->end   = s + len;
  enc->s = s;
  return encode (o, enc);

}


PyObject* pack(PyObject* self, PyObject *args, PyObject *kwargs) {
  static char *kwlist[] = { "obj", "ensure_ascii", "sort_keys", "indent", "skipkeys", "output_bytes", NULL };

  PyObject *oinput = NULL;
  PyObject *oensureAscii = NULL;
  //PyObject *oencodeHTMLChars = NULL;
  //PyObject *oescapeForwardSlashes = NULL;
  PyObject *osortKeys = NULL;
  PyObject* oindent = NULL;
  PyObject* oskipkeys = NULL;
  PyObject* obytes = NULL;

  Encoder enc = { NULL,NULL,NULL,0,0,0,0,0 };

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OOOOO", kwlist, &oinput, &oensureAscii, &osortKeys, &oindent, &oskipkeys, &obytes)) return NULL;

  if (oensureAscii != NULL && !PyObject_IsTrue(oensureAscii)) enc.ensure_ascii = 0;
  if (osortKeys != NULL && PyObject_IsTrue(osortKeys)) enc.sortKeys = 1;
  if (oskipkeys != NULL && PyObject_IsTrue(oskipkeys)) enc.skipKeys = 1;
  if (oindent == NULL ) {
    enc.indent = -1;
  } else {
    enc.indent = PyLong_AsLong(oindent);
  }

  int r = do_encode( oinput, &enc );
 
  if ( r != 0 ) {
    //printf(" len %d\n", enc.s - enc.start );
    //print_buffer( enc.start , enc.s - enc.start );
    PyObject *ret = PyBytes_FromStringAndSize(enc.start, enc.s-enc.start);
    free(enc.start);
    return ret;
  } 
  free(enc.start);
  return NULL;
}

