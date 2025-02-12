#include "yolo.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <time.h>

#include "cpu.h"
#include <omp.h>


#define LOG_TAG "NDK-TIMESTAMP"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

static const char *class_names[] = {
        "nest",
        "plastic",
        "flotsam",
        "ball",
        "light"
};

// 获取当前时间戳（以纳秒为单位）
uint64_t get_timestamp_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts); // 使用 CLOCK_MONOTONIC 获取单调递增时间
    return ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

void putKeyValueToMap(JNIEnv *env, jobject hashMapObj,
                      jclass hashMapClass,
                      jstring xKeyStr,
                      jobject xValue) {
    env->CallObjectMethod(hashMapObj,
                          env->GetMethodID(hashMapClass,
                                           "put",
                                           "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;"),
                          xKeyStr, xValue);
}

jstring charToString(JNIEnv *env, std::string data) {
    return env->NewStringUTF(data.c_str());
}

jstring intToString(JNIEnv *env, jint number) {
    // 将int转换为std::string
    std::string numStr = std::to_string(number);
    // 使用JNIEnv的NewStringUTF方法将std::string转换为jstring
    jstring jstr = env->NewStringUTF(numStr.c_str());
    return jstr;
}

jstring floatToString(JNIEnv *env, jfloat number) {
    // 将int转换为std::string
    std::string numStr = std::to_string(number);
    // 使用JNIEnv的NewStringUTF方法将std::string转换为jstring
    jstring jstr = env->NewStringUTF(numStr.c_str());
    return jstr;
}


static float fast_exp(float x) {
    union {
        uint32_t i;
        float f;
    } v{};
    v.i = (1 << 23) * (1.4426950409 * x + 126.93490512f);
    return v.f;
}

static float sigmoid(float x) {
    return 1.0f / (1.0f + fast_exp(-x));
}

static float intersection_area(const Object &a, const Object &b) {
    cv::Rect_<float> inter = a.rect & b.rect;
    return inter.area();
}

static void qsort_descent_inplace(std::vector<Object> &faceobjects, int left, int right) {
    int i = left;
    int j = right;
    float p = faceobjects[(left + right) / 2].prob;

    while (i <= j) {
        while (faceobjects[i].prob > p)
            i++;

        while (faceobjects[j].prob < p)
            j--;

        if (i <= j) {
            // swap
            std::swap(faceobjects[i], faceobjects[j]);

            i++;
            j--;
        }
    }

    //     #pragma omp parallel sections
    {
        //         #pragma omp section
        {
            if (left < j) qsort_descent_inplace(faceobjects, left, j);
        }
        //         #pragma omp section
        {
            if (i < right) qsort_descent_inplace(faceobjects, i, right);
        }
    }
}

static void qsort_descent_inplace(std::vector<Object> &faceobjects) {
    if (faceobjects.empty())
        return;

    qsort_descent_inplace(faceobjects, 0, faceobjects.size() - 1);
}

static void nms_sorted_bboxes(const std::vector<Object> &faceobjects, std::vector<int> &picked,
                              float nms_threshold) {
    picked.clear();

    const int n = faceobjects.size();

    std::vector<float> areas(n);
    for (int i = 0; i < n; i++) {
        areas[i] = faceobjects[i].rect.width * faceobjects[i].rect.height;
    }

    for (int i = 0; i < n; i++) {
        const Object &a = faceobjects[i];

        int keep = 1;
        for (int j = 0; j < (int) picked.size(); j++) {
            const Object &b = faceobjects[picked[j]];

            // intersection over union
            float inter_area = intersection_area(a, b);
            float union_area = areas[i] + areas[picked[j]] - inter_area;
            // float IoU = inter_area / union_area
            if (inter_area / union_area > nms_threshold)
                keep = 0;
        }

        if (keep)
            picked.push_back(i);
    }
}

static void
generate_grids_and_stride(const int target_w, const int target_h, std::vector<int> &strides,
                          std::vector<GridAndStride> &grid_strides) {
    for (int i = 0; i < (int) strides.size(); i++) {
        int stride = strides[i];
        int num_grid_w = target_w / stride;
        int num_grid_h = target_h / stride;
        for (int g1 = 0; g1 < num_grid_h; g1++) {
            for (int g0 = 0; g0 < num_grid_w; g0++) {
                GridAndStride gs;
                gs.grid0 = g0;
                gs.grid1 = g1;
                gs.stride = stride;
                grid_strides.push_back(gs);
            }
        }
    }
}

static void generate_proposals(std::vector<GridAndStride> grid_strides, const ncnn::Mat &pred,
                               float prob_threshold, std::vector<Object> &objects) {
    const int num_points = grid_strides.size();
    const int num_class = 5;
    const int reg_max_1 = 16;

    for (int i = 0; i < num_points; i++) {
        const float *scores = pred.row(i) + 4 * reg_max_1;

        // find label with max score
        int label = -1;
        float score = -FLT_MAX;
        for (int k = 0; k < num_class; k++) {
            float confidence = scores[k];
            if (confidence > score) {
                label = k;
                score = confidence;
            }
        }
        float box_prob = sigmoid(score);
        if (box_prob >= prob_threshold) {
            ncnn::Mat bbox_pred(reg_max_1, 4, (void *) pred.row(i));
            {
                ncnn::Layer *softmax = ncnn::create_layer("Softmax");

                ncnn::ParamDict pd;
                pd.set(0, 1); // axis
                pd.set(1, 1);
                softmax->load_param(pd);

                ncnn::Option opt;
                opt.num_threads = 1;
                opt.use_packing_layout = false;

                softmax->create_pipeline(opt);

                softmax->forward_inplace(bbox_pred, opt);

                softmax->destroy_pipeline(opt);

                delete softmax;
            }

            float pred_ltrb[4];
            for (int k = 0; k < 4; k++) {
                float dis = 0.f;
                const float *dis_after_sm = bbox_pred.row(k);
                for (int l = 0; l < reg_max_1; l++) {
                    dis += l * dis_after_sm[l];
                }

                pred_ltrb[k] = dis * grid_strides[i].stride;
            }

            float pb_cx = (grid_strides[i].grid0 + 0.5f) * grid_strides[i].stride;
            float pb_cy = (grid_strides[i].grid1 + 0.5f) * grid_strides[i].stride;

            float x0 = pb_cx - pred_ltrb[0];
            float y0 = pb_cy - pred_ltrb[1];
            float x1 = pb_cx + pred_ltrb[2];
            float y1 = pb_cy + pred_ltrb[3];

            Object obj;
            obj.rect.x = x0;
            obj.rect.y = y0;
            obj.rect.width = x1 - x0;
            obj.rect.height = y1 - y0;
            obj.label = label;
            obj.prob = box_prob;

            objects.push_back(obj);
        }
    }
}

Yolo::Yolo() {
    blob_pool_allocator.set_size_compare_ratio(0.f);
    workspace_pool_allocator.set_size_compare_ratio(0.f);
}


int Yolo::load(AAssetManager *mgr, const char *modeltype, int _target_size, const float *_mean_vals,
               const float *_norm_vals, bool use_gpu) {
    yolo.clear();
    blob_pool_allocator.clear();
    workspace_pool_allocator.clear();

    ncnn::set_cpu_powersave(2);
    ncnn::set_omp_num_threads(ncnn::get_big_cpu_count());

    yolo.opt = ncnn::Option();

#if NCNN_VULKAN
    yolo.opt.use_vulkan_compute = use_gpu;
#endif

    yolo.opt.num_threads = ncnn::get_big_cpu_count();
    yolo.opt.blob_allocator = &blob_pool_allocator;
    yolo.opt.workspace_allocator = &workspace_pool_allocator;

    char parampath[256];
    char modelpath[256];
    sprintf(parampath, "yolov8%s.param", modeltype);
    sprintf(modelpath, "yolov8%s.bin", modeltype);

    yolo.load_param(mgr, parampath);
    yolo.load_model(mgr, modelpath);

    target_size = _target_size;
    mean_vals[0] = _mean_vals[0];
    mean_vals[1] = _mean_vals[1];
    mean_vals[2] = _mean_vals[2];
    norm_vals[0] = _norm_vals[0];
    norm_vals[1] = _norm_vals[1];
    norm_vals[2] = _norm_vals[2];

    return 0;
}

int Yolo::detect(const cv::Mat &rgb, std::vector<Object> &objects, float prob_threshold,
                 float nms_threshold) {
    int width = rgb.cols;
    int height = rgb.rows;

    // pad to multiple of 32
    int w = width;
    int h = height;
    float scale = 1.f;
    if (w > h) {
        scale = (float) target_size / w;
        w = target_size;
        h = h * scale;
    } else {
        scale = (float) target_size / h;
        h = target_size;
        w = w * scale;
    }

    ncnn::Mat in = ncnn::Mat::from_pixels_resize(rgb.data, ncnn::Mat::PIXEL_RGB2BGR, width, height,
                                                 w, h);

    // pad to target_size rectangle
    int wpad = (w + 31) / 32 * 32 - w;
    int hpad = (h + 31) / 32 * 32 - h;
    ncnn::Mat in_pad;
    ncnn::copy_make_border(in, in_pad, hpad / 2, hpad - hpad / 2, wpad / 2, wpad - wpad / 2,
                           ncnn::BORDER_CONSTANT, 0.f);

    in_pad.substract_mean_normalize(0, norm_vals);

    ncnn::Extractor ex = yolo.create_extractor();

    ex.input("images", in_pad);

    std::vector<Object> proposals;

    ncnn::Mat out;
    ex.extract("output0", out);

    std::vector<int> strides = {8, 16, 32}; // might have stride=64
    std::vector<GridAndStride> grid_strides;
    generate_grids_and_stride(in_pad.w, in_pad.h, strides, grid_strides);
    generate_proposals(grid_strides, out, prob_threshold, proposals);

    // sort all proposals by score from highest to lowest
    qsort_descent_inplace(proposals);

    // apply nms with nms_threshold
    std::vector<int> picked;
    nms_sorted_bboxes(proposals, picked, nms_threshold);

    int count = picked.size();

    objects.resize(count);
    for (int i = 0; i < count; i++) {
        objects[i] = proposals[picked[i]];

        // adjust offset to original unpadded
        float x0 = (objects[i].rect.x - (wpad / 2)) / scale;
        float y0 = (objects[i].rect.y - (hpad / 2)) / scale;
        float x1 = (objects[i].rect.x + objects[i].rect.width - (wpad / 2)) / scale;
        float y1 = (objects[i].rect.y + objects[i].rect.height - (hpad / 2)) / scale;

        // clip
        x0 = std::max(std::min(x0, (float) (width - 1)), 0.f);
        y0 = std::max(std::min(y0, (float) (height - 1)), 0.f);
        x1 = std::max(std::min(x1, (float) (width - 1)), 0.f);
        y1 = std::max(std::min(y1, (float) (height - 1)), 0.f);

        objects[i].rect.x = x0;
        objects[i].rect.y = y0;
        objects[i].rect.width = x1 - x0;
        objects[i].rect.height = y1 - y0;
    }

    // sort objects by area
    struct {
        bool operator()(const Object &a, const Object &b) const {
            return a.rect.area() > b.rect.area();
        }
    } objects_area_greater;
    std::sort(objects.begin(), objects.end(), objects_area_greater);

    return 0;
}

int Yolo::draw(cv::Mat &rgb, const std::vector<Object> &objects) {
    /* static const char* class_names[] = {
             "person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light",
             "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow",
             "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee",
             "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard",
             "tennis racket", "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple",
             "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch",
             "potted plant", "bed", "dining table", "toilet", "tv", "laptop", "mouse", "remote", "keyboard", "cell phone",
             "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear",
             "hair drier", "toothbrush"
     };*/

    static const unsigned char colors[19][3] = {
            {54,  67,  244},
            {99,  30,  233},
            {176, 39,  156},
            {183, 58,  103},
            {181, 81,  63},
            {243, 150, 33},
            {244, 169, 3},
            {212, 188, 0},
            {136, 150, 0},
            {80,  175, 76},
            {74,  195, 139},
            {57,  220, 205},
            {59,  235, 255},
            {7,   193, 255},
            {0,   152, 255},
            {34,  87,  255},
            {72,  85,  121},
            {158, 158, 158},
            {139, 125, 96}
    };

    int color_index = 0;

    for (size_t i = 0; i < objects.size(); i++) {
        const Object &obj = objects[i];

//         fprintf(stderr, "%d = %.5f at %.2f %.2f %.2f x %.2f\n", obj.label, obj.prob,
//                 obj.rect.x, obj.rect.y, obj.rect.width, obj.rect.height);

        const unsigned char *color = colors[color_index % 19];
        color_index++;

        cv::Scalar cc(color[0], color[1], color[2]);

        cv::rectangle(rgb, obj.rect, cc, 2);

        char text[256];
        sprintf(text, "%s %.1f%%", class_names[obj.label], obj.prob * 100);

        int baseLine = 0;
        cv::Size label_size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);

        int x = obj.rect.x;
        int y = obj.rect.y - label_size.height - baseLine;
        if (y < 0)
            y = 0;
        if (x + label_size.width > rgb.cols)
            x = rgb.cols - label_size.width;

        cv::rectangle(rgb, cv::Rect(cv::Point(x, y),
                                    cv::Size(label_size.width, label_size.height + baseLine)), cc,
                      -1);

        cv::Scalar textcc = (color[0] + color[1] + color[2] >= 381) ? cv::Scalar(0, 0, 0)
                                                                    : cv::Scalar(255, 255, 255);

        cv::putText(rgb, text, cv::Point(x, y + label_size.height), cv::FONT_HERSHEY_SIMPLEX, 0.5,
                    textcc, 1);
    }

    return 0;
}

// 检测静态图片
jobject Yolo::detected_static_pic(JNIEnv *env, jobject thiz, jobject bitmap) {

    // 结果数据
    jclass arrayListClass = env->FindClass("java/util/ArrayList");
    if (arrayListClass == nullptr) {
        __android_log_print(ANDROID_LOG_ERROR, "Yolo", "Failed to find java/util/ArrayList class.");
        return nullptr;
    }
    jmethodID arrayListInit = env->GetMethodID(arrayListClass, "<init>", "()V");
    if (arrayListInit == nullptr) {
        __android_log_print(ANDROID_LOG_ERROR, "Yolo", "Failed to find ArrayList constructor.");
        return nullptr;
    }
    jobject arrayListObj = env->NewObject(arrayListClass, arrayListInit);
    if (arrayListObj == nullptr) {
        __android_log_print(ANDROID_LOG_ERROR, "Yolo", "Failed to create ArrayList object.");
        return nullptr;
    }

    // 获取Bitmap信息
    AndroidBitmapInfo info;
    if (AndroidBitmap_getInfo(env, bitmap, &info) < 0) {
        __android_log_print(ANDROID_LOG_ERROR, "Yolo", "Failed to get bitmap info.");
        return arrayListObj;
    }
    if (info.format != ANDROID_BITMAP_FORMAT_RGB_565 &&
        info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        __android_log_print(ANDROID_LOG_ERROR, "Yolo", "Unsupported bitmap format: %d",
                            info.format);
        return arrayListObj;
    }

    // 锁定Bitmap像素
    void *pixels = nullptr;
    if (AndroidBitmap_lockPixels(env, bitmap, &pixels) < 0) {
        __android_log_print(ANDROID_LOG_ERROR, "Yolo", "Failed to lock bitmap pixels.");
        return arrayListObj;
    }
    __android_log_print(ANDROID_LOG_DEBUG, "Yolo", "AndroidBitmap_lockPixels");


    // 计算RGB数据大小
    size_t pixelCount = info.width * info.height;
    size_t rgbDataSize = pixelCount * 3; // 每个像素3个字节(RGB)
    std::vector<unsigned char> rgbData(rgbDataSize);

    int64 start_time = get_timestamp_ns();
    // 提取RGB值
    uint8_t *p = static_cast<uint8_t *>(pixels);
    for (size_t i = 0; i < pixelCount; ++i) {
        if (info.format == ANDROID_BITMAP_FORMAT_RGB_565) {
            // 对于RGB_565格式，需要转换为RGB888
            uint16_t pixel = *reinterpret_cast<uint16_t *>(p);
            rgbData[i * 3] = ((pixel >> 8) & 0xF8) | ((pixel >> 13) & 0x07); // R
            rgbData[i * 3 + 1] = ((pixel >> 3) & 0xF8) | ((pixel >> 11) & 0x07); // G
            rgbData[i * 3 + 2] = ((pixel << 3) & 0xF8) | ((pixel >> 5) & 0x07); // B
            p += 2; // RGB_565每像素2字节
        } else if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
            // 直接提取RGB分量，忽略Alpha
            rgbData[i * 3] = p[0]; // R
            rgbData[i * 3 + 1] = p[1]; // G
            rgbData[i * 3 + 2] = p[2]; // B
            p += 4; // ARGB_8888每像素4字节
        }
    }

    // 获取结束时间
    uint64_t end_time = get_timestamp_ns();

    // 计算方法执行时间（以毫秒为单位）
    double elapsed_time_ms = (end_time - start_time) / 1000000.0; // 转换为毫秒
    LOGI("example_method executed in %.3f ms", elapsed_time_ms);

    int width = info.width;
    int height = info.height;
    // 解锁Bitmap像素
    AndroidBitmap_unlockPixels(env, bitmap);

    // 确保数据量与宽度、高度匹配
    if (rgbData.size() != static_cast<size_t>(width * height * 3)) {
        LOGI("RGB data size mismatch.");
        return arrayListObj;
    }
    cv::Mat mat(height, width, CV_8UC3, (void *) rgbData.data());
    // 完成了数据的转换，开始检测
    std::vector<Object> objects;
    detect(mat, objects);
    __android_log_print(ANDROID_LOG_ERROR, "Yolo", "objects size:  %d", objects.size());

    draw(mat, objects);

    if (objects.size() <= 0) {

        return arrayListObj;
    }

    // 组装数据
    jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");
    if (addMethod == nullptr) {
        __android_log_print(ANDROID_LOG_ERROR, "Yolo", "Failed to find ArrayList.add method.");
        return arrayListObj;
    }

    for (size_t i = 0; i < objects.size(); i++) {
        int label_pos = objects[i].label;

        if (label_pos < 0 || label_pos >= 81) {
            __android_log_print(ANDROID_LOG_ERROR, "Yolo", "label_pos out of bounds: %d",
                                label_pos);
            continue; // 跳过无效的标签
        }
        if (class_names[label_pos] == nullptr) {
            __android_log_print(ANDROID_LOG_ERROR, "Yolo", "class_names[%d] is null.", label_pos);
            continue; // 跳过空的类名
        }

        jclass hashMapClass = env->FindClass("java/util/HashMap");
        if (hashMapClass == nullptr) {
            __android_log_print(ANDROID_LOG_ERROR, "Yolo",
                                "Failed to find java/util/HashMap class.");
            return arrayListObj;
        }
        jmethodID hashMapInit = env->GetMethodID(hashMapClass, "<init>", "()V");
        if (hashMapInit == nullptr) {
            __android_log_print(ANDROID_LOG_ERROR, "Yolo", "Failed to find HashMap constructor.");
            return arrayListObj;
        }
        jobject hashMapObj = env->NewObject(hashMapClass, hashMapInit);
        if (hashMapObj == nullptr) {
            __android_log_print(ANDROID_LOG_ERROR, "Yolo", "Failed to create HashMap object.");
            return arrayListObj;
        }

        jstring xKeyStr = charToString(env, "x");
        jstring xValue = intToString(env, objects[i].rect.x);
        jstring yKeyStr = charToString(env, "y");
        jstring yValue = intToString(env, objects[i].rect.y);
        jstring wKeyStr = charToString(env, "w");
        jstring wValue = intToString(env, objects[i].rect.width);
        jstring hKeyStr = charToString(env, "h");
        jstring hValue = intToString(env, objects[i].rect.height);
        jstring labelKeyStr = charToString(env, "label");
        jstring labelValue = intToString(env, objects[i].label);
        jstring labelStrKeyStr = charToString(env, "label_str");
        jstring labelStrValue = charToString(env, class_names[label_pos]);
        jstring probKeyStr = charToString(env, "prob");
        jstring probValue = floatToString(env, objects[i].prob);

        // 检查每个转换是否成功
        if (xKeyStr == nullptr || xValue == nullptr || yKeyStr == nullptr || yValue == nullptr ||
            wKeyStr == nullptr || wValue == nullptr || hKeyStr == nullptr || hValue == nullptr ||
            labelKeyStr == nullptr || labelValue == nullptr || labelStrKeyStr == nullptr ||
            labelStrValue == nullptr || probKeyStr == nullptr || probValue == nullptr) {
            __android_log_print(ANDROID_LOG_ERROR, "Yolo",
                                "Failed to create one of the key/value strings.");
            // 释放局部引用
            env->DeleteLocalRef(hashMapObj);
            continue;
        }

        putKeyValueToMap(env, hashMapObj, hashMapClass, xKeyStr, xValue);
        putKeyValueToMap(env, hashMapObj, hashMapClass, yKeyStr, yValue);
        putKeyValueToMap(env, hashMapObj, hashMapClass, wKeyStr, wValue);
        putKeyValueToMap(env, hashMapObj, hashMapClass, hKeyStr, hValue);
        putKeyValueToMap(env, hashMapObj, hashMapClass, labelKeyStr, labelValue);
        putKeyValueToMap(env, hashMapObj, hashMapClass, labelStrKeyStr, labelStrValue);
        putKeyValueToMap(env, hashMapObj, hashMapClass, probKeyStr, probValue);

        env->CallBooleanMethod(arrayListObj, addMethod, hashMapObj);
        if (env->ExceptionCheck()) {
            __android_log_print(ANDROID_LOG_ERROR, "Yolo",
                                "Exception occurred while adding to ArrayList.");
            env->ExceptionDescribe();
            env->ExceptionClear();
            // 选择是否继续或返回
            env->DeleteLocalRef(hashMapObj);
            continue;
        }

        // 释放局部引用
        env->DeleteLocalRef(hashMapObj);
        env->DeleteLocalRef(xKeyStr);
        env->DeleteLocalRef(xValue);
        env->DeleteLocalRef(yKeyStr);
        env->DeleteLocalRef(yValue);
        env->DeleteLocalRef(wKeyStr);
        env->DeleteLocalRef(wValue);
        env->DeleteLocalRef(hKeyStr);
        env->DeleteLocalRef(hValue);
        env->DeleteLocalRef(labelKeyStr);
        env->DeleteLocalRef(labelValue);
        env->DeleteLocalRef(labelStrKeyStr);
        env->DeleteLocalRef(labelStrValue);
        env->DeleteLocalRef(probKeyStr);
        env->DeleteLocalRef(probValue);
    }

    return arrayListObj;
}





