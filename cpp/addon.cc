#include <napi.h>
#include "node_gd.h"
#include "node_gd.cc"

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  Gd::Init(env, exports);

  return exports;
}

NODE_API_MODULE(node_gd, InitAll);
