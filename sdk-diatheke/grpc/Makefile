# Copyright (2019) Cobalt Speech and Language Inc.

.PHONY: all go py go-test py-test docs

all: go py docs

ifeq ($(PROTOINC),)
$(error "env variable PROTOINC must be defined to the location of googleapis proto files")
endif

GO_OUTDIR=go-diatheke/diathekepb
PY_OUTDIR=py-diatheke/diatheke

go: ${GO_OUTDIR}/diatheke.pb.go
py: $(PY_OUTDIR)/diatheke_pb2.py ${PY_OUTDIR}/diatheke_pb2_grpc.py py-test

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
	cd py-diatheke/diatheke && python test_client.py
	cd py-diatheke/diatheke && python test_tls.py

.PHONY: gendoc
gendoc:
	protoc \
		-I . \
		-I ${PROTOINC} \
		--doc_out=./ \
		--doc_opt=doc.md.tmpl,autogen-doc-cubic-proto.md \
		diatheke.proto
