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

#ifndef DIATHEKE_COMMAND_STATUS_H
#define DIATHEKE_COMMAND_STATUS_H

#include "diatheke.pb.h"

#include <map>
#include <string>

namespace Diatheke
{

class CommandStatus
{
public:
    using ParamMap = std::map<std::string, std::string>;

    enum StatusCode
    {
        SUCCESS = 0,
        FAILURE
    };

    // Create a new CommandStatus object from the original CommandEvent.
    CommandStatus(const cobaltspeech::diatheke::CommandEvent &command);

    ~CommandStatus();

    // Return the commandID
    std::string commandID() const;

    // Return the command's state id. Most users will not need to
    // use this.
    std::string stateID() const;

    /*
     * Return the status code. If setStatusCode() has not been called,
     * this defaults to FAILURE.
     */
    StatusCode statusCode() const;

    // Return the command's output parameters
    const ParamMap &outputParams() const;

    // Return the error message.
    std::string errorMessage() const;

    // Set the status code for the command.
    void setStatusCode(StatusCode code);

    // Set the error message for the command.
    void setError(const std::string &errorMessage);

    // Replace the output parameters with the given set of parameters.
    void setOutputParameters(const ParamMap &params);

    /*
     * Set the value of the specified output parameter to val. This
     * method will automatically convert the integer to a string. Provided
     * for convenience.
     */
    void setIntParam(const std::string &key, int val);

    /*
     * Set the value of the specified output parameter to val. This
     * method will automatically convert the double to a string. Provided
     * for convenience
     */
    void setDoubleParam(const std::string &key, double val);

    // Set the specified output parameter to val.
    void setStringParam(const std::string &key, const std::string &val);

private:
    std::string mCommandID;
    std::string mStateID;
    StatusCode mCode;
    std::string mErrorMessage;
    ParamMap mParams;
};

} // namespace Diatheke

#endif // DIATHEKE_COMMAND_STATUS_H
