/****************************************************************************
*   Generated by ACUITY 5.16.3_0205
*   Match ovxlib 1.1.26
*
*   Neural Network appliction post-process header file
****************************************************************************/
#ifndef _VNN_POST_PROCESS_H_
#define _VNN_POST_PROCESS_H_

#ifdef __cplusplus
extern "C"{
#endif

vsi_status vnn_PostProcessYolov3(vsi_nn_graph_t *graph);

const vsi_nn_postprocess_map_element_t * vnn_GetPostPorcessMap();

uint32_t vnn_GetPostPorcessMapCount();

#ifdef __cplusplus
}
#endif

#endif
