---
title: "Connecting to the Server"
description: "Describes how to connect to a running Diatheke server instance."
weight: 400
---

Once you have the Diatheke server [up and running](../server/),
you are ready to create a client connection.

<!--more-->

First, you need to know the address (`host`:`port`) where the server is
running. This document will assume the values `127.0.0.1:9002`, but
these can be replaced with your server address in actual code.

## Default Connection

The following code snippet connects to the server and queries its version.
It uses our recommended default setup, expecting the server to be
listening on a TLS encrypted connection.

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
}
{{< /tab >}}

{{< tab "C++" "c++" >}}
#include <diatheke_client.h>
#include <iostream>

int main(int argc, char* argv[])
{
  std::string serverAddress = "127.0.0.1:9002";

  // Read or embed the digital certificate, and store
  // in rootCert. Unlike other languages, in C++ there
  // is nothing to do this automatically.
  grpc::string rootCert;

  // Set up SSL options for the secure connection.
  grpc::SslCredentialsOptions sslOpts;
  sslOpts.pem_root_certs = rootCert;

  // Create the client with ssl options
  Diatheke::Client client(serverAddress, sslOpts);

  return 0;
}
{{< /tab >}}

{{< tab "Python" "python">}}
import diatheke

client = diatheke.Client(server_address="localhost:9002")
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

public class DiathekeClient {
    private ManagedChannel mDiathekeChannel;
    private DiathekeGrpc.DiathekeStub mDiathekeService;
    private DiathekeGrpc.DiathekeBlockingStub mDiathekeBlockingService;

    public DiathekeClient() { }

    public void connect(String host, boolean isSecure) {
        ManagedChannelBuilder<?> builder = ManagedChannelBuilder
                .forTarget(host);
        if (!isSecure) {
            builder.usePlaintext();
        }

        mDiathekeChannel = builder.build();
        mDiathekeService = DiathekeGrpc.newStub(mDiathekeChannel);
        mDiathekeBlockingService = DiathekeGrpc.newBlockingStub(mDiathekeChannel);
    }

    public void disconnect() {
        if (mDiathekeChannel != null && !mDiathekeChannel.isShutdown()) {
            mDiathekeChannel.shutdownNow();
            mDiathekeChannel = null;
        }
        mDiathekeService = null;
        mDiathekeBlockingService = null;
    }
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
Diatheke::Client client(serverAddress);
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
std::string serverAddress = "127.0.0.1:9002";

// Read or embed the bytes of the client certificate and key
grpc::string certPem, keyPem;

// Set up the SSL options.
// See https://grpc.github.io/grpc/cpp/structgrpc_1_1_ssl_credentials_options.html
// for more details about the SSL options struct.
grpc::SslCredentialsOptions sslOpts;
sslOpts.pem_root_certs = certPem;
sslOpts.pem_private_key = keyPem;

Diatheke::Client client(serverAddress, sslOpts);
{{< /tab >}}

{{< tab "Python" "python">}}
# cert_pem and key_pem are the contents of the client certificate and key
# provided to you.
client = diatheke.Client(server_address="localhost:9002",
                         client_certificate=cert_pem,
                         client_key=key_pem)
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
import Diatheke

class DiathekeConnection {
    
    let serverAddress = "localhost"
    let serverPort = 9002
        
    let client = Client(host: serverAddress,
                        port: serverPort,
                        clientCertificatePath: "path/to/client_certificate.pem",
                        privateKeyPath: "path/to/private_key.pem")

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


## Server Information
The client provides two methods to get information about the server -
Version and ListModels.

### Version
The Version method provides information about the version of the Diatheke
server the client is connected to, as well as information about other
relevant services and packages the server uses.

{{< tabs >}}

{{< tab "Go" "go" >}}
// Request the server version info
ver, err := client.Version(context.Background())
fmt.Printf("Server Version\n")
fmt.Printf("  Diatheke: %v\n", ver.Diatheke)
fmt.Printf("  Chosun (NLU): %v\n", ver.Chosun)
fmt.Printf("  Cubic (ASR): %v\n", ver.Cubic)
fmt.Printf("  Luna (TTS): %v\n", ver.Luna)
{{< /tab >}}

{{< tab "Python" "python" >}}
# Request the server version info
ver = client.version()
print("Server Version")
print("  Diatheke:", ver.diatheke)
print("  Chosun (NLU):", ver.chosun)
print("  Cubic (ASR):", ver.cubic)
print("  Luna (TTS):", ver.luna)
{{< /tab >}}

{{< tab "C++" "c++" >}}
// Request the server version info
auto ver = client.version();
std::cout << "Server Version" << std::endl;
std::cout << "  Diatheke: " << ver.diatheke() << std::endl;
std::cout << "  Chosun (NLU): " << ver.chosun() << std::endl;
std::cout << "  Cubic (ASR): " << ver.cubic() << std::endl;
std::cout << "  Luna (TTS): " << ver.luna() << std::endl;
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
// Request the server version info
client.version { (response) in
	print("Server Version")
	print("Diatheke: \(response.diatheke)")
	print("Chosun (NLU): \(response.chosun)")
	print("Cubic (ASR): \(response.cubic)")
	print("Luna (TTS): \(response.luna)")
} failure: { (error) in
	print(error.localizedDescription)
}
{{< /tab >}}

{{< tab "Java/Android" "java" >}}
Empty e = Empty.newBuilder().build();
VersionResponse ver = mDiathekeBlockingService.version(e);
Log.i("Server Version");
Log.i("Diatheke: " + ver.getDiatheke());
Log.i("Chosun (NLU): " + ver.getChosun());
Log.i("Cubic (ASR): " + ver.getCubic());
Log.i("Luna (TTS): " + ver.getLuna());
{{< /tab >}}

{{< /tabs >}}

### List Models
The ListModels method fetches a list of models available to the Diatheke
server. On the server side, the models are specified as part of the
server's config file.

{{< tabs >}}

{{< tab "Go" "go" >}}
// Request the list of models
modelList, err := client.ListModels(bctx)
fmt.Printf("Available Models:\n")
for _, mdl := range modelList.Models {
	fmt.Printf("  ID: %v\n", mdl.Id)
	fmt.Printf("    Name: %v\n", mdl.Name)
	fmt.Printf("    Language: %v\n", mdl.Language)
	fmt.Printf("    ASR Sample Rate: %v\n", mdl.AsrSampleRate)
	fmt.Printf("    TTS Sample Rate: %v\n\n", mdl.TtsSampleRate)
}
{{< /tab >}}

{{< tab "Python" "python" >}}
# Request the list of models
model_list = client.list_models()
print("Available Models:")
for mdl in model_list:
    print("  ID:", mdl.id)
    print("    Name:", mdl.name)
    print("    Language:", mdl.language)
    print("    ASR Sample Rate:", mdl.asr_sample_rate)
    print("    TTS Sample Rate:", mdl.tts_sample_rate)
{{< /tab >}}

{{< tab "C++" "c++" >}}
// Request the list of available models
auto modelList = client.listModels();
std::cout << "\nAvailable Models:" << std::endl;
for (auto &mdl : modelList.models()) {
    std::cout << "  ID: " << mdl.id() << std::endl;
    std::cout << "    Name: " << mdl.name() << std::endl;
    std::cout << "    Language: " << mdl.language() << std::endl;
    std::cout << "    ASR Sample Rate: " << mdl.asr_sample_rate() << std::endl;
    std::cout << "    TTS Sample Rate: " << mdl.tts_sample_rate() << std::endl;
}
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
// Request the list of models
client.listModels { (models) in
	print("Available Models:")
	for model in models {
		print("ID: \(model.id)")
		print("Name: \(model.name)")
		print("Language: \(model.language)")
		print("ASR Sample Rate: \(model.asrSampleRate)")
		print("TTS Sample Rate: \(model.ttsSampleRate)")
	}
} failure: { (error) in
	print(error.localizedDescription)
}
{{< /tab >}}

{{< tab "Java/Android" "java" >}}
Empty e = Empty.newBuilder().build();
ListModelsResponse resp = mDiathekeBlockingService.listModels(e);
Log.i(TAG, "Available Models:");
for (ModelInfo model: resp.getModelsList()) {
    Log.i(TAG, "  ID:" + model.getId());
    Log.i(TAG, "    Name:" + model.getName());
    Log.i(TAG, "    Language:" + model.getLanguage());
    Log.i(TAG, "    ASR Sample Rate:" + model.getAsrSampleRate());
    Log.i(TAG, "    TTS Sample Rate:" + model.getTtsSampleRate());
}
{{< /tab >}}

{{< /tabs >}}
