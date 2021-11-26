#pragma once
#include "CommonD.h"

void Immediate3D__Hook();

RwBool
rxD3D9SubmitNode(RxPipelineNodeInstance* self __RWUNUSED__,
                 const RxPipelineNodeParam* params __RWUNUSED__);