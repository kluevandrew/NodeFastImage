// FastImage.cc
#include "FastImage.h"

using namespace v8;

/** Math Helpers **/

int Round(float number)
{
    return (int) (number < 0.0 ? std::ceil(number - 0.5) : std::floor(number + 0.5));
}

std::vector<int> GetClosestSizeKeepAspectRatio(int realWidth, int realHeight, int targetWidth, int targetHeight)
{
    std::vector<int> size(2);

    float aspectRatio = (float) realWidth / (float) realHeight;

    if (targetWidth == 0) {
        targetWidth = Round((float) targetHeight * aspectRatio);
    } else if (targetHeight == 0) {
        targetHeight = Round((float) targetWidth * aspectRatio);
    }

    int newWidth  = realWidth;
    int newHeight = realHeight;

    if (newWidth > targetWidth || newHeight > targetHeight) {
        if (newWidth > targetWidth && newHeight > targetHeight) {
            newWidth = targetWidth;
            newHeight = Round((float)newWidth / aspectRatio);
            if (newHeight > targetHeight) {
                newHeight = targetHeight;
                newWidth = Round((float)newHeight * aspectRatio);
            }
        } else {
            if (newWidth > targetWidth) {
                newWidth = targetWidth;
                newHeight = Round((float) newWidth / aspectRatio);
            } else {
                newHeight = targetHeight;
                newWidth = Round((float) newHeight * aspectRatio);
            }
        }
    }

    size[0] = (int)newWidth;
    size[1] = (int)newHeight;

    return size;
};

/**
 * Constructor
 */
Persistent<Function> FastImage::constructor;

FastImage::FastImage() {
}

/**
 * Destructor
 */
FastImage::~FastImage() {
    cvImage.release();
}

/**
 * Initialize OpenCV Image instance
 */
void FastImage::Open(std::string path) {

    inputFile = path;

    cvImage = cv::imread(inputFile);

    if (cvImage.empty()) {
        std::string message = (std::string)"FastImage: File \""+ inputFile + (std::string)"\" not exists or not valid image.";
        throw message;
    }
}

/**
 * Get image width
 */
int FastImage::GetWidth() {
    return cvImage.size().width;
}

/**
 * Get image height
 */
int FastImage::GetHeight() {
    return cvImage.size().height;
}

void FastImage::Save(std::string path = "", std::vector<int> flags = std::vector<int>())
{
    path = path == "" ? inputFile : path;

    if (!cv::imwrite(path, cvImage, flags)) {
        std::string message = (std::string)"FastImage: can't save file.";
        throw message;
    }
}

void FastImage::Resize(int width, int height, bool saveAspectRatio = false)
{
    if (saveAspectRatio) {
        std::vector<int> calculatedSize = GetClosestSizeKeepAspectRatio(GetWidth(), GetHeight(), width, height);
        width  = calculatedSize[0];
        height = calculatedSize[1];
    }

    cv::Mat newImage;
    if (width > 0 && height > 0) {
        cv::Size new_size;
        new_size.width = width;
        new_size.height = height;
        cv::resize(cvImage, newImage, new_size);
    } else {
        std::string message = (std::string)"FastImage.resize() to small size.";
        throw message;
    }

    cvImage = newImage;
    newImage.release();
}

void FastImage::Crop(int x, int y, int w, int h)
{
    if (x < 0 || y < 0 || w <= 0 || h <= 0) {
        std::string message = (std::string)"FastImage.crop() bad arguments";
        throw message;
    }

    if (((x+w) > cvImage.size().width) || ((y+h) > cvImage.size().height)) {
        std::string message = (std::string)"FastImage.crop() out of bounds";
        throw message;
    }

    cv::Rect roi(x, y, w, h);
    cv::Mat croppedImage = cvImage(roi);

    cvImage = croppedImage;
    croppedImage.release();
}

/**
* V8 Helpers
*/
void JsReturnInt(const FunctionCallbackInfo<Value>& args, int value) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    args.GetReturnValue().Set(Number::New(isolate, value));
}

void JsReturnString(const FunctionCallbackInfo<Value>& args, std::string value) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, value.c_str()));
}

void JsReturnThis(const FunctionCallbackInfo<Value>& args) {
    args.GetReturnValue().Set(args.This());
}

/**
 * JavaScript Object Definition
 */
void FastImage::Init(Handle<Object> exports) {
    Isolate* isolate = Isolate::GetCurrent();

    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, JsConstructor);
    tpl->SetClassName(String::NewFromUtf8(isolate, "FastImage"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    NODE_SET_PROTOTYPE_METHOD(tpl, "getPath", JsGetPath);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getWidth", JsGetWidth);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getHeight", JsGetHeight);
    NODE_SET_PROTOTYPE_METHOD(tpl, "save", JsSave);
    NODE_SET_PROTOTYPE_METHOD(tpl, "resize", JsResize);

    tpl->Set(v8::String::NewFromUtf8(isolate, "JPEG_QUALITY"), Number::New(isolate, 1) );
    tpl->Set(v8::String::NewFromUtf8(isolate, "PNG_COMPRESSION"), Number::New(isolate, 16) );
    tpl->Set(v8::String::NewFromUtf8(isolate, "PNG_STRATEGY"), Number::New(isolate, 17) );
    tpl->Set(v8::String::NewFromUtf8(isolate, "PNG_BILEVEL"), Number::New(isolate, 18) );
    tpl->Set(v8::String::NewFromUtf8(isolate, "PNG_STRATEGY_DEFAULT"), Number::New(isolate, 0) );
    tpl->Set(v8::String::NewFromUtf8(isolate, "PNG_STRATEGY_FILTERED"), Number::New(isolate, 1) );
    tpl->Set(v8::String::NewFromUtf8(isolate, "PNG_STRATEGY_HUFFMAN_ONLY"), Number::New(isolate, 2) );
    tpl->Set(v8::String::NewFromUtf8(isolate, "PNG_STRATEGY_RLE"), Number::New(isolate, 3) );
    tpl->Set(v8::String::NewFromUtf8(isolate, "PNG_STRATEGY_FIXED"), Number::New(isolate, 4) );
    tpl->Set(v8::String::NewFromUtf8(isolate, "PXM_BINARY"), Number::New(isolate, 32) );
    
    constructor.Reset(isolate, tpl->GetFunction());
    exports->Set(String::NewFromUtf8(isolate, "FastImage"), tpl->GetFunction());
}

std::string argToStdString(Value* arg) {
    v8::String::Utf8Value utf8(arg->ToString());

    return std::string(*utf8);
}

/** JavaScript Methods **/

/**
 * JavaScript Constructor
 */
void FastImage::JsConstructor(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    if (args.IsConstructCall()) {
        // Invoked as constructor  `new FastImage(...)`
        std::string path = argToStdString(*args[0]);

        FastImage* instance = new FastImage();
        instance->Wrap(args.This());

        try {
            instance->Open(path);
            args.GetReturnValue().Set(args.This());
        } catch (std::string e) {
            isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, e.c_str())));
        }
    } else {
        // Invoked as plain function `FastImage(...)`, turn into construct call.
        const int argc = 1;
        Local<Value> argv[argc] = { args[0] };
        Local<Function> cons = Local<Function>::New(isolate, constructor);
        args.GetReturnValue().Set(cons->NewInstance(argc, argv));
    }
}

/**
 *
 */
void FastImage::JsGetPath(const FunctionCallbackInfo<Value>& args) {
    FastImage* that = ObjectWrap::Unwrap<FastImage>(args.Holder());
    JsReturnString(args, that->inputFile);
}

void FastImage::JsGetWidth(const FunctionCallbackInfo<Value>& args) {
    FastImage* that = ObjectWrap::Unwrap<FastImage>(args.Holder());
    JsReturnInt(args, that->GetWidth());
}

void FastImage::JsGetHeight(const FunctionCallbackInfo<Value>& args) {
    FastImage* that = ObjectWrap::Unwrap<FastImage>(args.Holder());
    JsReturnInt(args, that->GetHeight());
}

void FastImage::JsSave(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    FastImage* that = ObjectWrap::Unwrap<FastImage>(args.Holder());

    std::string path = argToStdString(*args[0]);
    std::vector<int> flags;

    v8::Handle<v8::Array> falgsArg = v8::Handle<v8::Array>::Cast(args[1]);
    for (uint32_t i = 0; i < falgsArg->Length(); i++) {
        flags.push_back((int)falgsArg->Get(i)->NumberValue());
    }

    try {
        that->Save(path, flags);
    } catch (std::string e) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, e.c_str())));
    }

    JsReturnThis(args);
}

void FastImage::JsResize(const FunctionCallbackInfo<Value>& args) {
    FastImage* that = ObjectWrap::Unwrap<FastImage>(args.Holder());
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    int w = (uint32_t)args[0]->NumberValue();
    int h = (uint32_t)args[1]->NumberValue();
    bool keepAspectRatio = args[2]->BooleanValue();

    try {
        that->Resize(w, h, keepAspectRatio);
    } catch (std::string e) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, e.c_str())));
    }

    JsReturnThis(args);
}

void FastImage::JsCrop(const FunctionCallbackInfo<Value>& args) {
    FastImage* that = ObjectWrap::Unwrap<FastImage>(args.Holder());
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    int x = (int)args[0]->NumberValue();
    int y = (int)args[1]->NumberValue();
    int w = (int)args[2]->NumberValue();
    int h = (int)args[3]->NumberValue();

    try {
        that->Crop(x, y, w, h);
    } catch (std::string e) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, e.c_str())));
    }

    JsReturnThis(args);
}