#include <nan.h>
#include "node_gd.h"
#include "node_gd.cc"

NODE_MODULE(node_gd, Gd::Init);
