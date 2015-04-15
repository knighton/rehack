from distutils.core import setup, Extension


fast_verb = Extension(
    'fast_verb', [
        'panoptes/cc/extension/fast_verb_module.cc',
    ]
)

setup(
    ext_modules=[fast_verb],
)
