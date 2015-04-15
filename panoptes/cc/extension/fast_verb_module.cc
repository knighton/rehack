#include <Python.h>

char FAST_VERB_DOC[] =
    "Verb handling in C++ for performance reasons.\n";

bool init_fast_verb() {
    return true;
}

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
    return NULL;  // XXX
}

char SAY_DOC[] =
    "VerbWithContext as dict -> (pre words, main words) or None.\n";

PyObject* say(PyObject* self, PyObject* args) {
    return NULL;  // XXX
}

char IS_VALID_DOC[] =
    "VerbWithContext -> bool.\n";

PyObject* is_valid(PyObject* self, PyObject* args) {
    return NULL;  // XXX
}

char PARSE_DOC[] =
    "(pre words, main words) -> list of VerbWithContext.\n";

PyObject* parse(PyObject* self, PyObject* args) {
    return NULL;  // XXX
}

PyMethodDef FAST_VERB_METHODS[] = {
    {"conjugate", conjugate, METH_VARARGS, CONJUGATE_DOC},
    {"lemmatize", lemmatize, METH_VARARGS, LEMMATIZE_DOC},
    {"say", say, METH_VARARGS, SAY_DOC},
    {"is_valid", is_valid, METH_VARARGS, IS_VALID_DOC},
    {"parse", parse, METH_VARARGS, PARSE_DOC},
};

PyMODINIT_FUNC initfast_verb(void) {
    if (!init_fast_verb()) {
        return;
    }

    Py_InitModule3("fast_verb", FAST_VERB_METHODS, FAST_VERB_DOC);
}
