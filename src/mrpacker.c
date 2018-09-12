
#include "py_defines.h"

#define STRINGIFY(x) XSTRINGIFY(x)
#define XSTRINGIFY(x) #x

PyObject* pack     (PyObject* self, PyObject *args, PyObject *kwargs);
//PyObject* toJsonFile (PyObject* self, PyObject *args, PyObject *kwargs);
//PyObject* toJsonBytes(PyObject* self, PyObject *args, PyObject *kwargs);

PyObject* unpack     (PyObject* self, PyObject *args, PyObject *kwargs);
//PyObject* fromJsonFile (PyObject* self, PyObject *args, PyObject *kwargs);
//PyObject* fromJsonBytes(PyObject* self, PyObject *args, PyObject *kwargs);


static PyMethodDef mrpackerMethods[] = {
  {"pack",   (PyCFunction) pack,     METH_VARARGS | METH_KEYWORDS, "Pack an object"   },
  {"unpack", (PyCFunction) unpack,   METH_VARARGS | METH_KEYWORDS, "Unpack an object" },
  {NULL, NULL, 0, NULL}       /* Sentinel */
};

#if PY_MAJOR_VERSION >= 3

static struct PyModuleDef moduledef = {
  PyModuleDef_HEAD_INIT,
  "mrpacker",
  0,              /* m_doc */
  -1,             /* m_size */
  mrpackerMethods,  /* m_methods */
  NULL,           /* m_reload */
  NULL,           /* m_traverse */
  NULL,           /* m_clear */
  NULL            /* m_free */
};

#define PYMODINITFUNC       PyObject *PyInit_mrpacker(void)
#define PYMODULE_CREATE()   PyModule_Create(&moduledef)
#define MODINITERROR        return NULL

#else

#define PYMODINITFUNC       PyMODINIT_FUNC initmrpacker(void)
#define PYMODULE_CREATE()   Py_InitModule("mrpacker", mrpackerMethods)
#define MODINITERROR        return

#endif

PYMODINITFUNC
{
  PyObject *m;

  m = PYMODULE_CREATE();

  if (m == NULL) { MODINITERROR; }

  PyModule_AddStringConstant(m, "__version__", STRINGIFY(MRPACKER_VERSION));
  PyModule_AddStringConstant(m, "__author__", "Mark Reed <mark@untilfluent.com>");
#if PY_MAJOR_VERSION >= 3
  return m;
#endif
}

