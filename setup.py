from distutils.core import setup, Extension
import os


os.environ['CC'] = 'clang'
os.environ['CXX'] = 'clang++'


fast_verb = Extension(
    'fast_verb', [
        'panoptes/cc/extension/fast_verb_module.cc',
    ],
    include_dirs=['panoptes/'],
)

setup(
    ext_modules=[fast_verb],
    entry_points={
        'console_scripts': [
            'panoptes=panoptes.main:main'
        ],
    },
)
