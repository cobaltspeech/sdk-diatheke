#!/usr/bin/env python
# Copyright (2019) Cobalt Speech and Language Inc.

from setuptools import setup

setup(
    name='cobalt-diatheke',
    python_requires='>=3.5.0',
    description='Client SDK library for Cobalt\'s Diatheke engine.',
    author='Cobalt Speech and Language Inc.',
    author_email='tech@cobaltspeech.com',
    url='https://github.com/cobaltspeech/sdk-diatheke',
    packages=["diatheke"],
    version_config={
        "version_format": "{tag}.dev{commits}+{sha}"
    },
    setup_requires=['better-setuptools-git-version'],
    install_requires=['grpcio-tools']
)
