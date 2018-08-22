import os
import shutil
import subprocess
import sys

from setuptools import setup, find_packages
from setuptools import Extension
from setuptools.command.build_ext import build_ext

# Based on https://github.com/pybind/python_example


class BuildExtensionCommand(build_ext):
    compiler_options = {
        'msvc': ['/EHsc'],
        'unix': [],
    }

    if sys.platform == 'darwin':
        compiler_options['unix'] += [
            '-stdlib=libc++',
            '-mmacosx-version-min=10.7',
        ]

    def build_extensions(self):
        compiler_type = self.compiler.compiler_type
        options = self.compiler_options.get(compiler_type, [])

        if compiler_type == 'unix':
            options.append('-DVERSION_INFO="{}"'.format(self.distribution.get_version()))
            options.append(self._cpp_flag())

            if self._has_flag('-fvisibility=hidden'):
                options.append('-fvisibility=hidden')
        elif compiler_type == 'msvc':
            options.append('/DVERSION_INFO=\\"%s\\"' % self.distribution.get_version())

        for extension in self.extensions:
            extension.extra_compile_args = options

        super().build_extensions()

    # As of Python 3.6, CCompiler has a `has_flag` method.
    # cf http://bugs.python.org/issue26689
    def _has_flag(self, flag):
        """Return a boolean indicating whether a flag name is supported on
        the specified compiler.
        """
        import tempfile
        with tempfile.NamedTemporaryFile('w', suffix='.cpp') as output_file:
            output_file.write('int main (int argc, char **argv) { return 0; }')
            try:
                self.compiler.compile([output_file.name], extra_postargs=[flag])
            except setuptools.distutils.errors.CompileError:
                return False

        return True

    def _cpp_flag(self):
        """Return the -std=c++[11/14] compiler flag.
        The c++14 is prefered over c++11 (when it is available).
        """
        if self._has_flag('-std=c++14'):
            return '-std=c++14'
        elif self._has_flag('-std=c++11'):
            return '-std=c++11'
        else:
            raise RuntimeError('Unsupported compiler -- at least C++11 support is needed!')


def lazy(func):
    class Decorator:
        def __init__(self, *args, **kwargs):
            self.args = args
            self.kwargs = kwargs

        def __str__(self):
            return func(*self.args, **self.kwargs)

        def __add__(self, other):
            return str(self) + other

        def __radd__(self, other):
            return other + str(self)

    return Decorator


@lazy
def get_pybind_include(user=False):
    import pybind11
    return pybind11.get_include(user)


setup(
    name='python-pcre2',
    version='1.0.0',
    url='https://github.com/hyeon0145/python-pcre2',
    author='Jonghwan Hyeon',
    author_email='hyeon0145@gmail.com',
    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Science/Research',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3 :: Only',
        'Topic :: Text Processing',
    ],
    zip_safe=False,
    install_requires=[
        'pybind11 ~= 2.0'
    ],
    python_requires='>=3',
    packages=find_packages(),
    ext_modules=[
        Extension(
            name='_pcre2',
            define_macros=[
                ('PCRE2_CODE_UNIT_WIDTH', '8'),
            ],
            sources=[
                'pcre2/pybind/_pcre2.cpp',
                'pcre2/pybind/pcre2cpp/pattern.cpp',
                'pcre2/pybind/pcre2cpp/match.cpp',
                'pcre2/pybind/pcre2cpp/matchcontext.cpp',
                'pcre2/pybind/pcre2cpp/utils.cpp',
            ],
            include_dirs=[
                get_pybind_include(),
                get_pybind_include(user=True),
            ],
            libraries=['pcre2-8'],
            language='c++',
        ),
    ],
    cmdclass={
        'build_ext': BuildExtensionCommand,
    },
)
