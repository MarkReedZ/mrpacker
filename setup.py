try:
  from setuptools import setup, Extension
except ImportError:
  from distutils.core import setup, Extension
import os.path
import re
import sys, codecs, platform

with codecs.open('README.md', encoding='utf-8') as f:
    README = f.read()

with codecs.open('version.txt', encoding='utf-8') as f:
    VERSION = f.read().strip()

link_args = ['-lstdc++', '-lm']
compile_args = ['-D_GNU_SOURCE','-O3','-std=c99']
if platform.system() == 'Windows':
  compile_args = ['-D_GNU_SOURCE']
  link_args = []

module1 = Extension(
    'mrpacker',
     sources = [
         './src/mrpacker.c',
         './src/pack.c',
         './src/unpack.c'
     ],
     include_dirs = ['./src'],
     extra_compile_args = compile_args,
     extra_link_args = link_args,
     define_macros = [('MRPACKER_VERSION', VERSION),
                      ('_CRT_SECURE_NO_WARNINGS', 1)]
)

setup(
    name = 'mrpacker',
    version = VERSION,
    license="MIT License",
    description = "Binary object packer for python",
    keywords='mrpacker',
    long_description = README,
    long_description_content_type='text/markdown',
    ext_modules = [module1],
    author="Mark Reed",
    author_email="MarkReedZ@mail.com",
    download_url="https://github.com/MarkReedZ/mrpacker/archive/v1.6.tar.gz",
    platforms=['any'],
    url="https://github.com/MarkReedZ/mrpacker",
    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: C',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python',
    ],
)
