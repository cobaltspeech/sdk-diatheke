---
title: "Connecting to the Server"
description: "Describes how to connect to a running Diatheke server instance."
weight: 23
---

Once you have the Diatheke server [up and running](../../getting-started),
you are ready to create a client connection.

<!--more-->

First, you need to know the address (`host`:`port`) where the server is
running. This document will assume the values `127.0.0.1:9002`, but
these can be replaced with your server address in actual code.

## Default Connection

The following code snippet connects to the server and queries its version.  It uses our recommended 
default setup, expecting the server to be listening on a TLS encrypted connection.

{{< tabs >}}

{{< tab "Go" "go" >}}
package main

import (
	"context"
	"fmt"
	"log"

	"github.com/cobaltspeech/sdk-diatheke/grpc/go-diatheke"
)

const serverAddr = "127.0.0.1:9002"

func main() {
	client, err := diatheke.NewClient(serverAddr)
	if err != nil {
		log.Fatal(err)
	}

	// Be sure to close the client when we are done with it.
	defer client.Close()

	version, err := client.DiathekeVersion(context.Background())
	if err != nil {
		log.Fatal(err)
	}

	fmt.Println(version)
}
{{< /tab >}}

{{< tab "C++" "c++" >}}
// File - main.cpp

#include "diatheke_client.h"

#include <iostream>
#include <string>

const char* serverAddr = "127.0.0.1:9002";

int main(int argc, char *argv[])
{
	Diatheke::Client client(serverAddr);
	std::string version = client.diathekeVersion();
	std::cout << "Diatheke version: " << version << std::endl;

	return 0;
}

{{< /tab >}}

{{< tab "Python" "python">}}
import diatheke

client = diatheke.Client(server_address="localhost:9002")
version = client.diatheke_version()
print("Diatheke version {}".format(version))
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
import Diatheke

class DiathekeConnection {
    
    let serverAddress = "localhost"
    let serverPort = 9002

    let client = Client(host: serverAddress, port: serverPort, useTLS: true)

}
{{< /tab >}}

{{< tab "Java/Android" "java" >}}
	import cobaltspeech.diatheke.DiathekeGrpc;
	String serverAddress = "localhost";
	int serverPort = 9002;
	ManagedChannelBuilder<?> builder = ManagedChannelBuilder
			.forAddress(serverAddress,serverPort);
	builder.usePlaintext();
	ManagedChannel mCubicChannel = builder.build();
	DiathekeGrpc.DiathekeStub mClient = DiathekeGrpc.newStub(mCubicChannel);
{{< /tab >}}

{{</tabs >}}


## Insecure Connection

It is sometimes required to connect to Diatheke server without TLS enabled 
(during debugging, for example). Note that if the server has TLS enabled,
attempting to connect with an insecure client will fail.

To create an insecure connection, do the following when creating the client:

{{< tabs >}}

{{< tab "Go" "go" >}}
client, err := diatheke.NewClient(serverAddr, diatheke.WithInsecure())
{{< /tab >}}

{{< tab "C++" "c++" >}}
Diatheke::Client client(serverAddr, true);
{{< /tab >}}

{{< tab "Python" "python">}}
client = diatheke.Client(server_addres="localhost:9002",
                         insecure=True)
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
let client = Client(host: serverAddress, port: serverPort, useTLS: false)
{{< /tab >}}

{{< tab "Java/Android" "java" >}}
ManagedChannel mCubicChannel = ManagedChannelBuilder.forAddress(serverAddress,serverPort).build();
DiathekeGrpc.DiathekeStub mClient = DiathekeGrpc.newStub(mCubicChannel);
{{< /tab >}}

{{</tabs >}}

## Client Authentication

In our recommended default setup, TLS is enabled in the gRPC setup, and when
connecting to the server, clients validate the server's SSL certificate to make
sure they are talking to the right party.  This is similar to how "https"
connections work in web browsers.

In some setups, it may be desired that the server should also validate clients
connecting to it and only respond to the ones it can verify. If your Diatheke
server is configured to do client authentication, you will need to present the
appropriate certificate and key when connecting to it.

Please note that in the client-authentication mode, the client will still also
verify the server's certificate, and therefore this setup uses mutually
authenticated TLS. This can be done with:

{{< tabs >}}

{{< tab "Go" "go" >}}
// certPem and keyPem are the bytes of the client certificate and key
// provided to you.
client, err := diatheke.NewClient(serverAddr, diatheke.WithClientCert(certPem, keyPem))
{{< /tab >}}

{{< tab "C++" "c++" >}}

// Currently unsupported in C++.

{{< /tab >}}

{{< tab "Python" "python">}}
# cert_pem and key_pem are the contents of the client certificate and key
# provided to you.
client = diatheke.Client(server_address="localhost:9002",
                         client_certificate=cert_pem,
                         client_key=key_pem)
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
import Cubic
import NIOSSL

class DiathekeConnection {
    
    let serverAddress = "localhost"
    let serverPort = 9002

    let client = Client(host: serverAddress, port: serverPort, tlsCertificateFileName: "root", tlsCertificateFormat: .pem)

}
{{< /tab >}}

{{< tab "Java/Android" "java" >}}

ManagedChannelBuilder<?> builder = ManagedChannelBuilder
	.forAddress(serverAddress,serverPort)
	.useTransportSecurity(certChainFile, privateKeyFile)
ManagedChannel mCubicChannel = builder.build();
DiathekeGrpc.DiathekeStub mClient = DiathekeGrpc.newStub(mCubicChannel);
{{< /tab >}}

{{</tabs >}}