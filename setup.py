import setuptools

# see https://github.com/pybind/python_example/blob/master/setup.py

__version__ = '0.1.0'

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


with open("readme.md", "r") as fh:
    long_description = fh.read()

# https://docs.python.org/3/distutils/setupscript.html#describing-extension-modules
ext_modules = [
    setuptools.Extension(
        '_g3logPython',
        ['g3logPython/g3logPython.cpp', 'g3logPython/sinks.cpp', 'g3logPython/worker.cpp', 'g3logPython/log.cpp'],
        include_dirs=[
            # Path to pybind11 headers
            get_pybind_include(),
            get_pybind_include(user=True),
            '/usr/local/lib',
        ],
        libraries=['stdc++','g3logger','g3logrotate','g3log_syslog'],
        extra_compile_args=["-std=c++14", "-v","-fPIC"],
        language='c++'
    ),
]

setuptools.setup(
    name='g3logPython',
    version=__version__,
    author='Joel Stienlet',
    author_email='',
    url='https://github.com/JoelStienlet/g3logPython',
    description='python bindings for g3log',
    long_description=long_description,
    packages=setuptools.find_packages(),
    ext_modules=ext_modules,
    python_requires='>=3.6',
    install_requires=['pybind11>=2.2'],
    setup_requires=['pybind11>=2.2'],
    zip_safe=False,
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: Unlicense",
        "Operating System :: Linux",
    ],
)



