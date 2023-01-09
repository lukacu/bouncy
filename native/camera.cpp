
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <map>

#include "camera.h"


Camera::Camera(int camera) : fps(30), flip_lr(false), flip_ud(false) {

    capture.open(camera);

    if (!capture.isOpened()) {
        capture.release();
        throw std::runtime_error("Unable to open camera");
    }

    width = (int) capture.get(CAP_PROP_FRAME_WIDTH);
    height = (int) capture.get(CAP_PROP_FRAME_HEIGHT);


    std::cout << "Camera ready (" << width << "x" << height << ")" << std::endl;

    worker = std::thread(&Camera::loop, this);

}

Camera::~Camera() {

    {

        std::cout << "Closing camera" << std::endl;

        std::lock_guard<std::recursive_mutex> lock(guard);
        capture.release();

    }

    worker.join();

}

Mat Camera::getFrame() {

    std::lock_guard<std::recursive_mutex> lock(guard);

	return frame;
}


unsigned long Camera::getFrameNumber() {

    std::lock_guard<std::recursive_mutex> lock(guard);

    return counter;

}

Mat Camera::getFrameIfNewer(unsigned long& current) {

    std::lock_guard<std::recursive_mutex> lock(guard);

    if (current == counter) return Mat();

    current = counter;

    return frame;

}

int Camera::getWidth() {

    return width;

}


int Camera::getHeight() {

    return height;

}

int Camera::flip(bool flip_lr, bool flip_ud) {

    this->flip_lr = flip_lr;
    this->flip_ud = flip_ud;

    return 1;

}

void Camera::loop() {

    while (true) {

        auto start = std::chrono::high_resolution_clock::now();

        {

            std::lock_guard<std::recursive_mutex> lock(guard);

            capture.read(frame);

            if (frame.empty()) {
                break;
            }

            if (flip_lr || flip_ud) {
                int code = flip_lr ? (flip_ud ? -1 : 1) : 0;
                cv::flip(frame, frame, code);
            }

            counter++;

        }

        auto end = std::chrono::high_resolution_clock::now();

        auto used = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

		auto remaining = std::chrono::milliseconds(std::max(1l, (long)((1000.0 / fps) - used)));

        std::this_thread::sleep_for(remaining);

    }

}


static std::map<int, std::weak_ptr<Camera> > cameras;

static int default_camera = 0;

SharedCamera camera_open(int id) {

    if (id < 0) id = default_camera;

    std::cout << "Query camera " << id << std::endl;

    if (cameras.find(id) == cameras.end()) {

        try {

            SharedCamera camera = std::make_shared<Camera>(id);

            cameras[id] = camera;

            std::cout << "Ready camera " << id << std::endl;

            return camera;

        } catch (const std::runtime_error& e) {
            return std::shared_ptr<Camera>();
        }

    } else {

        return cameras[id].lock();

    }

}

void camera_delete_all() {

    cameras.clear();

}

void* camera_create(int id) {

    SharedCamera camera = camera_open(id);

    return new SharedCamera(camera);

}

void camera_delete(void *obj) {

    if (!obj) return;

    delete (SharedCamera*) obj;

}

int camera_get_image(void *obj, uint8_t* buffer, unsigned long* newer) {

    SharedCamera user_data = *((SharedCamera*) obj);

    Mat frame;

    if (newer)
        frame = user_data->getFrameIfNewer(*newer);
    else {
        frame = user_data->getFrame();
    }

    if (frame.empty()) {
        return 0;
    }

    Mat wrapper(user_data->getHeight(), user_data->getWidth(), CV_8UC3, buffer, std::max(user_data->getHeight(), user_data->getWidth()) * 3);
    cvtColor(frame, wrapper, COLOR_BGR2RGB);

	return 1;
}

int camera_get_width(void *obj) {

    SharedCamera user_data = *((SharedCamera*) obj);

    return user_data->getWidth();
}


int camera_get_height(void *obj) {

    SharedCamera user_data = *((SharedCamera*) obj);

    return user_data->getHeight();
}

void camera_set_default(int id) {
    default_camera = id;
}

void camera_flip(void *obj, int flip_lr, int flip_ud) {

    SharedCamera user_data = *((SharedCamera*) obj);

    user_data->flip(flip_lr, flip_ud);
}



