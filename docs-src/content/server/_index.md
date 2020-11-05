---
title: "Server Setup"
weight: 100
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

    This will output the name of the image (e.g. `diatheke-demo-en_us-16`).

4. Start the diatheke service

    ```bash
    docker run -p 9002:9002 --name cobalt diathekesvr-demo-en_us-16
    ```

    That will start listening for grpc commands on port 9002. You can replace
    `--name cobalt` with any name you want to give the docker container.
    The name simply provides a way to refer back to the running container.


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
  - Cubic: /data/configs/cubicsvr.cfg.toml
  - Luna: /data/configs/lunasvr.cfg.toml
  - Diatheke: /data/configs/diathekesvr.cfg.toml
- **Model Directories**
  - Cubic: /data/models/cubic
  - Luna: /data/models/luna
  - Diatheke: /data/models/diatheke

