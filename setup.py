import setuptools
import os

# see https://github.com/pybind/python_example/blob/master/setup.py

__version__ = '0.2.0'

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


def read_file(filename):
    with open(filename, "r") as fh:
        return fh.read()


# coverage option: selected with environment variable
# example: sudo BUILD_WITH_COVERAGE=1 python3 setup.py install
compile_args=["-std=c++17", "-v", "-fPIC"]
link_args=[]
env_var_name = "BUILD_WITH_COVERAGE"
cov_env_val = os.environ.get( env_var_name, None )
if cov_env_val is not None:
    print("BUILD_WITH_COVERAGE = ", cov_env_val)
    if cov_env_val == "1":
        print("code coverage enabled")
        compile_args.extend(["-O0", "-fprofile-arcs", "-ftest-coverage", "-fno-inline", "-fno-inline-small-functions", "-fno-default-inline"])
        link_args.extend(["-lgcov"])
else:
    print("no ",env_var_name," environment variable found")

# we have to add the paths were g3log's .so files were installed, so that the linker can find them.
# alternatively, one could add an option to the /usr/bin/env command in each script:
#  -S LD_LIBRARY_PATH="/usr/local/lib64;/usr/local/lib"
# but the later solution is less convenient.
link_args.extend(['-Wl,-rpath=/usr/local/lib','-Wl,-rpath=/usr/local/lib64'])




# https://docs.python.org/3/distutils/setupscript.html#describing-extension-modules
ext_modules = [
    setuptools.Extension(
        '_g3logPython',
        ['g3logPython/g3logPython.cpp'],
        include_dirs=[
            # Path to pybind11 headers
            get_pybind_include(),
            get_pybind_include(user=True),
            '/usr/local/include/',
        ],
        libraries=['stdc++','g3log','g3logrotate','g3syslog','g3logBindings'],
        extra_compile_args=compile_args,
        extra_link_args=link_args,
        language='c++'
    ),
]

# 
for sE in ext_modules:
    print('Extension ', sE.name)
    print('    libraries: ', sE.libraries)
    print('    extra link args: ', sE.extra_link_args)


setuptools.setup(
    name='g3logPython',
    version=__version__,
    author='Joel Stienlet',
    author_email='',
    url='https://github.com/JoelStienlet/g3logPython',
    description='python bindings for g3log',
    long_description=read_file("README.md"),
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



