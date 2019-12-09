# -*- coding: utf-8 -*-
#
# Copyright(2019) Cobalt Speech and Language Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License")
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http: // www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


# First make sure the directory that contains this module code is added to
# the PYTHONPATH. This ensures that python will be able to find the
# protobuf/gRPC generated files for other imports.
import os
import sys
sys.path.append(os.path.join(os.path.dirname(os.path.realpath(__file__))))

from .client import Client
from .session import Session
from .diatheke_pb2 import CommandStatus
