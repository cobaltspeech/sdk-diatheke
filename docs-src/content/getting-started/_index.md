---
title: "Getting Started"
weight: 1
---


## Installing the Diatheke Server Image

The SDK communicates with a Diatheke Server instance using 
[gRPC](https://grpc.io). Cobalt distributes a docker image that contains 
the `diathekesvr` binary and model files.

<!--more-->

1. Contact Cobalt to get a link to the image file in AWS S3.  This link
   will expire in two weeks, so be sure to download the file to your own server.

2. Download with the AWS CLI if you have it, or with curl:

    ```bash
    URL="the url sent by Cobalt"
    IMAGE_NAME="name you want to give the file (should end with the same extension as the url, usually bz2)"
    curl $URL -L -o $IMAGE_NAME
    ```

3. Load the docker image

    ```bash
    docker load < $IMAGE_NAME
    ```

    This will output the name of the image (e.g. diatheke-demo-en_us-16).

4. Start the diatheke service

    ```bash
    docker run -p 9002:9002 --name cobalt diathekesvr-demo-en_us-16
    ```

    That will start listening for grpc commands on port 9002.  (You can
    replace `--name cobalt` with whatever name you want.  That just provides
    a way to refer back to the currently running container.)


### Contents of the docker image

- **Base docker image** : ubuntu:18.04
- **Additional dependencies**
  - supervisor
  - sox
  - libsndfile1
  - language-pack-en
- **Servers**
  - Cubic ([ASR](../glossary/#asr))
  - Luna ([TTS](../glossary/#tts))
  - Diatheke (Dialog)
- **Config Files**
  - Cubic configuration at /data/configs/cubicsvr.cfg.toml
  - Luna configuration at /data/configs/lunasvr.cfg.toml
  - Diatheke configuration at /data/configs/diathekesvr.cfg.toml
- **Models**
  - Cubic model at /data/models/cubic
  - Luna model at /data/models/luna
  - Diatheke model at /data/models/diatheke


## Installing the Diatheke SDK
To install the Diatheke SDK, use git to clone the
[sdk-diatheke repository](https://github.com/cobaltspeech/sdk-diatheke):
```bash
git clone https://github.com/cobaltspeech/sdk-diatheke.git
```

### Examples
The sdk-diatheke repo contains [example code](https://github.com/cobaltspeech/sdk-diatheke/tree/master/examples)
with subdirectories for various languages to build demo client applications. Simply follow the README instructions for each language to build and run the
sample applications.
