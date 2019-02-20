#include <gdnative_api_struct.gen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "camera.h"
#include "processing.h"

typedef struct camera_data_struct {
	void* camera;
    godot_pool_byte_array buffer;
    unsigned long counter;
} camera_data_struct;

const godot_gdnative_core_api_struct *api = NULL;
const godot_gdnative_ext_nativescript_api_struct *nativescript_api = NULL;

GDCALLINGCONV void *_camera_constructor(godot_object *p_instance, void *p_method_data);
GDCALLINGCONV void _camera_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data);
godot_variant _camera_get_image(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
godot_variant _camera_get_width(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
godot_variant _camera_flip(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
godot_variant _camera_get_height(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
godot_variant _camera_open(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
godot_variant _camera_set_default(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
godot_variant _camera_detect_face(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
godot_variant _camera_compute_flow(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);

void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *p_options) {
	api = p_options->api_struct;

	// now find our extensions
	for (int i = 0; i < api->num_extensions; i++) {
		switch (api->extensions[i]->type) {
			case GDNATIVE_EXT_NATIVESCRIPT: {
				nativescript_api = (godot_gdnative_ext_nativescript_api_struct *)api->extensions[i];
			}; break;
			default: break;
		};
	};
}

godot_arvr_interface_gdnative ar_interface;

void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *p_options) {

    camera_delete_all();

	api = NULL;
	nativescript_api = NULL;
}

void GDN_EXPORT godot_nativescript_init(void *p_handle) {

	godot_instance_create_func create = { NULL, NULL, NULL };
	create.create_func = &_camera_constructor;

	godot_instance_destroy_func destroy = { NULL, NULL, NULL };
	destroy.destroy_func = &_camera_destructor;

	nativescript_api->godot_nativescript_register_class(p_handle, "Camera", "Reference", create, destroy);

	godot_method_attributes attributes = { GODOT_METHOD_RPC_MODE_DISABLED };

	godot_instance_method get_image = { NULL, NULL, NULL };
	get_image.method = &_camera_get_image;

	nativescript_api->godot_nativescript_register_method(p_handle, "Camera", "get_image", attributes, get_image);

	godot_instance_method open = { NULL, NULL, NULL };
	open.method = &_camera_open;

	nativescript_api->godot_nativescript_register_method(p_handle, "Camera", "open", attributes, open);

	godot_instance_method get_width = { NULL, NULL, NULL };
	get_width.method = &_camera_get_width;

	nativescript_api->godot_nativescript_register_method(p_handle, "Camera", "get_width", attributes, get_width);

	godot_instance_method get_height = { NULL, NULL, NULL };
	get_height.method = &_camera_get_height;

	nativescript_api->godot_nativescript_register_method(p_handle, "Camera", "get_height", attributes, get_height);

    godot_instance_method set_default = { NULL, NULL, NULL };
	set_default.method = &_camera_set_default;

	nativescript_api->godot_nativescript_register_method(p_handle, "Camera", "set_default", attributes, set_default);

    godot_instance_method flip = { NULL, NULL, NULL };
	flip.method = &_camera_flip;

	nativescript_api->godot_nativescript_register_method(p_handle, "Camera", "flip", attributes, flip);

	godot_instance_method detect_face = { NULL, NULL, NULL };
	detect_face.method = &_camera_detect_face;

	nativescript_api->godot_nativescript_register_method(p_handle, "Camera", "detect_face", attributes, detect_face);

	godot_instance_method compute_flow = { NULL, NULL, NULL };
	compute_flow.method = &_camera_compute_flow;

	nativescript_api->godot_nativescript_register_method(p_handle, "Camera", "compute_flow", attributes, compute_flow);

printf("Loading resources \n");

    processing_initialize();

}

GDCALLINGCONV void *_camera_constructor(godot_object *p_instance, void *p_method_data) {

    camera_data_struct *user_data = api->godot_alloc(sizeof(camera_data_struct));

	user_data->camera = NULL;
    api->godot_pool_byte_array_new(&user_data->buffer);
    user_data->counter = 0;

fprintf(stderr, "Loading resources \n");

    return user_data;

}

GDCALLINGCONV void _camera_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data) {

    camera_data_struct *user_data = (camera_data_struct*) p_user_data;

    if (user_data->camera) {
        camera_delete(user_data->camera);
        user_data->camera = NULL;
    }

    api->godot_pool_byte_array_destroy(&user_data->buffer);
    api->godot_free(p_user_data);
}

godot_variant _camera_set_default(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

    int camera_id = 0, success;
    int width, height, ms;
    godot_variant res;

    if (p_num_args > 0) {
        camera_id = api->godot_variant_as_int(p_args[0]);
    }

    camera_set_default(camera_id);

    api->godot_variant_new_bool(&res, 1);
	return res;
}

godot_variant _camera_open(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

    int camera_id = -1, success;
    int width, height, ms;
    godot_variant res;

    camera_data_struct *user_data = (camera_data_struct*) p_user_data;

    if (p_num_args > 0) {
        camera_id = api->godot_variant_as_int(p_args[0]);
    }

    if (!user_data) {
        api->godot_variant_new_bool(&res, GODOT_FALSE);
	    return res;
    }

    if (user_data->camera) {
        camera_delete(user_data->camera);
    }

    user_data->camera = camera_create(camera_id);

    if (!user_data->camera) {
        api->godot_variant_new_bool(&res, GODOT_FALSE);
	    return res;
    }

    width = camera_get_width(user_data->camera);
    height = camera_get_height(user_data->camera);

    ms = (width < height) ? height : width;
    api->godot_pool_byte_array_resize(&user_data->buffer, ms * ms * 3);

    printf("Opened camera \n");

    api->godot_variant_new_bool(&res, 1);
	return res;
}

godot_variant _camera_get_image(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

    int updated = 0;

    godot_variant res;
	camera_data_struct * user_data = (camera_data_struct *) p_user_data;
    godot_pool_byte_array_write_access* writer;

    writer = api->godot_pool_byte_array_write(&user_data->buffer);
    uint8_t* data = api->godot_pool_byte_array_write_access_ptr(writer);
    updated = camera_get_image(user_data->camera, data, &user_data->counter);
    api->godot_pool_byte_array_write_access_destroy(writer);


    if (updated) {
        api->godot_variant_new_pool_byte_array(&res, &user_data->buffer);
    } else {
        api->godot_variant_new_bool(&res, GODOT_FALSE);
    }

	return res;
}

godot_variant _camera_get_width(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

    godot_variant res;
	camera_data_struct * user_data = (camera_data_struct *) p_user_data;

    api->godot_variant_new_int(&res, camera_get_width(user_data->camera));
	return res;
}

godot_variant _camera_get_height(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

    godot_variant res;
	camera_data_struct * user_data = (camera_data_struct *) p_user_data;

    api->godot_variant_new_int(&res, camera_get_height(user_data->camera));
	return res;
}

godot_variant _camera_flip(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

    godot_variant res;
	camera_data_struct * user_data = (camera_data_struct *) p_user_data;

    int flip_lr = 1, flip_ud = 0;

    if (p_num_args > 0) {
        flip_lr = api->godot_variant_as_bool(p_args[0]);
    }

    if (p_num_args > 1) {
        flip_ud = api->godot_variant_as_bool(p_args[1]);
    }

    camera_flip(user_data->camera, flip_lr, flip_ud);

    api->godot_variant_new_bool(&res, GODOT_TRUE);

	return res;
}


godot_variant _camera_detect_face(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

    godot_variant res;
	camera_data_struct * user_data = (camera_data_struct *) p_user_data;

    region r = processing_detect_face(user_data->camera);

    if (r.x < 0) {
        api->godot_variant_new_bool(&res, GODOT_FALSE);
        return res;
    }

    godot_rect2 grec;

    api->godot_rect2_new(&grec, r.x, r.y, r.w, r.h);

    api->godot_variant_new_rect2(&res, &grec);

	return res;
}

godot_variant _camera_compute_flow(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

    godot_variant res;
	camera_data_struct * user_data = (camera_data_struct *) p_user_data;

    if (p_num_args == 0) {
        api->godot_variant_new_bool(&res, GODOT_FALSE);
        return res;
    }

    godot_rect2 grec = api->godot_variant_as_rect2(p_args[0]);

    region r;
    godot_vector2 p = api->godot_rect2_get_position(&grec);
    godot_vector2 s = api->godot_rect2_get_size(&grec);
    r.w = api->godot_vector2_get_x(&s);
    r.h = api->godot_vector2_get_y(&s);
    r.x = api->godot_vector2_get_x(&p) - r.w / 2;
    r.y = api->godot_vector2_get_y(&p) - r.h / 2;

    flow f = processing_calculate_flow(user_data->camera, r);

    godot_vector2 gvec;
    api->godot_vector2_new(&gvec, f.x, f.y);

    api->godot_variant_new_vector2(&res, &gvec);

	return res;
}


