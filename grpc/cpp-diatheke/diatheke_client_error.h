/*
 * Copyright (2020) Cobalt Speech and Language, Inc.
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

#ifndef DIATHEKE_CLIENT_ERROR_H
#define DIATHEKE_CLIENT_ERROR_H

#include <exception>
#include <grpcpp/grpcpp.h>
#include <string>

namespace Diatheke
{

// ClientError is thrown when there is an error running the Client.
class ClientError : public std::exception
{
public:
    ClientError(const std::string &msg);
    ClientError(const grpc::Status &status);
    ~ClientError() override;

    const char *what() const noexcept override;

private:
    std::string mMsg;
};

} // namespace Diatheke

#endif // DIATHEKE_CLIENT_ERROR_H
