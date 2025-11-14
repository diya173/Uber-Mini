{
  "targets": [
    {
      "target_name": "uber_mini_native",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [
        "backend/cpp/node_binding.cpp",
        "backend/cpp/graph.cpp",
        "backend/cpp/dijkstra.cpp",
        "backend/cpp/min_heap.cpp",
        "backend/cpp/driver_manager.cpp",
        "backend/cpp/ride_matcher.cpp",
        "backend/cpp/city_graph_generator.cpp"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "backend/cpp/include"
      ],
      "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
      "msvs_settings": {
        "VCCLCompilerTool": {
          "ExceptionHandling": 1,
          "AdditionalOptions": [ "/std:c++17" ]
        }
      },
      "conditions": [
        ["OS=='win'", {
          "defines": [ "_HAS_EXCEPTIONS=1" ]
        }],
        ["OS!='win'", {
          "cflags": [ "-std=c++17" ],
          "cflags_cc": [ "-std=c++17" ]
        }]
      ]
    }
  ]
}
