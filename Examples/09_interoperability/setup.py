
# very similar to the setup.py used to build g3logPython

import setuptools
import os

__version__ = '1.0.0'


class get_pybind_include(object):
    """Helper class to determine the pybind11 include path
    The purpose of this class is to postpone importing pybind11
    until it is actually installed, so that the ``get_include()``
    method can be invoked. """

    def __init__(self, user=False):
        self.user = user

    def __str__(self):
        import pybind11
        return pybind11.get_include(self.user)


# https://docs.python.org/3/distutils/setupscript.html#describing-extension-modules
ext_modules = [
    setuptools.Extension(
        'cppP3Dcalls',
        ['cppP3Dcalls/lib.cpp'],
        include_dirs=[
            # Path to pybind11 headers
            get_pybind_include(),
            get_pybind_include(user=True),
            '/usr/local/lib',
            '/usr/include/panda3d',
        ],
        library_dirs=[
            '/usr/lib64/panda3d/',
            '/usr/local/lib64',
            '/usr/local/lib',
            ],
        # list of installed files: rpm -qlp ./panda3d/*.rpm
        # https://docs.panda3d.org/1.10/cpp/programming/using-cpp/compiling/linux#how-to-compile-a-c-panda3d-program-on-linux
        libraries=['stdc++',
                   'g3log',
                   'g3logrotate',
                   'g3syslog',
                   'g3logBindings',
                   'p3direct',
                   'p3dtool',
                   'p3dtoolconfig',
                   'p3framework',
                   'p3interrogatedb',
                   'panda',
                   'pandaexpress',
                   ],
        extra_compile_args=["-std=c++17", "-v","-fPIC"],
        language='c++'
    ),
]

setuptools.setup(
    name='cppP3Dcalls',
    version=__version__,
    author='Joel Stienlet',
    author_email='',
    url='https://github.com/JoelStienlet/g3logPython',
    description='demo N°5',
    long_description="demo mixing python and c++, showing usage of g3logPython in a mixed language project",
    packages=setuptools.find_packages(),
    ext_modules=ext_modules,
    install_requires=['pybind11>=2.2'],
    setup_requires=['pybind11>=2.2'],
    zip_safe=False,
)


