
#ifndef CAMERA_API_H
#define CAMERA_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void* camera_create(int camera);
void camera_delete(void *obj);

int camera_get_image(void *obj, uint8_t* buffer, unsigned long* newer);
int camera_get_width(void *obj);
int camera_get_height(void *obj);

void camera_flip(void *obj, int flip_lr, int flip_ud);

void camera_delete_all();

void camera_set_default(int id);

#ifdef __cplusplus
}


#include <thread>
#include <mutex>
#include <memory>

#include <opencv2/videoio/videoio.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

class Camera {
public:

    Camera(int camera);
    ~Camera();

    Mat getFrame();
    Mat getFrameIfNewer(unsigned long& current);
    unsigned long getFrameNumber();
    int getWidth();
    int getHeight();

    int flip(bool flip_lr = true, bool flip_ud = false);

private:

    void loop();

	VideoCapture capture;
    Mat frame;
    int width;
    int height;
    unsigned long counter;
    float fps;
    bool flip_lr, flip_ud;

    std::recursive_mutex guard;
    std::thread worker;

};

typedef std::shared_ptr<Camera> SharedCamera;

SharedCamera camera_open(int id = -1);

#endif


#endif
