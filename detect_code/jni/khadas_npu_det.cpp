
//#define LOG_NDEBUG 0
#define LOG_TAG "khadas_npu_det"

#include <jni.h>
#include <utils/Log.h>
#include <android/log.h>
#include <cutils/properties.h>

#include "nn_detect.h"
#include "nn_detect_utils.h"
#include "nn_detect_common.h"
#include "detect_log.h"
#include <stdlib.h>
#include <string.h>


#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/core/core_c.h"

using namespace std;
using namespace cv;

static JavaVM   *gJavaVM = NULL;
static jclass    gDetectResultClass;
static jmethodID gDetectResultInitID;
int g_nn_height, g_nn_width, g_nn_channel;

#define log_info(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define log_error(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define FUNC_get_float_array(_n, _S, _c, _s, _e, _g) \
static jfloatArray get_##_n##_array(_S *s) \
{ \
   JNIEnv *env; \
   int attached = 0; \
   int ret = -1; \
   int i; \
   if (!s->_c) \
       return NULL; \
   ret = (*gJavaVM).GetEnv((void**) &env, JNI_VERSION_1_4); \
   if (ret <0) { \
       ret = (*gJavaVM).AttachCurrentThread(&env,NULL); \
       if (ret <0) { \
           log_error("callback handler:failed to attach current thread"); \
           return NULL; \
       } \
       attached = 1; \
   } \
   jfloatArray result = (*env).NewFloatArray(s->_c); \
   float *left_tmp = (float*)malloc(s->_c * sizeof(float)); \
   for (i=0; i < s->_c; i++) \
       left_tmp[i] = s->_s[i]._s._e._g; \
   (*env).SetFloatArrayRegion(result, 0, s->_c, left_tmp); \
   free(left_tmp); \
   if (attached) { \
       log_info("callback handler:detach current thread"); \
       (*gJavaVM).DetachCurrentThread(); \
   } \
   return result; \
}

#define FUNC_get_string_array(_n, _S, _c, _s, _e) \
static jobjectArray get_##_n##_array(_S *s)\
{ \
    JNIEnv *env; \
    jstring  str; \
    jobjectArray args = 0; \
    int  attached = 0; \
    char sa[64]; \
    int  i=0; \
    int  ret = -1; \
    if (!s->_c) \
        return NULL; \
    ret = (*gJavaVM).GetEnv((void**) &env, JNI_VERSION_1_4); \
    if (ret <0) { \
        ret = (*gJavaVM).AttachCurrentThread(&env,NULL); \
        if (ret <0) { \
            log_error("callback handler:failed to attach current thread"); \
            return NULL; \
        } \
        attached = 1; \
    } \
    args = (*env).NewObjectArray(s->_c,(*env).FindClass("java/lang/String"),0); \
    for ( i=0; i < s->_c; i++ ) \
    { \
        memcpy(sa, s->_s[i]._e, 64); \
        str = (*env).NewStringUTF(sa);\
        (*env).SetObjectArrayElement(args, i, str); \
    } \
    if (attached) { \
        log_info("callback handler:detach current thread"); \
        (*gJavaVM).DetachCurrentThread(); \
    } \
    return args; \
}


#define FUNC_get_int_array(_n, _S, _c, _s, _e) \
static jintArray get_##_n##_array(_S *s) \
{ \
    JNIEnv *env; \
    int attached = 0; \
    int ret = -1; \
    int i; \
    if (!s->_c) \
        return NULL; \
    ret = (*gJavaVM).GetEnv((void**) &env, JNI_VERSION_1_4); \
    if (ret <0) { \
        ret = (*gJavaVM).AttachCurrentThread(&env,NULL); \
        if (ret <0) { \
            log_error("callback handler:failed to attach current thread"); \
            return NULL; \
        } \
        attached = 1; \
    } \
    jintArray result = (*env).NewIntArray(s->_c); \
    int *pa_tmp = (int*)malloc(s->_c * sizeof(int)); \
    for (i=0; i < s->_c; i++) \
        pa_tmp[i] = s->_s[i]._e; \
    (*env).SetIntArrayRegion(result, 0, s->_c, pa_tmp); \
    free(pa_tmp); \
    if (attached) { \
        log_info("callback handler:detach current thread"); \
        (*gJavaVM).DetachCurrentThread(); \
    } \
    return result; \
}



FUNC_get_float_array(pleft, DetectResult, detect_num, point, rectPoint, left);
FUNC_get_float_array(ptop, DetectResult, detect_num, point, rectPoint, top);
FUNC_get_float_array(pright, DetectResult, detect_num, point, rectPoint, right);
FUNC_get_float_array(pbottom, DetectResult, detect_num, point, rectPoint, bottom);
FUNC_get_float_array(pscore, DetectResult, detect_num, point, rectPoint, score);

FUNC_get_string_array(plable_name, DetectResult, detect_num, result_name, lable_name);
FUNC_get_int_array(plable_id, DetectResult, detect_num, result_name, lable_id);


static jint npu_det_set_model(JNIEnv *env, jclass clazz __unused,jint modelType) {
	ALOGD("npu_det_set_models enter ");
	int ret = -1;
	int nn_height, nn_width, nn_channel;
	det_model_type type;

	switch(modelType) 
	{
		case 0:
		type = DET_YOLOFACE_V2;
		break;
		case 1:
		type = DET_YOLO_V2;
		break;
		case 2:
		type = DET_YOLO_V3;
		break;
		case 3:
		type = DET_YOLO_TINY;
		break;		
		default:
		type = DET_FACENET;
		break;

	}

	det_set_log_config(DET_DEBUG_LEVEL_DEBUG,DET_LOG_TERMINAL);
	ret = det_set_model(type);
	if(ret) {
		ALOGD("npu_det_set_model fail ");
	}
	ret = det_get_model_size(type, &nn_width, &nn_height, &nn_channel);
	if (ret) {
		ALOGD("det_get_model_size fail ");
	    return ret;
	}
	g_nn_width = nn_width;
	g_nn_height = nn_height;
	g_nn_channel = nn_channel;

	return ret;

}


static jint npu_det_get_result(JNIEnv *env, jclass clazz __unused,jobject detresult,jint modelType) {
    ALOGD("det_get_result enter  ");
	int status = -1;
	int i;
	det_model_type type;

	switch(modelType) 
	{
		case 0:
		type = DET_YOLOFACE_V2;
		break;
		case 1:
		type = DET_YOLO_V2;
		break;
		case 2:
		type = DET_YOLO_V3;
		break;
		case 3:
		type = DET_YOLO_TINY;
		break;			
		default:
		type = DET_FACENET;
		break;

	}
   
	DetectResult resultData;
	memset(&resultData,0,sizeof(resultData));
	status = det_get_result(&resultData, type);
    ALOGD("det_get_result resultData.detect_num :%d ",resultData.detect_num);

    for(i = 0;i < resultData.detect_num;i++) {
    ALOGD("det_get_result i:%d left:%f",i,resultData.point[i].point.rectPoint.left);
	ALOGD("det_get_result i:%d top:%f",i,resultData.point[i].point.rectPoint.top);
	ALOGD("det_get_result i:%d right:%f ",i,resultData.point[i].point.rectPoint.right);
	ALOGD("det_get_result i:%d bottom:%f",i,resultData.point[i].point.rectPoint.bottom);
	ALOGD("det_get_result i:%d score:%f",i,resultData.point[i].point.rectPoint.score);	
    ALOGD("det_get_result i:%d lable_id:%d lable_name:%s",i,resultData.result_name[i].lable_id,resultData.result_name[i].lable_name);
    }


    (*env).SetIntField(detresult,\
            (*env).GetFieldID(gDetectResultClass, "detect_num", "I"),  resultData.detect_num);
    (*env).SetObjectField(detresult,\
            (*env).GetFieldID(gDetectResultClass, "left", "[F"), get_pleft_array(&resultData));
	(*env).SetObjectField(detresult,\
            (*env).GetFieldID(gDetectResultClass, "top", "[F"), get_ptop_array(&resultData));
	(*env).SetObjectField(detresult,\
            (*env).GetFieldID(gDetectResultClass, "right", "[F"), get_pright_array(&resultData));
	(*env).SetObjectField(detresult,\
            (*env).GetFieldID(gDetectResultClass, "bottom", "[F"), get_pbottom_array(&resultData));
	(*env).SetObjectField(detresult,\
            (*env).GetFieldID(gDetectResultClass, "score", "[F"), get_pscore_array(&resultData));
	(*env).SetObjectField(detresult,\
            (*env).GetFieldID(gDetectResultClass, "lable_name", "[Ljava/lang/String;"), get_plable_name_array(&resultData));
	(*env).SetObjectField(detresult,\
            (*env).GetFieldID(gDetectResultClass, "lable_id", "[I"), get_plable_id_array(&resultData));


	ALOGD("npu_det_get_result status is %d", status);
	return status;

}

static jint npu_det_set_input(JNIEnv *env, jclass clazz __unused,jbyteArray imgbyte,jint pixel_format,jint width,jint height,jint channel,jint modelType) {
	ALOGD("npu_det_set_input v2 enter 11 ");
	unsigned char* data = NULL;
	IplImage *frame2process = NULL;
	input_image_t image;
	int ret = -1;
	jint length = 0;
	det_model_type type;

	cv::Mat yolo_Image(g_nn_height, g_nn_width, CV_8UC3);


	if (NULL != imgbyte) {
		 length = (*env).GetArrayLength(imgbyte);


		if (0 < length) {
			data = (unsigned char *)malloc(length);
			(*env).GetByteArrayRegion(imgbyte, (jint)0, length, (jbyte *) data);

		}
	}

	frame2process = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
    if (frame2process == NULL)
    {
		ALOGD("cvCreateImage fail ");
		
    }
    if(frame2process->width != 1920)
    {
        ALOGD("read image not 1920 width ");
		
    }

	memcpy(frame2process->imageData,data,length);
	if(data) {
		
		free(data);
		
	}
	if(frame2process->imageData) {
		
    	free(frame2process->imageData);
		
	}
	cv::Mat sourceFrame = cv::cvarrToMat(frame2process);
    cv::resize(sourceFrame, yolo_Image, yolo_Image.size());
    
	image.data      = yolo_Image.data;
	image.width     = yolo_Image.cols;
	image.height    = yolo_Image.rows;
	image.channel   = yolo_Image.channels();
	image.pixel_format = PIX_FMT_RGB888;
	
	switch(modelType) 
	{
		case 0:
		type = DET_YOLOFACE_V2;
		break;
		case 1:
		type = DET_YOLO_V2;
		break;
		case 2:
		type = DET_YOLO_V3;
		break;
		case 3:
		type = DET_YOLO_TINY;
		break;			
		default:
		type = DET_FACENET;
		break;

	}

	ret = det_set_input(image, type);
	if(ret) {

		ALOGD("npu_det_set_input fail ");
	}
	return ret;
}

static JNINativeMethod sMethods[] = {
    {"native_npu_det_get_result", "(Lcom/khadas/npudemo/DetectResult;I)I", (void*)npu_det_get_result},
    {"native_npu_det_set_input", "([BIIIII)I", (void*)npu_det_set_input},
    {"native_npu_det_set_model", "(I)I", (void*)npu_det_set_model},
};

#ifndef NELEM
# define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif

static int registerNativeMethods(JNIEnv* env, const char* className,
                                 const JNINativeMethod* methods, int numMethods)
{
    int rc;
    jclass clazz;

    clazz = (*env).FindClass(className);

    if (clazz == NULL)
        return -1;

    if ((rc = ((*env).RegisterNatives(clazz, methods, numMethods))) < 0)
        return -1;

    return 0;
}


JNIEXPORT jint
JNI_OnLoad(JavaVM* vm, void* reserved __unused)
{
    JNIEnv* env = NULL;
    jclass clazz;

    if ((*vm).GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK)
        return -1;

    if (registerNativeMethods(env,"com/khadas/npudemo/KhadasNpuManager", sMethods, NELEM(sMethods)) < 0)
    return -1;

    gJavaVM = vm;

    clazz = (*env).FindClass("com/khadas/npudemo/KhadasNpuManager");
    if (clazz == NULL) {
        log_error("FindClass com/khadas/npudemo/KhadasNpuManager failed");
        return -1;
    }

	gDetectResultClass	= (*env).FindClass("com/khadas/npudemo/DetectResult");
	  if (gDetectResultClass == NULL) {
        log_error("FindClass com/khadas/npudemo/DetectResult failed");
        return -1;
    }
   log_error("FindClass com/example/khadasnpu/DetectResult sucess");
    gDetectResultClass   = (jclass)(*env).NewGlobalRef((jobject)gDetectResultClass);
    gDetectResultInitID  = (*env).GetMethodID(gDetectResultClass, "<init>", "()V");
    log_error("gDetectResultInitID %d",gDetectResultInitID);
    return JNI_VERSION_1_4;
}





