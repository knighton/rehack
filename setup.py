from distutils.core import setup, Extension
import os


def find_cc_files(root_dir):
    ff = []
    for root, dirs, files in os.walk(root_dir):
        for name in files:
            if name.endswith('.cc'):
                f = os.path.join(root, name)
                ff.append(f)
    return ff


os.environ['CC'] = 'clang'
os.environ['CXX'] = 'clang++'


verb_ext = Extension(
    'verb_ext', find_cc_files('panoptes/cc'),
    include_dirs=['panoptes/']
)


setup(
    ext_modules=[verb_ext],
    entry_points={
        'console_scripts': [
            'panoptes=panoptes.main:main'
        ],
    },
)
