module github.com/cobaltspeech/sdk-diatheke/examples/go

go 1.12

require (
	github.com/BurntSushi/toml v0.3.1
	github.com/c-bata/go-prompt v0.2.3
	github.com/cobaltspeech/sdk-diatheke/grpc/go-diatheke v0.0.0-20190830174431-8ed9fef788e6
	github.com/mattn/go-colorable v0.1.2 // indirect
	github.com/mattn/go-runewidth v0.0.4 // indirect
	github.com/mattn/go-tty v0.0.0-20190424173100-523744f04859 // indirect
	github.com/pkg/term v0.0.0-20190109203006-aa71e9d9e942 // indirect
	google.golang.org/grpc v1.23.0 // indirect
)

replace github.com/cobaltspeech/sdk-diatheke/grpc/go-diatheke => ../../grpc/go-diatheke
