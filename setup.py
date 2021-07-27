import sys
import subprocess
import os

from setuptools import setup, Extension
from setuptools.command.build_py import build_py


class Build(build_py):
	"""Customized setuptools build command - builds protos on build."""
	def run(self):
		os.chdir("tmk/cpp")
		command = ["make"]
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
)


setup(name='tmkpy',
	cmdclass={
		'build_py': Build,
	},
	version='0.1',
	description='Python bindings for the TMK video similarity library',
	author='Scott Hale',
	author_email='scott@meedan.com',
	url='https://github.com/meedan/ThreatExchange/tree/swig',
	ext_modules=[tmkpy_module],
	py_modules=["tmkpy"],
	setup_requires=['wheel'],
	package_data = {'': ['tmkpy.h']}  # needed for sdist
)
