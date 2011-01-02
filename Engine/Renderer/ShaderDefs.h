//
// Urho3D Engine
// Copyright (c) 2008-2011 Lasse ��rni
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#ifndef RENDERER_SHADERDEFS_H
#define RENDERER_SHADERDEFS_H

//! Vertex shader parameters
enum VSParameter
{
    VSP_AMBIENTCOLOR = 0,
    VSP_DEPTHMODE,
    VSP_ELAPSEDTIME,
    VSP_FRUSTUMSIZE,
    VSP_GBUFFEROFFSETS,
    VSP_MODEL,
    VSP_MODELVIEWPROJ,
    VSP_SHADOWPROJ,
    VSP_SPOTPROJ,
    VSP_VIEWINVERSE,
    VSP_VIEWPROJ,
    VSP_UOFFSET,
    VSP_VOFFSET,
    VSP_VIEWRIGHTVECTOR,
    VSP_VIEWUPVECTOR,
    VSP_MODELSKINMATRICES,
    VSP_MODELINSTANCES,
    MAX_VS_PARAMETERS
};

//! Pixel shader parameters
enum PSParameter
{
    PSP_AMBIENTCOLOR = 0,
    PSP_DEPTHRECONSTRUCT,
    PSP_EDGEFILTERPARAMS,
    PSP_ELAPSEDTIME,
    PSP_FOGCOLOR,
    PSP_FOGPARAMS,
    PSP_LIGHTATTEN,
    PSP_LIGHTCOLOR,
    PSP_LIGHTDIR,
    PSP_LIGHTPOS,
    PSP_LIGHTSPLITS,
    PSP_MATDIFFCOLOR,
    PSP_MATEMISSIVECOLOR,
    PSP_MATSPECPROPERTIES,
    PSP_SAMPLEOFFSETS,
    PSP_SHADOWPROJ,
    PSP_SPOTPROJ,
    MAX_PS_PARAMETERS
};

//! Texture units
enum TextureUnit
{
    TU_DIFFUSE = 0,
    TU_NORMAL = 1,
    TU_SPECULAR = 2,
    TU_DETAIL = 3,
    TU_ENVIRONMENT = 4,
    TU_EMISSIVE = 5,
    MAX_MATERIAL_TEXTURE_UNITS = 6,
    TU_DIFFBUFFER = 0,
    TU_NORMALBUFFER = 1,
    TU_DEPTHBUFFER = 2,
    TU_SHADOWMAP = 5,
    TU_LIGHTBUFFER = 6,
    TU_LIGHTRAMP = 6,
    TU_LIGHTSPOT = 7
};

//! Light vertex shader variations
enum LightVSVariation
{
    LVS_NONE = 0,
    LVS_SPOT,
    LVS_SHADOW,
    LVS_SPOTSHADOW,
    MAX_LIGHT_VS_VARIATIONS
};

//! Light pixel shader variations
enum LightPSVariation
{
    LPS_NONE = 0,
    LPS_SPEC,
    LPS_SHADOW,
    LPS_SHADOWSPEC,
    LPS_NEGATIVE,
    LPS_POINT,
    LPS_POINTSPEC,
    LPS_POINTSHADOW,
    LPS_POINTSHADOWSPEC,
    LPS_POINTNEGATIVE,
    LPS_SPOT,
    LPS_SPOTSPEC,
    LPS_SPOTSHADOW,
    LPS_SPOTSHADOWSPEC,
    LPS_SPOTNEGATIVE,
    MAX_LIGHT_PS_VARIATIONS
};

//! Deferred light volume vertex shader variations
enum DeferredLightVSVariation
{
    DLVS_NONE = 0,
    DLVS_DIR,
    DLVS_ORTHO,
    DLVS_ORTHODIR,
    MAX_DEFERRED_LIGHT_VS_VARIATIONS
};

//! Deferred light volume pixels shader variations
enum DeferredLightPSVariation
{
    DLPS_NONE = 0,
    DLPS_SPEC,
    DLPS_SHADOW,
    DLPS_SHADOWSPEC,
    DLPS_NEGATIVE,
    DLPS_POINT,
    DLPS_POINTSPEC,
    DLPS_POINTSHADOW,
    DLPS_POINTSHADOWSPEC,
    DLPS_POINTNEGATIVE,
    DLPS_SPOT,
    DLPS_SPOTSPEC,
    DLPS_SPOTSHADOW,
    DLPS_SPOTSHADOWSPEC,
    DLPS_SPOTNEGATIVE,
    DLPS_ORTHO,
    DLPS_ORTHOSPEC,
    DLPS_ORTHOSHADOW,
    DLPS_ORTHOSHADOWSPEC,
    DLPS_ORTHONEGATIVE,
    DLPS_ORTHOPOINT,
    DLPS_ORTHOPOINTSPEC,
    DLPS_ORTHOPOINTSHADOW,
    DLPS_ORTHOPOINTSHADOWSPEC,
    DLPS_ORTHOPOINTNEGATIVE,
    DLPS_ORTHOSPOT,
    DLPS_ORTHOSPOTSPEC,
    DLPS_ORTHOSPOTSHADOW,
    DLPS_ORTHOSPOTSHADOWSPEC,
    DLPS_ORTHOSPOTNEGATIVE,
    DLPS_LINEAR,
    DLPS_LINEARSPEC,
    DLPS_LINEARSHADOW,
    DLPS_LINEARSHADOWSPEC,
    DLPS_LINEARNEGATIVE,
    DLPS_LINEARPOINT,
    DLPS_LINEARPOINTSPEC,
    DLPS_LINEARPOINTSHADOW,
    DLPS_LINEARPOINTSHADOWSPEC,
    DLPS_LINEARPOINTNEGATIVE,
    DLPS_LINEARSPOT,
    DLPS_LINEARSPOTSPEC,
    DLPS_LINEARSPOTSHADOW,
    DLPS_LINEARSPOTSHADOWSPEC,
    DLPS_LINEARSPOTNEGATIVE,
    MAX_DEFERRED_LIGHT_PS_VARIATIONS
};

#endif // RENDERER_SHADERDEFS_H
