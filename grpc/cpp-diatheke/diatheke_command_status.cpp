/*
 * Copyright (2019) Cobalt Speech and Language, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "diatheke_command_status.h"

namespace Diatheke
{

CommandStatus::CommandStatus(
    const cobaltspeech::diatheke::CommandEvent &command)
    : mCommandID(command.command_id()), mStateID(command.command_state_id()),
      mCode(StatusCode::FAILURE), mErrorMessage(""),
      mParams(command.parameters().begin(), command.parameters().end())
{
}

CommandStatus::~CommandStatus() {}

std::string CommandStatus::commandID() const { return mCommandID; }

std::string CommandStatus::stateID() const { return mStateID; }

CommandStatus::StatusCode CommandStatus::statusCode() const { return mCode; }

const CommandStatus::ParamMap &CommandStatus::params() const { return mParams; }

std::string CommandStatus::errorMessage() const { return mErrorMessage; }

void CommandStatus::setStatusCode(StatusCode code) { mCode = code; }

void CommandStatus::setError(const std::string &errorMessage)
{
    mErrorMessage = errorMessage;
}

void CommandStatus::setParameters(const ParamMap &params) { mParams = params; }

void CommandStatus::setIntParam(const std::string &key, int val)
{
    mParams[key] = std::to_string(val);
}

void CommandStatus::setDoubleParam(const std::string &key, double val)
{
    mParams[key] = std::to_string(val);
}

void CommandStatus::setStringParam(const std::string &key,
                                   const std::string &val)
{
    mParams[key] = val;
}

} // namespace Diatheke
