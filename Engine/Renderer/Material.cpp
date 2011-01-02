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

#include "Precompiled.h"
#include "Log.h"
#include "Material.h"
#include "Matrix4x3.h"
#include "PixelShader.h"
#include "Profiler.h"
#include "ResourceCache.h"
#include "StringUtils.h"
#include "Texture2D.h"
#include "TextureCube.h"
#include "VertexShader.h"
#include "XMLFile.h"

#include "DebugNew.h"

MaterialPass::MaterialPass() :
    mParent(0),
    mAlphaMask(false),
    mAlphaTest(false),
    mBlendMode(BLEND_REPLACE),
    mCullMode(CULL_CCW),
    mDepthTestMode(CMP_LESSEQUAL),
    mDepthWrite(true)
{
}

MaterialPass::~MaterialPass()
{
}

void MaterialPass::setAlphaMask(bool enable)
{
    mAlphaMask = enable;
    if (mParent)
        mParent->setDirty();
}

void MaterialPass::setAlphaTest(bool enable)
{
    mAlphaTest = enable;
    if (mParent)
        mParent->setDirty();
}

void MaterialPass::setBlendMode(BlendMode mode)
{
    mBlendMode = mode;
    if (mParent)
        mParent->setDirty();
}

void MaterialPass::setCullMode(CullMode mode)
{
    mCullMode = mode;
    if (mParent)
        mParent->setDirty();
}

void MaterialPass::setDepthTestMode(CompareMode mode)
{
    mDepthTestMode = mode;
    if (mParent)
        mParent->setDirty();
}

void MaterialPass::setDepthWrite(bool enable)
{
    mDepthWrite = enable;
    if (mParent)
        mParent->setDirty();
}

void MaterialPass::setVertexShader(const std::string& name)
{
    mVertexShaderName = name;
    releaseShaders();
    if (mParent)
        mParent->setDirty();
}

void MaterialPass::setPixelShader(const std::string& name)
{
    mPixelShaderName = name;
    releaseShaders();
    if (mParent)
        mParent->setDirty();
}

void MaterialPass::releaseShaders()
{
    mVertexShaders.clear();
    mPixelShaders.clear();
}

void MaterialPass::setParent(Material* parent)
{
    mParent = parent;
}

MaterialTechnique::MaterialTechnique() :
    mParent(0),
    mQualityLevel(0),
    mLodDistance(0.0f),
    mRequireSM3(false),
    mShadersLoadedFrameNumber(M_MAX_UNSIGNED),
    mAuxViewFrameNumber(M_MAX_UNSIGNED)
{
    mTextures.resize(MAX_MATERIAL_TEXTURE_UNITS);
    
    // Setup often used defaults
    mVSParameters[VSP_UOFFSET] = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
    mVSParameters[VSP_VOFFSET] = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
    mPSParameters[PSP_MATDIFFCOLOR] = Vector4::sUnity;
    mPSParameters[PSP_MATEMISSIVECOLOR] = Vector4::sZero;
    mPSParameters[PSP_MATSPECPROPERTIES] = Vector4::sZero;
}

MaterialTechnique::~MaterialTechnique()
{
}

void MaterialTechnique::setQualityLevel(int quality)
{
    mQualityLevel = quality;
}

void MaterialTechnique::setLodDistance(float distance)
{
    mLodDistance = distance;
}

void MaterialTechnique::setRequireSM3(bool enable)
{
    mRequireSM3 = enable;
}

void MaterialTechnique::setVertexShaderParameter(VSParameter parameter, const Vector4& value)
{
    mVSParameters[parameter] = value;
}

void MaterialTechnique::setPixelShaderParameter(PSParameter parameter, const Vector4& value)
{
    mPSParameters[parameter] = value;
}

void MaterialTechnique::setTexture(TextureUnit unit, Texture* texture)
{
    if (unit >= MAX_MATERIAL_TEXTURE_UNITS)
        return;
    
    mTextures[unit] = texture;
}

void MaterialTechnique::setUVTransform(const Vector2& offset, float rotation, const Vector2& repeat)
{
    Matrix4x3 transform = Matrix4x3::sIdentity;
    transform.m00 = repeat.mX;
    transform.m11 = repeat.mY;
    transform.m03 = -0.5f * transform.m00 + 0.5f;
    transform.m13 = -0.5f * transform.m11 + 0.5f;
    
    Matrix4x3 rotationMatrix = Matrix4x3::sIdentity;
    float angleRad = rotation * M_DEGTORAD;
    rotationMatrix.m00 = cosf(angleRad);
    rotationMatrix.m01 = sinf(angleRad);
    rotationMatrix.m10 = -rotationMatrix.m01;
    rotationMatrix.m11 = rotationMatrix.m00;
    rotationMatrix.m03 = 0.5f - 0.5f * (rotationMatrix.m00 + rotationMatrix.m01);
    rotationMatrix.m13 = 0.5f - 0.5f * (rotationMatrix.m10 + rotationMatrix.m11);
    
    transform = rotationMatrix * transform;
    
    Matrix4x3 offsetMatrix = Matrix4x3::sIdentity;
    offsetMatrix.m03 = offset.mX;
    offsetMatrix.m13 = offset.mY;
    
    transform = offsetMatrix * transform;
    
    Vector4& uOffset = mVSParameters[VSP_UOFFSET];
    Vector4& vOffset = mVSParameters[VSP_VOFFSET];
    uOffset.mX = transform.m00;
    uOffset.mY = transform.m01;
    uOffset.mW = transform.m03;
    vOffset.mX = transform.m10;
    vOffset.mY = transform.m11;
    vOffset.mW = transform.m13;
}

void MaterialTechnique::setUVTransform(const Vector2& offset, float rotation, float repeat)
{
    setUVTransform(offset, rotation, Vector2(repeat, repeat));
}

void MaterialTechnique::releaseShaders()
{
    for (std::map<PassType, MaterialPass>::iterator i = mPasses.begin(); i != mPasses.end(); ++i)
        i->second.releaseShaders();
}

MaterialPass* MaterialTechnique::createPass(PassType pass)
{
    MaterialPass* existing = getPass(pass);
    if (existing)
        return existing;
    
    MaterialPass newPass;
    newPass.setParent(mParent);
    mPasses[pass] = newPass;
    
    return getPass(pass);
}

void MaterialTechnique::removePass(PassType pass)
{
    mPasses.erase(pass);
}

void MaterialTechnique::setParent(Material* parent)
{
    mParent = parent;
    
    for (std::map<PassType, MaterialPass>::iterator i = mPasses.begin(); i != mPasses.end(); ++i)
        i->second.setParent(parent);
}

void MaterialTechnique::markForAuxView(unsigned frameNumber)
{
    mAuxViewFrameNumber = frameNumber;
}

void MaterialTechnique::markShadersLoaded(unsigned frameNumber)
{
    mShadersLoadedFrameNumber = frameNumber;
}

Texture* MaterialTechnique::getTexture(TextureUnit unit) const
{
    if ((unsigned)unit >= mTextures.size())
        return 0;
    
    return mTextures[unit];
}

Material::Material(const std::string& name) :
    Resource(name),
    mDirty(true),
    mCastShadows(false),
    mOcclusion(true),
    mOcclusionCullMode(CULL_CCW)
{
    setNumTechniques(1);
}

Material::~Material()
{
}

void Material::load(Deserializer& source, ResourceCache* cache)
{
    PROFILE(Material_Load);
    
    XMLFile xml;
    xml.load(source, cache);
    
    XMLElement rootElem = xml.getRootElement();
    // Check for base material and inherit all settings, techniques and passes
    XMLElement baseElem = rootElem.getChildElement("base", false);
    if (baseElem.notNull())
    {
        Material* baseMaterial = cache->getResource<Material>(baseElem.getString("name"));
        mTechniques = baseMaterial->mTechniques;
        
        // Reparent, and release all shaders, because they most likely will be different in the new material
        for (unsigned i = 0; i < mTechniques.size(); ++i)
        {
            mTechniques[i].setParent(this);
            mTechniques[i].releaseShaders();
        }
    }
    else
        mTechniques.clear();
    
    XMLElement techniqueElem = rootElem.getChildElement("technique", false);
    unsigned index = 0;
    while (techniqueElem.notNull())
    {
        if (mTechniques.size() < index + 1)
            setNumTechniques(index + 1);
        
        MaterialTechnique& newTechnique = mTechniques[index];
        
        if (techniqueElem.hasAttribute("quality"))
            newTechnique.setQualityLevel(techniqueElem.getInt("quality"));
        if (techniqueElem.hasAttribute("loddistance"))
            newTechnique.setLodDistance(techniqueElem.getFloat("loddistance"));
        if (techniqueElem.hasAttribute("sm3"))
            newTechnique.setRequireSM3(techniqueElem.getBool("sm3"));
        
        XMLElement textureElem = techniqueElem.getChildElement("texture", false);
        while (textureElem.notNull())
        {
            TextureUnit unit = TU_DIFFUSE;
            if (textureElem.hasAttribute("unit"))
            {
                const std::string unitName = textureElem.getStringLower("unit");
                unit = MAX_MATERIAL_TEXTURE_UNITS;
                if ((unitName == "diffuse") || (unitName == "diff"))
                    unit = TU_DIFFUSE;
                if ((unitName == "normal") || (unitName == "norm"))
                    unit = TU_NORMAL;
                if ((unitName == "specular") || (unitName == "spec"))
                    unit = TU_SPECULAR;
                if (unitName == "emissive")
                    unit = TU_EMISSIVE;
                if (unitName == "detail")
                    unit = TU_DETAIL;
                if ((unitName == "environment") || (unitName == "env"))
                    unit = TU_ENVIRONMENT;
                if (unit == MAX_MATERIAL_TEXTURE_UNITS)
                    EXCEPTION("Unknown texture unit " + unitName);
            }
            std::string name = textureElem.getString("name");
            // Detect cube maps by file extension: they are defined by an XML file
            if (getExtension(name) == ".xml")
                newTechnique.setTexture(unit, cache->getResource<TextureCube>(name));
            else
                newTechnique.setTexture(unit, cache->getResource<Texture2D>(name));
            textureElem = textureElem.getNextElement("texture");
        }
        
        XMLElement parameterElem = techniqueElem.getChildElement("parameter", false);
        while (parameterElem.notNull())
        {
            std::string name = parameterElem.getString("name");
            Vector4 value = parameterElem.getVector("value");
            VSParameter vsParam = VertexShader::getParameter(name);
            if (vsParam != MAX_VS_PARAMETERS)
                newTechnique.setVertexShaderParameter(vsParam, value);
            else
            {
                PSParameter psParam = PixelShader::getParameter(name);
                if (psParam != MAX_PS_PARAMETERS)
                    newTechnique.setPixelShaderParameter(psParam, value);
                else
                    EXCEPTION("Unknown shader parameter " + name);
            }
            
            parameterElem = parameterElem.getNextElement("parameter");
        }
        
        XMLElement passElem = techniqueElem.getChildElement("pass", false);
        while (passElem.notNull())
        {
            PassType type = MAX_PASSES;
            if (passElem.hasAttribute("name"))
            {
                std::string name = passElem.getStringLower("name");
                if ((name == "deferred") || (name == "gbuffer"))
                    type = PASS_DEFERRED;
                if (name == "prepass")
                    type = PASS_PREPASS;
                if (name == "material")
                    type = PASS_MATERIAL;
                if (name == "emissive")
                    type = PASS_EMISSIVE;
                if ((name == "postopaque") || (name == "custom"))
                    type = PASS_POSTOPAQUE;
                if (name == "ambient")
                    type = PASS_AMBIENT;
                if (name == "negative")
                    type = PASS_NEGATIVE;
                if (name == "light")
                    type = PASS_LIGHT;
                if (name == "shadow")
                    type = PASS_SHADOW;
                if (type == MAX_PASSES)
                    EXCEPTION("Unknown pass " + name);
            }
            else
                EXCEPTION("Missing pass name");
            
            MaterialPass& newPass = *newTechnique.createPass(type);
            
            if (passElem.hasAttribute("vs"))
                newPass.setVertexShader(passElem.getString("vs"));
            
            if (passElem.hasAttribute("ps"))
                newPass.setPixelShader(passElem.getString("ps"));
            
            if (passElem.hasAttribute("alphamask"))
                newPass.setAlphaMask(passElem.getBool("alphamask"));
            
            if (passElem.hasAttribute("alphatest"))
                newPass.setAlphaTest(passElem.getBool("alphatest"));
            
            if (passElem.hasAttribute("blend"))
            {
                std::string blend = passElem.getStringLower("blend");
                if (blend == "replace")
                    newPass.setBlendMode(BLEND_REPLACE);
                if (blend == "add")
                    newPass.setBlendMode(BLEND_ADD);
                if (blend == "multiply")
                    newPass.setBlendMode(BLEND_MULTIPLY);
                if (blend == "alpha")
                    newPass.setBlendMode(BLEND_ALPHA);
                if (blend == "addalpha")
                    newPass.setBlendMode(BLEND_ADDALPHA);
                if (blend == "premulalpha")
                    newPass.setBlendMode(BLEND_PREMULALPHA);
                if (blend == "invdestalpha")
                    newPass.setBlendMode(BLEND_INVDESTALPHA);
            }
            
            if (passElem.hasAttribute("cull"))
            {
                std::string cull = passElem.getStringLower("cull");
                if (cull == "none")
                    newPass.setCullMode(CULL_NONE);
                if (cull == "ccw")
                    newPass.setCullMode(CULL_CCW);
                if (cull == "cw")
                    newPass.setCullMode(CULL_CW);
            }
            
            if (passElem.hasAttribute("depthtest"))
            {
                std::string depthTest = passElem.getStringLower("depthtest");
                if ((depthTest == "false") || (depthTest == "always"))
                    newPass.setDepthTestMode(CMP_ALWAYS);
                if (depthTest == "equal")
                    newPass.setDepthTestMode(CMP_EQUAL);
                if (depthTest == "less")
                    newPass.setDepthTestMode(CMP_LESS);
                if (depthTest == "lessequal")
                    newPass.setDepthTestMode(CMP_LESSEQUAL);
                if (depthTest == "greater")
                    newPass.setDepthTestMode(CMP_GREATER);
                if (depthTest == "greaterequal")
                    newPass.setDepthTestMode(CMP_GREATEREQUAL);
            }
            
            if (passElem.hasAttribute("depthwrite"))
                newPass.setDepthWrite(passElem.getBool("depthwrite"));
            
            // Undefine a pass by setting empty vertex or pixel shader name
            if ((newPass.getVertexShaderName().empty()) || (newPass.getPixelShaderName().empty()))
                newTechnique.removePass(type);
            
            passElem = passElem.getNextElement("pass");
        }
        
        techniqueElem = techniqueElem.getNextElement("technique");
        ++index;
    }
    
    // Calculate memory use
    unsigned memoryUse = 0;
    memoryUse += sizeof(Material);
    for (unsigned i = 0; i < mTechniques.size(); ++i)
    {
        MaterialTechnique& technique = mTechniques[i];
        memoryUse += sizeof(MaterialTechnique);
        memoryUse += technique.getTextures().size() * sizeof(SharedPtr<Texture>);
        memoryUse += technique.getVertexShaderParameters().size() * (sizeof(VSParameter) + sizeof(Vector4));
        memoryUse += technique.getPixelShaderParameters().size() * (sizeof(PSParameter) + sizeof(Vector4));
        
        const std::map<PassType, MaterialPass>& passes = technique.getPasses();
        for (std::map<PassType, MaterialPass>::const_iterator j = passes.begin(); j != passes.end(); ++j)
            memoryUse += sizeof(MaterialPass);
    }
    setMemoryUse(memoryUse);
    
    setDirty();
}

void Material::setNumTechniques(unsigned num)
{
    if (!num)
        return;
    
    mTechniques.resize(num);
    
    for (unsigned i = 0; i < mTechniques.size(); ++i)
        mTechniques[i].setParent(this);
}

void Material::setVertexShaderParameter(VSParameter parameter, const Vector4& value)
{
    for (unsigned i = 0; i < mTechniques.size(); ++i)
        mTechniques[i].setVertexShaderParameter(parameter, value);
}

void Material::setPixelShaderParameter(PSParameter parameter, const Vector4& value)
{
    for (unsigned i = 0; i < mTechniques.size(); ++i)
        mTechniques[i].setPixelShaderParameter(parameter, value);
}

void Material::setTexture(TextureUnit unit, Texture* texture)
{
    for (unsigned i = 0; i < mTechniques.size(); ++i)
        mTechniques[i].setTexture(unit, texture);
}

void Material::setUVTransform(const Vector2& offset, float rotation, const Vector2& repeat)
{
    for (unsigned i = 0; i < mTechniques.size(); ++i)
        mTechniques[i].setUVTransform(offset, rotation, repeat);
}

void Material::setUVTransform(const Vector2& offset, float rotation, float repeat)
{
    for (unsigned i = 0; i < mTechniques.size(); ++i)
        mTechniques[i].setUVTransform(offset, rotation, repeat);
}

void Material::releaseShaders()
{
    for (unsigned i = 0; i < mTechniques.size(); ++i)
        mTechniques[i].releaseShaders();
}

void Material::setDirty()
{
    mDirty = true;
}

SharedPtr<Material> Material::clone(const std::string& cloneName) const
{
    SharedPtr<Material> ret(new Material(cloneName));
    
    ret->mTechniques = mTechniques;
    ret->mCastShadows = mCastShadows;
    ret->mOcclusion = mOcclusion;
    ret->mOcclusionCullMode = mOcclusionCullMode;
    
    // Reparent, and release shaders from the clone, in case they will be set different
    for (unsigned i = 0; i < ret->mTechniques.size(); ++i)
    {
        ret->mTechniques[i].setParent(ret);
        ret->mTechniques[i].releaseShaders();
    }
    
    return ret;
}

MaterialTechnique* Material::getTechnique(unsigned index)
{
    if (index >= mTechniques.size())
        return 0;
    
    return &mTechniques[index];
}

MaterialPass* Material::getPass(unsigned technique, PassType pass)
{
    if (technique >= mTechniques.size())
        return 0;
    
    return mTechniques[technique].getPass(pass);
}

const std::string& getPassName(PassType pass)
{
    static const std::string passName[] =
    {
        "Deferred",
        "Prepass",
        "Material",
        "Emissive",
        "PostOpaque",
        "Ambient",
        "Negative",
        "Light"
        "Shadow"
    };
    
    return passName[pass];
}

bool Material::getCastShadows()
{
    if (mDirty)
        update();
    
    return mCastShadows;
}

bool Material::getOcclusion()
{
    if (mDirty)
        update();
    
    return mOcclusion;
}

CullMode Material::getOcclusionCullMode()
{
    if (mDirty)
        update();
    
    return mOcclusionCullMode;
}

void Material::update()
{
    // Report true if any of the material's techniques casts shadows
    mCastShadows = false;
    for (unsigned i = 0; i < mTechniques.size(); ++i)
    {
        if (mTechniques[i].hasPass(PASS_SHADOW))
        {
            mCastShadows = true;
            break;
        }
    }
    
    // Determine occlusion by checking the first pass of each technique
    mOcclusion = false;
    for (unsigned i = 0; i < mTechniques.size(); ++i)
    {
        const std::map<PassType, MaterialPass>& passes = mTechniques[i].getPasses();
        if (!passes.empty())
        {
            // If pass writes depth, enable occlusion
            const MaterialPass& pass = passes.begin()->second;
            if (pass.getDepthWrite())
            {
                mOcclusion = true;
                mOcclusionCullMode = pass.getCullMode();
                break;
            }
        }
    }
    
    mDirty = false;
}
