# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: diatheke.proto
"""Generated protocol buffer code."""
from google.protobuf.internal import builder as _builder
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\x0e\x64iatheke.proto\x12\x15\x63obaltspeech.diatheke\"P\n\x0fVersionResponse\x12\x10\n\x08\x64iatheke\x18\x01 \x01(\t\x12\x0e\n\x06\x63hosun\x18\x02 \x01(\t\x12\r\n\x05\x63ubic\x18\x03 \x01(\t\x12\x0c\n\x04luna\x18\x04 \x01(\t\"F\n\x12ListModelsResponse\x12\x30\n\x06models\x18\x01 \x03(\x0b\x32 .cobaltspeech.diatheke.ModelInfo\"i\n\tModelInfo\x12\n\n\x02id\x18\x01 \x01(\t\x12\x0c\n\x04name\x18\x02 \x01(\t\x12\x10\n\x08language\x18\x03 \x01(\t\x12\x17\n\x0f\x61sr_sample_rate\x18\x04 \x01(\r\x12\x17\n\x0ftts_sample_rate\x18\x05 \x01(\r\"\x92\x02\n\x0cSessionInput\x12/\n\x05token\x18\x01 \x01(\x0b\x32 .cobaltspeech.diatheke.TokenData\x12\x30\n\x04text\x18\x02 \x01(\x0b\x32 .cobaltspeech.diatheke.TextInputH\x00\x12/\n\x03\x61sr\x18\x03 \x01(\x0b\x32 .cobaltspeech.diatheke.ASRResultH\x00\x12\x33\n\x03\x63md\x18\x04 \x01(\x0b\x32$.cobaltspeech.diatheke.CommandResultH\x00\x12\x30\n\x05story\x18\x05 \x01(\x0b\x32\x1f.cobaltspeech.diatheke.SetStoryH\x00\x42\x07\n\x05input\"7\n\tTokenData\x12\x0c\n\x04\x64\x61ta\x18\x01 \x01(\x0c\x12\n\n\x02id\x18\x02 \x01(\t\x12\x10\n\x08metadata\x18\x03 \x01(\t\"2\n\x0cSessionStart\x12\x10\n\x08model_id\x18\x01 \x01(\t\x12\x10\n\x08wakeword\x18\x02 \x01(\t\"\x19\n\tTextInput\x12\x0c\n\x04text\x18\x01 \x01(\t\"\xb1\x01\n\rCommandResult\x12\n\n\x02id\x18\x01 \x01(\t\x12O\n\x0eout_parameters\x18\x02 \x03(\x0b\x32\x37.cobaltspeech.diatheke.CommandResult.OutParametersEntry\x12\r\n\x05\x65rror\x18\x03 \x01(\t\x1a\x34\n\x12OutParametersEntry\x12\x0b\n\x03key\x18\x01 \x01(\t\x12\r\n\x05value\x18\x02 \x01(\t:\x02\x38\x01\"\x94\x01\n\x08SetStory\x12\x10\n\x08story_id\x18\x01 \x01(\t\x12\x43\n\nparameters\x18\x02 \x03(\x0b\x32/.cobaltspeech.diatheke.SetStory.ParametersEntry\x1a\x31\n\x0fParametersEntry\x12\x0b\n\x03key\x18\x01 \x01(\t\x12\r\n\x05value\x18\x02 \x01(\t:\x02\x38\x01\"x\n\rSessionOutput\x12/\n\x05token\x18\x01 \x01(\x0b\x32 .cobaltspeech.diatheke.TokenData\x12\x36\n\x0b\x61\x63tion_list\x18\x02 \x03(\x0b\x32!.cobaltspeech.diatheke.ActionData\"\xfe\x01\n\nActionData\x12\x39\n\x05input\x18\x01 \x01(\x0b\x32(.cobaltspeech.diatheke.WaitForUserActionH\x00\x12\x37\n\x07\x63ommand\x18\x02 \x01(\x0b\x32$.cobaltspeech.diatheke.CommandActionH\x00\x12\x33\n\x05reply\x18\x03 \x01(\x0b\x32\".cobaltspeech.diatheke.ReplyActionH\x00\x12=\n\ntranscribe\x18\x04 \x01(\x0b\x32\'.cobaltspeech.diatheke.TranscribeActionH\x00\x42\x08\n\x06\x61\x63tion\"B\n\x11WaitForUserAction\x12\x1a\n\x12requires_wake_word\x18\x01 \x01(\x08\x12\x11\n\timmediate\x18\x02 \x01(\x08\"\xa8\x01\n\rCommandAction\x12\n\n\x02id\x18\x01 \x01(\t\x12S\n\x10input_parameters\x18\x02 \x03(\x0b\x32\x39.cobaltspeech.diatheke.CommandAction.InputParametersEntry\x1a\x36\n\x14InputParametersEntry\x12\x0b\n\x03key\x18\x01 \x01(\t\x12\r\n\x05value\x18\x02 \x01(\t:\x02\x38\x01\"/\n\x0bReplyAction\x12\x0c\n\x04text\x18\x01 \x01(\t\x12\x12\n\nluna_model\x18\x02 \x01(\t\"Q\n\x10TranscribeAction\x12\n\n\x02id\x18\x01 \x01(\t\x12\x16\n\x0e\x63ubic_model_id\x18\x02 \x01(\t\x12\x19\n\x11\x64iatheke_model_id\x18\x03 \x01(\t\"V\n\x08\x41SRInput\x12\x31\n\x05token\x18\x01 \x01(\x0b\x32 .cobaltspeech.diatheke.TokenDataH\x00\x12\x0f\n\x05\x61udio\x18\x02 \x01(\x0cH\x00\x42\x06\n\x04\x64\x61ta\"?\n\tASRResult\x12\x0c\n\x04text\x18\x01 \x01(\t\x12\x12\n\nconfidence\x18\x02 \x01(\x01\x12\x10\n\x08timedOut\x18\x03 \x01(\x08\"\x19\n\x08TTSAudio\x12\r\n\x05\x61udio\x18\x01 \x01(\x0c\"e\n\x0fTranscribeInput\x12\x39\n\x06\x61\x63tion\x18\x01 \x01(\x0b\x32\'.cobaltspeech.diatheke.TranscribeActionH\x00\x12\x0f\n\x05\x61udio\x18\x02 \x01(\x0cH\x00\x42\x06\n\x04\x64\x61ta\"H\n\x10TranscribeResult\x12\x0c\n\x04text\x18\x01 \x01(\t\x12\x12\n\nconfidence\x18\x02 \x01(\x01\x12\x12\n\nis_partial\x18\x03 \x01(\x08\"\x07\n\x05\x45mpty2\xd4\x05\n\x08\x44iatheke\x12Q\n\x07Version\x12\x1c.cobaltspeech.diatheke.Empty\x1a&.cobaltspeech.diatheke.VersionResponse\"\x00\x12W\n\nListModels\x12\x1c.cobaltspeech.diatheke.Empty\x1a).cobaltspeech.diatheke.ListModelsResponse\"\x00\x12\\\n\rCreateSession\x12#.cobaltspeech.diatheke.SessionStart\x1a$.cobaltspeech.diatheke.SessionOutput\"\x00\x12Q\n\rDeleteSession\x12 .cobaltspeech.diatheke.TokenData\x1a\x1c.cobaltspeech.diatheke.Empty\"\x00\x12\\\n\rUpdateSession\x12#.cobaltspeech.diatheke.SessionInput\x1a$.cobaltspeech.diatheke.SessionOutput\"\x00\x12R\n\tStreamASR\x12\x1f.cobaltspeech.diatheke.ASRInput\x1a .cobaltspeech.diatheke.ASRResult\"\x00(\x01\x12T\n\tStreamTTS\x12\".cobaltspeech.diatheke.ReplyAction\x1a\x1f.cobaltspeech.diatheke.TTSAudio\"\x00\x30\x01\x12\x63\n\nTranscribe\x12&.cobaltspeech.diatheke.TranscribeInput\x1a\'.cobaltspeech.diatheke.TranscribeResult\"\x00(\x01\x30\x01\x42)\n\x19\x63om.cobaltspeech.diathekeZ\x0c.;diathekepbb\x06proto3')

_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, globals())
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'diatheke_pb2', globals())
if _descriptor._USE_C_DESCRIPTORS == False:

  DESCRIPTOR._options = None
  DESCRIPTOR._serialized_options = b'\n\031com.cobaltspeech.diathekeZ\014.;diathekepb'
  _COMMANDRESULT_OUTPARAMETERSENTRY._options = None
  _COMMANDRESULT_OUTPARAMETERSENTRY._serialized_options = b'8\001'
  _SETSTORY_PARAMETERSENTRY._options = None
  _SETSTORY_PARAMETERSENTRY._serialized_options = b'8\001'
  _COMMANDACTION_INPUTPARAMETERSENTRY._options = None
  _COMMANDACTION_INPUTPARAMETERSENTRY._serialized_options = b'8\001'
  _VERSIONRESPONSE._serialized_start=41
  _VERSIONRESPONSE._serialized_end=121
  _LISTMODELSRESPONSE._serialized_start=123
  _LISTMODELSRESPONSE._serialized_end=193
  _MODELINFO._serialized_start=195
  _MODELINFO._serialized_end=300
  _SESSIONINPUT._serialized_start=303
  _SESSIONINPUT._serialized_end=577
  _TOKENDATA._serialized_start=579
  _TOKENDATA._serialized_end=634
  _SESSIONSTART._serialized_start=636
  _SESSIONSTART._serialized_end=686
  _TEXTINPUT._serialized_start=688
  _TEXTINPUT._serialized_end=713
  _COMMANDRESULT._serialized_start=716
  _COMMANDRESULT._serialized_end=893
  _COMMANDRESULT_OUTPARAMETERSENTRY._serialized_start=841
  _COMMANDRESULT_OUTPARAMETERSENTRY._serialized_end=893
  _SETSTORY._serialized_start=896
  _SETSTORY._serialized_end=1044
  _SETSTORY_PARAMETERSENTRY._serialized_start=995
  _SETSTORY_PARAMETERSENTRY._serialized_end=1044
  _SESSIONOUTPUT._serialized_start=1046
  _SESSIONOUTPUT._serialized_end=1166
  _ACTIONDATA._serialized_start=1169
  _ACTIONDATA._serialized_end=1423
  _WAITFORUSERACTION._serialized_start=1425
  _WAITFORUSERACTION._serialized_end=1491
  _COMMANDACTION._serialized_start=1494
  _COMMANDACTION._serialized_end=1662
  _COMMANDACTION_INPUTPARAMETERSENTRY._serialized_start=1608
  _COMMANDACTION_INPUTPARAMETERSENTRY._serialized_end=1662
  _REPLYACTION._serialized_start=1664
  _REPLYACTION._serialized_end=1711
  _TRANSCRIBEACTION._serialized_start=1713
  _TRANSCRIBEACTION._serialized_end=1794
  _ASRINPUT._serialized_start=1796
  _ASRINPUT._serialized_end=1882
  _ASRRESULT._serialized_start=1884
  _ASRRESULT._serialized_end=1947
  _TTSAUDIO._serialized_start=1949
  _TTSAUDIO._serialized_end=1974
  _TRANSCRIBEINPUT._serialized_start=1976
  _TRANSCRIBEINPUT._serialized_end=2077
  _TRANSCRIBERESULT._serialized_start=2079
  _TRANSCRIBERESULT._serialized_end=2151
  _EMPTY._serialized_start=2153
  _EMPTY._serialized_end=2160
  _DIATHEKE._serialized_start=2163
  _DIATHEKE._serialized_end=2887
# @@protoc_insertion_point(module_scope)
