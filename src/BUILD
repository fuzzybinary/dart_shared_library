cc_shared_library(
    name = "dyndart",
    roots = [ ":libdyndart" ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "libdyndart",
    hdrs = [ "dart_dll.h" ],
    srcs = [ "dart_dll.cpp" ],
    visibility = ["//visibility:public"],
    copts = [ "/DDART_DLL_EXPORTING" ]
)