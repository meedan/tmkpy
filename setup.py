import sys
import subprocess
import os

from setuptools import setup, Extension
from setuptools.command.build_py import build_py


class Build(build_py):
	"""Customized setuptools build command - builds protos on build."""
	def run(self):
		os.chdir("tmk/cpp")
		command = ["make","libtmk.a"]
		if subprocess.call(command) != 0:
			sys.exit(-1)
		os.chdir("../../")
		build_py.run(self)


tmkpy_module = Extension('_tmkpy',
	sources=["tmkpy.i","tmkpy.cpp"],
	swig_opts=["-c++"],
	include_dirs=["."],
	libraries=["tmk"],
	library_dirs=["tmk/cpp/"],
 	extra_compile_args=["-std=c++14"]
)


setup(name='tmkpy',
	cmdclass={
		'build_py': Build,
	},
	version='0.1.1',
	description='Python bindings for the TMK video similarity library',
	long_description=open('README.md').read(),
	long_description_content_type="text/markdown",
	author='Scott Hale',
	author_email='scott@meedan.com',
	url='https://github.com/meedan/tmkpy',
	ext_modules=[tmkpy_module],
	py_modules=["tmkpy"],
	setup_requires=['wheel'],
	package_data = {'': ['tmkpy.h']}  # needed for sdist
)
