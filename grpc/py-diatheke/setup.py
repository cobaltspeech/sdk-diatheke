#!/usr/bin/env python
# Copyright (2020) Cobalt Speech and Language Inc.

from setuptools import setup

setup(
    name='cobalt-diatheke',
    python_requires='>=3.5.0',
    description='Client SDK library for Cobalt\'s Diatheke engine.',
    author='Cobalt Speech and Language Inc.',
    author_email='tech@cobaltspeech.com',
    url='https://github.com/cobaltspeech/sdk-diatheke',
    packages=["diatheke"],
    setuptools_git_versioning={
        "enabled": True,
    },
    setup_requires=["setuptools-git-versioning<2"],
    install_requires=['protobuf', 'grpcio']
)
