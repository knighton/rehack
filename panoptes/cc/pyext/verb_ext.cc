#include <Python.h>

#include "cc/base/warning.h"

namespace {

char VERB_EXT_DOC[] =
    "Verb handling in C++ for performance reasons.\n";

char CONJUGATE_DOC[] =
    "(lemma, verb field index) -> conjugated verb word or None on error.\n"
    "\n"
    "Lemma must be alpha.  Returns None on invalid lemma or invalid field\n"
    "index.\n"
    "\n"
    "Verb field index is\n"
    "* 0 lemma\n"
    "* 1 present participle\n"
    "* 2 past participle\n"
    "* 3-8 nonpast: 1st person singular, 2s, 3s, 1st plural, 2p, 3p\n"
    "* 9-14 past: 1st person singular, 2s, 3s, 1st plural, 2p, 3p\n";

PyObject* conjugate(PyObject* self, PyObject* args) {
    UNUSED(self);
    UNUSED(args);
    return NULL;  // XXX
}

char LEMMATIZE_DOC[] =
    "conjugated verb word -> list of (lemma, verb field index).\n"
    "\n"
    "Verb field index is\n"
    "* 0 lemma\n"
    "* 1 present participle\n"
    "* 2 past participle\n"
    "* 3-8 nonpast: 1st person singular, 2s, 3s, 1st plural, 2p, 3p\n"
    "* 9-14 past: 1st person singular, 2s, 3s, 1st plural, 2p, 3p\n";

PyObject* lemmatize(PyObject* self, PyObject* args) {
    UNUSED(self);
    UNUSED(args);
    return NULL;  // XXX
}

char SAY_DOC[] =
    "VerbWithContext as dict -> (pre words, main words) or None.\n";

PyObject* say(PyObject* self, PyObject* args) {
    UNUSED(self);
    UNUSED(args);
    return NULL;  // XXX
}

char IS_VALID_DOC[] =
    "VerbWithContext -> bool.\n";

PyObject* is_valid(PyObject* self, PyObject* args) {
    UNUSED(self);
    UNUSED(args);
    return NULL;  // XXX
}

char PARSE_DOC[] =
    "(pre words, main words) -> list of VerbWithContext.\n";

PyObject* parse(PyObject* self, PyObject* args) {
    UNUSED(self);
    UNUSED(args);
    return NULL;  // XXX
}

PyMethodDef VERB_EXT_METHODS[] = {
    {"conjugate", conjugate, METH_VARARGS, CONJUGATE_DOC},
    {"lemmatize", lemmatize, METH_VARARGS, LEMMATIZE_DOC},
    {"say", say, METH_VARARGS, SAY_DOC},
    {"is_valid", is_valid, METH_VARARGS, IS_VALID_DOC},
    {"parse", parse, METH_VARARGS, PARSE_DOC},
};

}  // namespace

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-prototypes"
PyMODINIT_FUNC initverb_ext(void) {
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
    Py_InitModule3("verb_ext", VERB_EXT_METHODS, VERB_EXT_DOC);
#pragma clang diagnostic pop
}
