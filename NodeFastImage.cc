#include <node.h>
#include "FastImage.h"

using namespace v8;

void InitAll(Handle<Object> exports) {
    FastImage::Init(exports);
}

NODE_MODULE(addon, InitAll)