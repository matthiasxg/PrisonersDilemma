# uncomment as necessary
project('PrisonersDilemma', 'cpp',
        license : ['Boost'],
        version : '0.1',
        default_options : ['cpp_std=c++17', 'warning_level=3', 'werror=true'])

thread_dep = dependency('threads')

# FS lib
add_project_link_arguments(['-lstdc++fs', '-lssl', '-lcrypto'], language : 'cpp')

# # begin asio: https://think-async.com/Asio/
add_global_arguments('-I' + get_option('asio_include_dir'), language : 'cpp')
add_global_arguments('-DASIO_STANDALONE', language : 'cpp')
add_global_arguments('-DASIO_NO_DEPRECATED', language : 'cpp')
# # end asio

# clipp: https://github.com/muellan/clipp/
add_global_arguments('-I' + get_option('clipp_include_dir'), language : 'cpp')

# spdlog: https://github.com/gabime/spdlog
add_global_arguments('-I' + get_option('spdlog_include_dir'), language : 'cpp')

# json: https://github.com/nlohmann/json
add_global_arguments('-I' + get_option('json_include_dir'), language : 'cpp')

# begin protobuf: https://developers.google.com/protocol-buffers/
# protobuf: must be installed independently and has to be found...
protoc = find_program('protoc', required : true)
grpc_cpp_plugin = find_program('grpc_cpp_plugin', required : true)
protobuf_dep = dependency('protobuf', required : true)
grpc_dep = dependency('grpc', required : true)
grpcpp_dep = dependency('grpc++', required : true)

gen = generator(protoc, \
  output    : ['@BASENAME@.pb.cc', '@BASENAME@.pb.h', '@BASENAME@.grpc.pb.cc', '@BASENAME@.grpc.pb.h',
               '@BASENAME@_pb2.py', '@BASENAME@_pb2_grpc.py'],
  arguments : ['--proto_path=@SOURCE_DIR@/src/static',
               '--plugin=protoc-gen-grpc=' + grpc_cpp_plugin.path(),
        #        '--plugin=protoc-gen-grpc=' + grpc_python_plugin.path(),
               '--grpc_out=@BUILD_DIR@',
               '--cpp_out=@BUILD_DIR@',
               '../src/static/@PLAINNAME@'])

# change proto file accordingly
proto = gen.process('src/static/PrisonersDilemma.proto')
# end protobuf

inc_dir = include_directories('include')
server_src = [proto, 'src/server/main.cpp', 'src/server/server.cpp', 'src/static/player.cpp']
client_src = [proto, 'src/client/main.cpp', 'src/client/client.cpp', 
              'src/static/player.cpp', 'src/client/strategy.cpp']

executable('server',
           sources : server_src,
           include_directories : inc_dir,
           dependencies : [thread_dep, protobuf_dep, grpc_dep, grpcpp_dep]
          )

executable('client',
           sources : client_src,
           include_directories : inc_dir,
           dependencies : [thread_dep, protobuf_dep, grpc_dep, grpcpp_dep]
          )