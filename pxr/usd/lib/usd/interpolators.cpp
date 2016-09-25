//
// Copyright 2016 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#include "pxr/usd/usd/interpolators.h"

#include "pxr/usd/usd/attribute.h"
#include "pxr/usd/usd/interpolation.h"
#include "pxr/usd/usd/stage.h"

#include <boost/preprocessor/seq/for_each.hpp>


#ifdef USD_ENABLE_SIMD_LERP
#include "SIMDLerp.h"

template<> void Usd_LerpArray(half *dst, double alpha, const half *lower, const half *upper, int num)
{
    ispc::LerpHalfArray((uint16_t*)dst, (const uint16_t*)lower, (const uint16_t*)upper, alpha, num);
}
template<> void Usd_LerpArray(GfVec2h *dst, double alpha, const GfVec2h *lower, const GfVec2h *upper, int num)
{
    ispc::LerpHalfArray((uint16_t*)dst, (const uint16_t*)lower, (const uint16_t*)upper, alpha, num * 2);
}
template<> void Usd_LerpArray(GfVec3h *dst, double alpha, const GfVec3h *lower, const GfVec3h *upper, int num)
{
    ispc::LerpHalfArray((uint16_t*)dst, (const uint16_t*)lower, (const uint16_t*)upper, alpha, num * 3);
}
template<> void Usd_LerpArray(GfVec4h *dst, double alpha, const GfVec4h *lower, const GfVec4h *upper, int num)
{
    ispc::LerpHalfArray((uint16_t*)dst, (const uint16_t*)lower, (const uint16_t*)upper, alpha, num * 4);
}
template<> void Usd_LerpArray(GfQuath *dst, double alpha, const GfQuath *lower, const GfQuath *upper, int num)
{
    ispc::SlerpQuathArray((ispc::quath*)dst, (const ispc::quath*)lower, (const ispc::quath*)upper, alpha, num);
}

template<> void Usd_LerpArray(float *dst, double alpha, const float *lower, const float *upper, int num)
{
    ispc::LerpFloatArray((float*)dst, (const float*)lower, (const float*)upper, alpha, num);
}
template<> void Usd_LerpArray(GfVec2f *dst, double alpha, const GfVec2f *lower, const GfVec2f *upper, int num)
{
    ispc::LerpFloatArray((float*)dst, (const float*)lower, (const float*)upper, alpha, num * 2);
}
template<> void Usd_LerpArray(GfVec3f *dst, double alpha, const GfVec3f *lower, const GfVec3f *upper, int num)
{
    ispc::LerpFloatArray((float*)dst, (const float*)lower, (const float*)upper, alpha, num * 3);
}
template<> void Usd_LerpArray(GfVec4f *dst, double alpha, const GfVec4f *lower, const GfVec4f *upper, int num)
{
    ispc::LerpFloatArray((float*)dst, (const float*)lower, (const float*)upper, alpha, num * 4);
}
template<> void Usd_LerpArray(GfQuatf *dst, double alpha, const GfQuatf *lower, const GfQuatf *upper, int num)
{
    ispc::SlerpQuatfArray((ispc::quatf*)dst, (const ispc::quatf*)lower, (const ispc::quatf*)upper, alpha, num);
}

template<> void Usd_LerpArray(double *dst, double alpha, const double *lower, const double *upper, int num)
{
    ispc::LerpDoubleArray((double*)dst, (const double*)lower, (const double*)upper, alpha, num);
}
template<> void Usd_LerpArray(GfVec2d *dst, double alpha, const GfVec2d *lower, const GfVec2d *upper, int num)
{
    ispc::LerpDoubleArray((double*)dst, (const double*)lower, (const double*)upper, alpha, num * 2);
}
template<> void Usd_LerpArray(GfVec3d *dst, double alpha, const GfVec3d *lower, const GfVec3d *upper, int num)
{
    ispc::LerpDoubleArray((double*)dst, (const double*)lower, (const double*)upper, alpha, num * 3);
}
template<> void Usd_LerpArray(GfVec4d *dst, double alpha, const GfVec4d *lower, const GfVec4d *upper, int num)
{
    ispc::LerpDoubleArray((double*)dst, (const double*)lower, (const double*)upper, alpha, num * 4);
}
template<> void Usd_LerpArray(GfQuatd *dst, double alpha, const GfQuatd *lower, const GfQuatd *upper, int num)
{
    ispc::SlerpQuatdArray((ispc::quatd*)dst, (const ispc::quatd*)lower, (const ispc::quatd*)upper, alpha, num);
}

#else

template <class T>
inline void Usd_LerpArrayGeneric(T *dst, double alpha, const T *lower, const T *upper, int num)
{
    for (int i = 0; i < num; ++i) {
        dst[i] = GfLerp(alpha, lower[i], upper[i]);
    }
}

template<> void Usd_LerpArray(half *dst, double alpha, const half *lower, const half *upper, int num)
{
    Usd_LerpArrayGeneric(dst, alpha, lower, upper, num);
}
template<> void Usd_LerpArray(GfVec2h *dst, double alpha, const GfVec2h *lower, const GfVec2h *upper, int num)
{
    Usd_LerpArrayGeneric(dst, alpha, lower, upper, num);
}
template<> void Usd_LerpArray(GfVec3h *dst, double alpha, const GfVec3h *lower, const GfVec3h *upper, int num)
{
    Usd_LerpArrayGeneric(dst, alpha, lower, upper, num);
}
template<> void Usd_LerpArray(GfVec4h *dst, double alpha, const GfVec4h *lower, const GfVec4h *upper, int num)
{
    Usd_LerpArrayGeneric(dst, alpha, lower, upper, num);
}
template<> void Usd_LerpArray(GfQuath *dst, double alpha, const GfQuath *lower, const GfQuath *upper, int num)
{
    Usd_LerpArrayGeneric(dst, alpha, lower, upper, num);
}

template<> void Usd_LerpArray(float *dst, double alpha, const float *lower, const float *upper, int num)
{
    Usd_LerpArrayGeneric(dst, alpha, lower, upper, num);
}
template<> void Usd_LerpArray(GfVec2f *dst, double alpha, const GfVec2f *lower, const GfVec2f *upper, int num)
{
    Usd_LerpArrayGeneric(dst, alpha, lower, upper, num);
}
template<> void Usd_LerpArray(GfVec3f *dst, double alpha, const GfVec3f *lower, const GfVec3f *upper, int num)
{
    Usd_LerpArrayGeneric(dst, alpha, lower, upper, num);
}
template<> void Usd_LerpArray(GfVec4f *dst, double alpha, const GfVec4f *lower, const GfVec4f *upper, int num)
{
    Usd_LerpArrayGeneric(dst, alpha, lower, upper, num);
}
template<> void Usd_LerpArray(GfQuatf *dst, double alpha, const GfQuatf *lower, const GfQuatf *upper, int num)
{
    Usd_LerpArrayGeneric(dst, alpha, lower, upper, num);
}

template<> void Usd_LerpArray(double *dst, double alpha, const double *lower, const double *upper, int num)
{
    Usd_LerpArrayGeneric(dst, alpha, lower, upper, num);
}
template<> void Usd_LerpArray(GfVec2d *dst, double alpha, const GfVec2d *lower, const GfVec2d *upper, int num)
{
    Usd_LerpArrayGeneric(dst, alpha, lower, upper, num);
}
template<> void Usd_LerpArray(GfVec3d *dst, double alpha, const GfVec3d *lower, const GfVec3d *upper, int num)
{
    Usd_LerpArrayGeneric(dst, alpha, lower, upper, num);
}
template<> void Usd_LerpArray(GfVec4d *dst, double alpha, const GfVec4d *lower, const GfVec4d *upper, int num)
{
    Usd_LerpArrayGeneric(dst, alpha, lower, upper, num);
}
template<> void Usd_LerpArray(GfQuatd *dst, double alpha, const GfQuatd *lower, const GfQuatd *upper, int num)
{
    Usd_LerpArrayGeneric(dst, alpha, lower, upper, num);
}
#endif


bool
Usd_UntypedInterpolator::Interpolate(
    const UsdAttribute& attr, 
    const SdfLayerRefPtr& layer, const SdfAbstractDataSpecId& specId,
    double time, double lower, double upper)
{
    return _Interpolate(attr, layer, specId, time, lower, upper);
}

bool 
Usd_UntypedInterpolator::Interpolate(
    const UsdAttribute& attr, 
    const Usd_ClipRefPtr& clip, const SdfAbstractDataSpecId& specId,
    double time, double lower, double upper)
{
    return _Interpolate(attr, clip, specId, time, lower, upper);
}

template <class Src>
bool 
Usd_UntypedInterpolator::_Interpolate(
    const UsdAttribute& attr, 
    const Src& src, const SdfAbstractDataSpecId& specId,
    double time, double lower, double upper)
{
    if (attr.GetStage()->GetInterpolationType() == UsdInterpolationTypeHeld) {
        return Usd_HeldInterpolator<VtValue>(_result).Interpolate(
            attr, src, specId, time, lower, upper);
    }

    // Since we're working with type-erased objects, we have no
    // choice but to do a series of runtime type checks to determine 
    // what kind of interpolation is supported for the attribute's
    // value.

    const TfType attrValueType = attr.GetTypeName().GetType();
    if (not attrValueType) {
        TF_RUNTIME_ERROR(
            "Unknown value type '%s' for attribute '%s'",
            attr.GetTypeName().GetAsToken().GetText(),
            attr.GetPath().GetString().c_str());
        return false;
    }

#define _MAKE_CLAUSE(r, unused, type)                                   \
    {                                                                   \
        static const TfType valueType = TfType::Find<type>();           \
        if (attrValueType == valueType) {                               \
            type result;                                                \
            if (Usd_LinearInterpolator<type>(&result).Interpolate(      \
                    attr, src, specId, time, lower, upper)) {           \
                *_result = result;                                      \
                return true;                                            \
            }                                                           \
            return false;                                               \
        }                                                               \
    }

    BOOST_PP_SEQ_FOR_EACH(_MAKE_CLAUSE, ~, USD_LINEAR_INTERPOLATION_TYPES)
#undef _MAKE_CLAUSE

    return Usd_HeldInterpolator<VtValue>(_result).Interpolate(
        attr, src, specId, time, lower, upper);
}
