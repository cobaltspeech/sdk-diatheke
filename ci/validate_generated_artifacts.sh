#!/bin/bash
# Copyright (2019) Cobalt Speech and Language Inc.

set -e

# install system dependencies (ci assumes golang:1.12 as the base image)
apt update && apt install -y python3 python-virtualenv unzip

# generate all artifacts again
make

# it's an error if we generated something that wasn't checked in.
git diff --name-only --exit-code || (echo "Not all generated files were checked in" && exit 1)
