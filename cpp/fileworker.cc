#include <nan.h>
#include "node_gd.h"
#include <gd.h>

class FileWorker : public Nan::AsyncWorker {
  public:
    FileWorker(Nan::Callback *callback, gdImagePtr image, char *path)
      : Nan::AsyncWorker(callback), _image(image), _path(path) {}
    ~FileWorker() {}

    void Execute() {
      result = gdImageFile(_image, _path);
      if (result == false) {
        SetErrorMessage("Unable to write file.");
      }
    }

    void HandleOKCallback() {
      v8::Local<v8::Value> argv[] = {
        Nan::Null()
      };
      callback->Call(1, argv);
    }

  private:
    gdImagePtr _image;
    char *_path;
    bool result;
};
