#pragma once

#include <node.h>
#include <node_object_wrap.h>
#include <bits/stringfwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>


class FastImage : public node::ObjectWrap {
public:
    static void Init(v8::Handle<v8::Object> exports);
    int GetWidth();
    int GetHeight();
    void Save(std::string path, std::vector<int> flags);
    void Resize(int width, int height, bool saveAspectRatio);
    void Crop(int x, int y, int w, int h);

private:
    cv::Mat cvImage;
    std::string inputFile;

    explicit FastImage();
    void Open(std::string path);
    ~FastImage();

    static void JsConstructor(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void JsGetPath(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void JsGetWidth(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void JsGetHeight(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void JsSave(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void JsResize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void JsCrop(const v8::FunctionCallbackInfo<v8::Value>& args);

    static v8::Persistent<v8::Function> constructor;
};
