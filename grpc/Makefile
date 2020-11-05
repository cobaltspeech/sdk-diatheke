# Copyright (2019) Cobalt Speech and Language Inc.

.PHONY: all go py go-test py-test gendoc

all: go py swift gendoc

ifeq ($(PROTOINC),)
$(error "env variable PROTOINC must be defined to the location of googleapis proto files")
endif

GO_OUTDIR=go-diatheke/diathekepb
PY_OUTDIR=py-diatheke/diatheke
DOCS_OUTDIR=../docs-src/v2/content/protobuf
SWIFTDIR=../deps/swift/bin

go: ${GO_OUTDIR}/diatheke.pb.go
py: $(PY_OUTDIR)/diatheke_pb2.py ${PY_OUTDIR}/diatheke_pb2_grpc.py py-test

swift:
	protoc -I . -I ${PROTOINC} \
		--plugin=$$(which protoc-gen-grpc-swift) \
		--grpc-swift_out=Client=true,Server=false,Visibility=Public:./swift-diatheke \
		--plugin=$$(which protoc-gen-swift) \
		--swift_opt=Visibility=Public \
		--swift_out=./swift-diatheke \
		diatheke.proto

java:
	cd java-groovy-diatheke && gradle wrapper && ./gradlew :androidlib:compileJava

${GO_OUTDIR}/diatheke.pb.go: diatheke.proto
	mkdir -p ${GO_OUTDIR}
	protoc -I . -I ${PROTOINC} --go_out=plugins=grpc:"${GO_OUTDIR}" $<

${PY_OUTDIR}/diatheke_pb2.py: diatheke.proto
	mkdir -p ${PY_OUTDIR}
	python -m grpc_tools.protoc -I . -I ${PROTOINC} --python_out=${PY_OUTDIR} $<

${PY_OUTDIR}/diatheke_pb2_grpc.py: diatheke.proto
	mkdir -p ${PY_OUTDIR}
	python -m grpc_tools.protoc -I . -I ${PROTOINC} --grpc_python_out=${PY_OUTDIR} $<

go-test:
	cd go-diatheke && go test
	cd go-diatheke/diathekepb/gw && go test

py-test:
	# cd py-diatheke/diatheke && python test_client.py
	# cd py-diatheke/diatheke && python test_tls.py

gendoc:
	protoc \
		-I . \
		-I ${PROTOINC} \
		--doc_out=${DOCS_OUTDIR} \
		--doc_opt=doc.md.tmpl,${DOCS_OUTDIR}/_index.md \
		diatheke.proto

	# Remove any TODO items from the generated doc
	sed -i 's/TODO.*//g' ${DOCS_OUTDIR}/_index.md
