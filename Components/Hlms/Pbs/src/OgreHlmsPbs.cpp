/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2014 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#include "OgreStableHeaders.h"

#include "OgreHlmsPbs.h"
#include "OgreHlmsPbsDatablock.h"

#include "OgreViewport.h"
#include "OgreRenderTarget.h"
#include "OgreHighLevelGpuProgramManager.h"
#include "OgreHighLevelGpuProgram.h"

#include "OgreSceneManager.h"
#include "Compositor/OgreCompositorShadowNode.h"
#include "Vao/OgreVaoManager.h"
#include "Vao/OgreConstBufferPacked.h"
#include "Vao/OgreTexBufferPacked.h"
#include "Vao/OgreStagingBuffer.h"

#include "CommandBuffer/OgreCommandBuffer.h"
#include "CommandBuffer/OgreCbTexture.h"
#include "CommandBuffer/OgreCbShaderBuffer.h"

namespace Ogre
{
    const IdString PbsProperty::HwGammaRead       = IdString( "hw_gamma_read" );
    const IdString PbsProperty::HwGammaWrite      = IdString( "hw_gamma_write" );
    const IdString PbsProperty::SignedIntTex      = IdString( "signed_int_textures" );

    const IdString PbsProperty::NumTextures       = IdString( "num_textures" );
    const IdString PbsProperty::DiffuseMap        = IdString( "diffuse_map" );
    const IdString PbsProperty::NormalMapTex      = IdString( "normal_map_tex" );
    const IdString PbsProperty::SpecularMap       = IdString( "specular_map" );
    const IdString PbsProperty::RoughnessMap      = IdString( "roughness_map" );
    const IdString PbsProperty::EnvProbeMap       = IdString( "envprobe_map" );
    const IdString PbsProperty::DetailWeightMap   = IdString( "detail_weight_map" );

    const IdString PbsProperty::NormalMap         = IdString( "normal_map" );

    const IdString PbsProperty::FresnelScalar     = IdString( "fresnel_scalar" );

    const IdString PbsProperty::NormalWeight          = IdString( "normal_weight" );
    const IdString PbsProperty::NormalWeightTex       = IdString( "normal_weight_tex" );
    const IdString PbsProperty::NormalWeightDetail0   = IdString( "normal_weight_detail0" );
    const IdString PbsProperty::NormalWeightDetail1   = IdString( "normal_weight_detail1" );
    const IdString PbsProperty::NormalWeightDetail2   = IdString( "normal_weight_detail2" );
    const IdString PbsProperty::NormalWeightDetail3   = IdString( "normal_weight_detail3" );

    const IdString PbsProperty::DetailWeights     = IdString( "detail_weights" );
    const IdString PbsProperty::DetailOffsetsD    = IdString( "detail_offsetsD" );
    const IdString PbsProperty::DetailOffsetsD0   = IdString( "detail_offsetsD0" );
    const IdString PbsProperty::DetailOffsetsD1   = IdString( "detail_offsetsD1" );
    const IdString PbsProperty::DetailOffsetsD2   = IdString( "detail_offsetsD2" );
    const IdString PbsProperty::DetailOffsetsD3   = IdString( "detail_offsetsD3" );
    const IdString PbsProperty::DetailOffsetsN    = IdString( "detail_offsetsN" );
    const IdString PbsProperty::DetailOffsetsN0   = IdString( "detail_offsetsN0" );
    const IdString PbsProperty::DetailOffsetsN1   = IdString( "detail_offsetsN1" );
    const IdString PbsProperty::DetailOffsetsN2   = IdString( "detail_offsetsN2" );
    const IdString PbsProperty::DetailOffsetsN3   = IdString( "detail_offsetsN3" );

    const IdString PbsProperty::UvDiffuse         = IdString( "uv_diffuse" );
    const IdString PbsProperty::UvNormal          = IdString( "uv_normal" );
    const IdString PbsProperty::UvSpecular        = IdString( "uv_specular" );
    const IdString PbsProperty::UvRoughness       = IdString( "uv_roughness" );
    const IdString PbsProperty::UvDetailWeight    = IdString( "uv_detail_weight" );

    const IdString PbsProperty::UvDetail0         = IdString( "uv_detail0" );
    const IdString PbsProperty::UvDetail1         = IdString( "uv_detail1" );
    const IdString PbsProperty::UvDetail2         = IdString( "uv_detail2" );
    const IdString PbsProperty::UvDetail3         = IdString( "uv_detail3" );

    const IdString PbsProperty::UvDetailNm0       = IdString( "uv_detail_nm0" );
    const IdString PbsProperty::UvDetailNm1       = IdString( "uv_detail_nm1" );
    const IdString PbsProperty::UvDetailNm2       = IdString( "uv_detail_nm2" );
    const IdString PbsProperty::UvDetailNm3       = IdString( "uv_detail_nm3" );

    const IdString PbsProperty::BlendModeIndex0   = IdString( "blend_mode_idx0" );
    const IdString PbsProperty::BlendModeIndex1   = IdString( "blend_mode_idx1" );
    const IdString PbsProperty::BlendModeIndex2   = IdString( "blend_mode_idx2" );
    const IdString PbsProperty::BlendModeIndex3   = IdString( "blend_mode_idx3" );

    const IdString PbsProperty::DetailMapsDiffuse = IdString( "detail_maps_diffuse" );
    const IdString PbsProperty::DetailMapsNormal  = IdString( "detail_maps_normal" );

    const IdString PbsProperty::DetailDiffuseSwizzle0 = IdString( "detail_diffuse_swizzle0" );
    const IdString PbsProperty::DetailDiffuseSwizzle1 = IdString( "detail_diffuse_swizzle1" );
    const IdString PbsProperty::DetailDiffuseSwizzle2 = IdString( "detail_diffuse_swizzle2" );
    const IdString PbsProperty::DetailDiffuseSwizzle3 = IdString( "detail_diffuse_swizzle3" );

    const IdString PbsProperty::DetailNormalSwizzle0  = IdString( "detail_normal_swizzle0" );
    const IdString PbsProperty::DetailNormalSwizzle1  = IdString( "detail_normal_swizzle1" );
    const IdString PbsProperty::DetailNormalSwizzle2  = IdString( "detail_normal_swizzle2" );
    const IdString PbsProperty::DetailNormalSwizzle3  = IdString( "detail_normal_swizzle3" );

    const IdString *PbsProperty::UvSourcePtrs[NUM_PBSM_SOURCES] =
    {
        &PbsProperty::UvDiffuse,
        &PbsProperty::UvNormal,
        &PbsProperty::UvSpecular,
        &PbsProperty::UvRoughness,
        &PbsProperty::UvDetailWeight,
        &PbsProperty::UvDetail0,
        &PbsProperty::UvDetail1,
        &PbsProperty::UvDetail2,
        &PbsProperty::UvDetail3,
        &PbsProperty::UvDetailNm0,
        &PbsProperty::UvDetailNm1,
        &PbsProperty::UvDetailNm2,
        &PbsProperty::UvDetailNm3
    };

    const IdString *PbsProperty::DetailDiffuseSwizzles[4] =
    {
        &PbsProperty::DetailDiffuseSwizzle0,
        &PbsProperty::DetailDiffuseSwizzle1,
        &PbsProperty::DetailDiffuseSwizzle2,
        &PbsProperty::DetailDiffuseSwizzle3
    };

    const IdString *PbsProperty::DetailNormalSwizzles[4] =
    {
        &PbsProperty::DetailNormalSwizzle0,
        &PbsProperty::DetailNormalSwizzle1,
        &PbsProperty::DetailNormalSwizzle2,
        &PbsProperty::DetailNormalSwizzle3
    };

    const IdString *PbsProperty::DetailNormalWeights[4] =
    {
        &PbsProperty::NormalWeightDetail0,
        &PbsProperty::NormalWeightDetail1,
        &PbsProperty::NormalWeightDetail2,
        &PbsProperty::NormalWeightDetail3
    };

    const IdString *PbsProperty::DetailOffsetsDPtrs[4] =
    {
        &PbsProperty::DetailOffsetsD0,
        &PbsProperty::DetailOffsetsD1,
        &PbsProperty::DetailOffsetsD2,
        &PbsProperty::DetailOffsetsD3
    };

    const IdString *PbsProperty::DetailOffsetsNPtrs[4] =
    {
        &PbsProperty::DetailOffsetsN0,
        &PbsProperty::DetailOffsetsN1,
        &PbsProperty::DetailOffsetsN2,
        &PbsProperty::DetailOffsetsN3
    };

    const IdString *PbsProperty::BlendModes[4] =
    {
        &PbsProperty::BlendModeIndex0,
        &PbsProperty::BlendModeIndex1,
        &PbsProperty::BlendModeIndex2,
        &PbsProperty::BlendModeIndex3
    };

    extern const String c_pbsBlendModes[];

    HlmsPbs::HlmsPbs( Archive *dataFolder ) :
        Hlms( HLMS_PBS, "pbs", dataFolder ),
        ConstBufferPool( 68, 16 * 1024 * 1024, 0 ),
        mCurrentPassBuffer( 0 ),
        mCurrentConstBuffer( 0 ),
        mCurrentTexBuffer( 0 ),
        mStartMappedConstBuffer( 0 ),
        mCurrentMappedConstBuffer( 0 ),
        mCurrentConstBufferSize( 0 ),
        mStartMappedTexBuffer( 0 ),
        mCurrentMappedTexBuffer( 0 ),
        mCurrentTexBufferSize( 0 ),
        mTexLastOffset( 0 ),
        mLastTexBufferCmdOffset( (size_t)~0 ),
        mLastTextureHash( 0 )
    {
    }
    //-----------------------------------------------------------------------------------
    HlmsPbs::~HlmsPbs()
    {
        {
            TexBufferPackedVec::const_iterator itor = mTexBuffers.begin();
            TexBufferPackedVec::const_iterator end  = mTexBuffers.end();

            while( itor != end )
                mVaoManager->destroyTexBuffer( *itor++ );

            mTexBuffers.clear();
        }

        {
            ConstBufferPackedVec::const_iterator itor = mConstBuffers.begin();
            ConstBufferPackedVec::const_iterator end  = mConstBuffers.end();

            while( itor != end )
                mVaoManager->destroyConstBuffer( *itor++ );

            mConstBuffers.clear();

            itor = mPassBuffers.begin();
            end  = mPassBuffers.end();

            while( itor != end )
                mVaoManager->destroyConstBuffer( *itor++ );

            mPassBuffers.clear();
        }
    }
    //-----------------------------------------------------------------------------------
    void HlmsPbs::_changeRenderSystem( RenderSystem *newRs )
    {
        if( mVaoManager )
            destroyAllBuffers();

        Hlms::_changeRenderSystem( newRs );
        ConstBufferPool::_changeRenderSystem( newRs );

        if( newRs )
        {
            //mPassBuffer = mVaoManager->createConstBuffer( , BT_DYNAMIC, 0, false );

            HlmsDatablockMap::const_iterator itor = mDatablocks.begin();
            HlmsDatablockMap::const_iterator end  = mDatablocks.end();

            while( itor != end )
            {
                assert( dynamic_cast<HlmsPbsDatablock*>( itor->second.datablock ) );
                HlmsPbsDatablock *datablock = static_cast<HlmsPbsDatablock*>( itor->second.datablock );

                scheduleForUpdate( datablock );
                ++itor;
            }
        }
    }
    //-----------------------------------------------------------------------------------
    const HlmsCache* HlmsPbs::createShaderCacheEntry( uint32 renderableHash,
                                                            const HlmsCache &passCache,
                                                            uint32 finalHash,
                                                            const QueuedRenderable &queuedRenderable )
    {
        const HlmsCache *retVal = Hlms::createShaderCacheEntry( renderableHash, passCache, finalHash,
                                                                queuedRenderable );

        //Set samplers.
        GpuProgramParametersSharedPtr psParams = retVal->pixelShader->getDefaultParameters();

        int texUnit = 1; //Vertex shader consumes 1 slot with its tbuffer.
        if( !mPreparedPass.shadowMaps.empty() )
        {
            vector<int>::type shadowMaps;
            shadowMaps.reserve( mPreparedPass.shadowMaps.size() );
            for( texUnit=0; texUnit<(int)mPreparedPass.shadowMaps.size(); ++texUnit )
                shadowMaps.push_back( texUnit );

            psParams->setNamedConstant( "texShadowMap", &shadowMaps[0], shadowMaps.size(), 1 );
        }

        assert( dynamic_cast<const HlmsPbsDatablock*>( queuedRenderable.renderable->getDatablock() ) );
        const HlmsPbsDatablock *datablock = static_cast<const HlmsPbsDatablock*>(
                                                    queuedRenderable.renderable->getDatablock() );

        int numTextures = getProperty( PbsProperty::NumTextures );
        for( int i=0; i<numTextures; ++i )
        {
            psParams->setNamedConstant( "textureMaps[" + StringConverter::toString( i ) + "]",
                                        texUnit++ );
        }

        if( getProperty( PbsProperty::EnvProbeMap ) )
        {
            assert( !datablock->getTexture( PBSM_REFLECTION ).isNull() );
            psParams->setNamedConstant( "texEnvProbeMap", texUnit++ );
        }

        mRenderSystem->_setProgramsFromHlms( retVal );

        GpuProgramParametersSharedPtr vpParams = retVal->vertexShader->getDefaultParameters();
        mRenderSystem->bindGpuProgramParameters( GPT_VERTEX_PROGRAM, vpParams, GPV_ALL );
        mRenderSystem->bindGpuProgramParameters( GPT_FRAGMENT_PROGRAM, psParams, GPV_ALL );

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void HlmsPbs::setDetailMapProperties( bool diffuseMaps, HlmsPbsDatablock *datablock,
                                                PiecesMap *inOutPieces )
    {
        PbsTextureTypes detailTextureStart = diffuseMaps ? PBSM_DETAIL0 : PBSM_DETAIL0_NM;
        const IdString **detailSwizzles = diffuseMaps ? PbsProperty::DetailDiffuseSwizzles :
                                                        PbsProperty::DetailNormalSwizzles;

        size_t validDetailMaps = 0;
        for( size_t i=0; i<4; ++i )
        {
            uint8 blendMode = datablock->mBlendModes[i];

            //If Detail map 0 doesn't exists but Detail map 1 does;
            //then DetailDiffuseSwizzle0 must reference the swizzle 'y'
            //Same happens with the UV sources (the UV sources[1] end up
            //actually as sources[0], etc).
            if( !datablock->getTexture( detailTextureStart + i ).isNull() )
            {
                if( diffuseMaps )
                {
                    inOutPieces[PixelShader][*PbsProperty::BlendModes[validDetailMaps]] =
                                                "@insertpiece( " + c_pbsBlendModes[blendMode] + ")";
                }

                const char *swizzles[4] = { "x", "y", "z", "w" };
                IdString swizzleN = *detailSwizzles[validDetailMaps];
                inOutPieces[PixelShader][swizzleN] = swizzles[i];

                uint8 uvSource = datablock->mUvSource[detailTextureStart + i];
                setProperty( *PbsProperty::UvSourcePtrs[detailTextureStart + validDetailMaps],
                             uvSource );

                if( getProperty( *HlmsBaseProp::UvCountPtrs[uvSource] ) < 2 )
                {
                    OGRE_EXCEPT( Exception::ERR_INVALID_STATE,
                                 "Renderable needs at least 2 coordinates in UV set #" +
                                 StringConverter::toString( uvSource ) +
                                 ". Either change the mesh, or change the UV source settings",
                                 "HlmsPbs::setDetailMapProperties" );
                }

                ++validDetailMaps;
            }
        }

        setProperty( diffuseMaps ? PbsProperty::DetailMapsDiffuse :
                                   PbsProperty::DetailMapsNormal, validDetailMaps );
    }
    //-----------------------------------------------------------------------------------
    void HlmsPbs::calculateHashForPreCreate( Renderable *renderable, PiecesMap *inOutPieces )
    {
        assert( dynamic_cast<HlmsPbsDatablock*>( renderable->getDatablock() ) );
        HlmsPbsDatablock *datablock = static_cast<HlmsPbsDatablock*>(
                                                        renderable->getDatablock() );
        setProperty( PbsProperty::FresnelScalar, datablock->hasSeparateFresnel() );

        for( size_t i=0; i<PBSM_DETAIL0; ++i )
        {
            uint8 uvSource = datablock->mUvSource[i];
            setProperty( *PbsProperty::UvSourcePtrs[i], uvSource );

            if( !datablock->getTexture( i ).isNull() &&
                getProperty( *HlmsBaseProp::UvCountPtrs[uvSource] ) < 2 )
            {
                OGRE_EXCEPT( Exception::ERR_INVALID_STATE,
                             "Renderable needs at least 2 coordinates in UV set #" +
                             StringConverter::toString( uvSource ) +
                             ". Either change the mesh, or change the UV source settings",
                             "HlmsPbs::calculateHashForPreCreate" );
            }
        }

        int numNormalWeights = 0;
        if( datablock->getNormalMapWeight() != 1.0f && !datablock->getTexture( PBSM_NORMAL ).isNull() )
        {
            setProperty( PbsProperty::NormalWeightTex, 1 );
            ++numNormalWeights;
        }

        {
            size_t validDetailMaps = 0;
            for( size_t i=0; i<4; ++i )
            {
                if( !datablock->getTexture( PBSM_DETAIL0_NM + i ).isNull() )
                {
                    if( datablock->getDetailNormalWeight( i ) != 1.0f )
                    {
                        setProperty( *PbsProperty::DetailNormalWeights[validDetailMaps], 1 );
                        ++numNormalWeights;
                    }

                    ++validDetailMaps;
                }
            }
        }

        setProperty( PbsProperty::NormalWeight, numNormalWeights );

        setDetailMapProperties( true, datablock, inOutPieces );
        setDetailMapProperties( false, datablock, inOutPieces );

        {
            bool anyDetailWeight = false;
            for( size_t i=0; i<4 && !anyDetailWeight; ++i )
            {
                if( datablock->mDetailWeight[i] != 1.0f &&
                    (!datablock->getTexture( PBSM_DETAIL0 + i ).isNull() ||
                     !datablock->getTexture( PBSM_DETAIL0_NM + i ).isNull()) )
                {
                    anyDetailWeight = true;
                }
            }

            if( anyDetailWeight )
                setProperty( PbsProperty::DetailWeights, 1 );
        }

        {
            int numOffsets = 0;
            size_t validDetailMaps = 0;
            for( size_t i=0; i<4; ++i )
            {
                if( datablock->mDetailsOffsetScale[i] != Vector4( 0, 0, 1, 1 ) )
                {
                    setProperty( *PbsProperty::DetailOffsetsDPtrs[validDetailMaps], 1 );
                    ++numOffsets;
                }

                if( !datablock->getTexture( PBSM_DETAIL0 + i ).isNull() )
                    ++validDetailMaps;
            }

            setProperty( PbsProperty::DetailOffsetsD, numOffsets );

            numOffsets = 0;
            validDetailMaps = 0;
            for( size_t i=0; i<4; ++i )
            {
                if( datablock->mDetailsOffsetScale[i+4] != Vector4( 0, 0, 1, 1 ) )
                {
                    setProperty( *PbsProperty::DetailOffsetsNPtrs[validDetailMaps], 1 );
                    ++numOffsets;
                }

                if( !datablock->getTexture( PBSM_DETAIL0_NM + i ).isNull() )
                    ++validDetailMaps;
            }

            setProperty( PbsProperty::DetailOffsetsN, numOffsets );
        }

        setProperty( PbsProperty::DiffuseMap,     !datablock->getTexture( PBSM_DIFFUSE ).isNull() );
        setProperty( PbsProperty::NormalMapTex,   !datablock->getTexture( PBSM_NORMAL ).isNull() );
        setProperty( PbsProperty::SpecularMap,    !datablock->getTexture( PBSM_SPECULAR ).isNull() );
        setProperty( PbsProperty::RoughnessMap,   !datablock->getTexture( PBSM_ROUGHNESS ).isNull() );
        setProperty( PbsProperty::EnvProbeMap,    !datablock->getTexture( PBSM_REFLECTION ).isNull() );
        setProperty( PbsProperty::DetailWeightMap,!datablock->getTexture( PBSM_DETAIL_WEIGHT ).isNull() );

        bool usesNormalMap = !datablock->getTexture( PBSM_NORMAL ).isNull();
        for( size_t i=PBSM_DETAIL0_NM; i<=PBSM_DETAIL3_NM; ++i )
            usesNormalMap |= !datablock->getTexture( i ).isNull();
        setProperty( PbsProperty::NormalMap, usesNormalMap );

        /*setProperty( HlmsBaseProp::, !datablock->getTexture( PBSM_DETAIL0 ).isNull() );
        setProperty( HlmsBaseProp::DiffuseMap, !datablock->getTexture( PBSM_DETAIL1 ).isNull() );*/
        bool normalMapCanBeSupported = (getProperty( HlmsBaseProp::Normal ) &&
                                        getProperty( HlmsBaseProp::Tangent )) ||
                                        getProperty( HlmsBaseProp::QTangent );

        if( !normalMapCanBeSupported && usesNormalMap )
        {
            OGRE_EXCEPT( Exception::ERR_INVALID_STATE,
                         "Renderable can't use normal maps but datablock wants normal maps. "
                         "Generate Tangents for this mesh to fix the problem or use a "
                         "datablock without normal maps.", "HlmsPbs::calculateHashForPreCreate" );
        }
    }
    //-----------------------------------------------------------------------------------
    void HlmsPbs::calculateHashForPreCaster( Renderable *renderable, PiecesMap *inOutPieces )
    {
        //HlmsPbsDatablock *datablock = static_cast<HlmsPbsDatablock*>(
        //                                              renderable->getDatablock() );

        HlmsPropertyVec::iterator itor = mSetProperties.begin();
        HlmsPropertyVec::iterator end  = mSetProperties.end();

        while( itor != end )
        {
            if( itor->keyName != PbsProperty::HwGammaRead &&
                itor->keyName != PbsProperty::UvDiffuse &&
                itor->keyName != HlmsBaseProp::Skeleton &&
                itor->keyName != HlmsBaseProp::BonesPerVertex &&
                itor->keyName != HlmsBaseProp::DualParaboloidMapping &&
                itor->keyName != HlmsBaseProp::AlphaTest )
            {
                itor->value = 0;
            }

            ++itor;
        }
    }
    //-----------------------------------------------------------------------------------
    void HlmsPbs::uploadDirtyDatablocks(void)
    {
        if( mDirtyUsers.empty() )
            return;

        std::sort( mDirtyUsers.begin(), mDirtyUsers.end(), OrderConstBufferPoolUserByPoolThenSlot );

        size_t uploadSize = HlmsPbsDatablock::MaterialSizeInGpuAligned * mDirtyUsers.size();
        StagingBuffer *stagingBuffer = mVaoManager->getStagingBuffer( uploadSize, true );

        StagingBuffer::DestinationVec destinations;

        ConstBufferPoolUserVec::const_iterator itor = mDirtyUsers.begin();
        ConstBufferPoolUserVec::const_iterator end  = mDirtyUsers.end();

        char *bufferStart = reinterpret_cast<char*>( stagingBuffer->map( uploadSize ) );
        char *data = bufferStart;

        while( itor != end )
        {
            HlmsPbsDatablock *datablock = static_cast<HlmsPbsDatablock*>(*itor);

            size_t srcOffset = static_cast<size_t>( data - bufferStart );
            size_t dstOffset = datablock->getAssignedSlot() * HlmsPbsDatablock::MaterialSizeInGpuAligned;

            data = datablock->uploadToConstBuffer( data );

            StagingBuffer::Destination dst( datablock->getAssignedPool()->materialBuffer, dstOffset,
                                            srcOffset, HlmsPbsDatablock::MaterialSizeInGpuAligned );

            if( !destinations.empty() )
            {
                StagingBuffer::Destination &lastElement = destinations.back();

                if( lastElement.destination == dst.destination &&
                    (lastElement.dstOffset + lastElement.length == dst.dstOffset) )
                {
                    lastElement.length += dst.length;
                }
                else
                {
                    destinations.push_back( dst );
                }
            }
            else
            {
                destinations.push_back( dst );
            }

            ++itor;
        }

        stagingBuffer->unmap( destinations );
        stagingBuffer->removeReferenceCount();
    }
    //-----------------------------------------------------------------------------------
    HlmsCache HlmsPbs::preparePassHash( const CompositorShadowNode *shadowNode, bool casterPass,
                                        bool dualParaboloid, SceneManager *sceneManager )
    {
        HlmsCache retVal = Hlms::preparePassHash( shadowNode, casterPass, dualParaboloid, sceneManager );

        RenderTarget *renderTarget = sceneManager->getCurrentViewport()->getTarget();

        const RenderSystemCapabilities *capabilities = mRenderSystem->getCapabilities();
        setProperty( PbsProperty::HwGammaRead, capabilities->hasCapability( RSC_HW_GAMMA ) );
        setProperty( PbsProperty::HwGammaWrite, capabilities->hasCapability( RSC_HW_GAMMA ) &&
                                                        renderTarget->isHardwareGammaEnabled() );
        setProperty( PbsProperty::SignedIntTex, capabilities->hasCapability(
                                                            RSC_TEXTURE_SIGNED_INT ) );

        retVal.setProperties = mSetProperties;

        Camera *camera = sceneManager->getCameraInProgress();
        Matrix4 viewMatrix = camera->getViewMatrix(true);

        Matrix4 projectionMatrix = camera->getProjectionMatrixWithRSDepth();

        if( renderTarget->requiresTextureFlipping() )
        {
            projectionMatrix[1][0] = -projectionMatrix[1][0];
            projectionMatrix[1][1] = -projectionMatrix[1][1];
            projectionMatrix[1][2] = -projectionMatrix[1][2];
            projectionMatrix[1][3] = -projectionMatrix[1][3];
        }

        int32 numLights             = getProperty( HlmsBaseProp::LightsSpot );
        int32 numDirectionalLights  = getProperty( HlmsBaseProp::LightsDirectional );
        int32 numShadowMaps         = getProperty( HlmsBaseProp::NumShadowMaps );
        int32 numPssmSplits         = getProperty( HlmsBaseProp::PssmSplits );

        size_t mapSize = 16 * 4;

        if( !casterPass )
        {
            mapSize += ( 16 + (16 + 2 + 2) * numShadowMaps + 9 ) * 4;
            mapSize += numPssmSplits * 4;
            mapSize = alignToNextMultiple( mapSize, 16 );

            if( shadowNode )
                mapSize += ( 7 * 4 * 4 ) * numLights;
            else
                mapSize += ( 3 * 4 * 4 ) * numDirectionalLights;
        }
        else
        {
            mapSize += (2 + 2) * 4;
        }

        //Arbitrary 16kb (minimum supported by GL), should be enough.
        const size_t maxBufferSize = 16 * 1024 * 1024;

        assert( mapSize <= maxBufferSize );

        if( mCurrentPassBuffer >= mPassBuffers.size() )
        {
            mPassBuffers.push_back( mVaoManager->createConstBuffer( maxBufferSize, BT_DYNAMIC,
                                                                    0, false ) );
        }

        ConstBufferPacked *passBuffer = mPassBuffers[mCurrentPassBuffer++];
        float *passBufferPtr = reinterpret_cast<float*>( passBuffer->map( 0, mapSize,
                                                                          MS_PERSISTENT_INCOHERENT ) );

#ifndef NDEBUG
        const float *startupPtr = passBufferPtr;
#endif

        //---------------------------------------------------------------------------
        //                          ---- VERTEX SHADER ----
        //---------------------------------------------------------------------------

        //mat4 viewProj;
        Matrix4 viewProjMatrix = projectionMatrix * viewMatrix;
        for( size_t i=0; i<16; ++i )
            *passBufferPtr++ = (float)viewProjMatrix[0][i];

        if( !casterPass )
        {
            //mat4 view;
            for( size_t i=0; i<16; ++i )
                *passBufferPtr++ = (float)viewMatrix[0][i];

            for( int32 i=0; i<numShadowMaps; ++i )
            {
                //mat4 shadowRcv[numShadowMaps].texWorldViewProj
                Matrix4 viewProjTex = shadowNode->getViewProjectionMatrix( i );
                for( size_t j=0; j<16; ++j )
                    *passBufferPtr++ = (float)viewProjTex[0][j];

                //vec2 shadowRcv[numShadowMaps].shadowDepthRange
                Real fNear, fFar;
                shadowNode->getMinMaxDepthRange( i, fNear, fFar );
                const Real depthRange = fFar - fNear;
                *passBufferPtr++ = fNear;
                *passBufferPtr++ = 1.0f / depthRange;

                passBufferPtr += 2; //Padding
            }

            //---------------------------------------------------------------------------
            //                          ---- PIXEL SHADER ----
            //---------------------------------------------------------------------------

            Matrix3 viewMatrix3, invViewMatrix3;
            viewMatrix.extract3x3Matrix( viewMatrix3 );
            invViewMatrix3 = viewMatrix3.Inverse();

            //mat3 invViewMatCubemap
            for( size_t i=0; i<9; ++i )
            {
#ifdef OGRE_GLES2_WORKAROUND_2
                Matrix3 xRot( 1.0f, 0.0f, 0.0f,
                              0.0f, 0.0f, -1.0f,
                              0.0f, 1.0f, 0.0f );
                xRot = xRot * invViewMatrix3;
                *passBufferPtr++ = (float)xRot[0][i];
#else
                *passBufferPtr++ = (float)invViewMatrix3[0][i];
#endif

                //Alignment: each row/column is one vec4, despite being 3x3
                if( !( (i+1) % 3 ) )
                    ++passBufferPtr;
            }

            //float pssmSplitPoints
            for( int32 i=0; i<numPssmSplits; ++i )
                *passBufferPtr++ = (*shadowNode->getPssmSplits(0))[i];

            passBufferPtr += alignToNextMultiple( numPssmSplits, 4 ) - numPssmSplits;

            if( shadowNode )
            {
                const LightClosestArray &lights = shadowNode->getShadowCastingLights();

                for( int32 i=0; i<numLights; ++i )
                {
                    Vector4 lightPos4 = lights[i].light->getAs4DVector();
                    Vector3 lightPos = viewMatrix3 * Vector3( lightPos4.x, lightPos4.y, lightPos4.z );

                    //vec3 lights[numLights].position
                    *passBufferPtr++ = lightPos.x;
                    *passBufferPtr++ = lightPos.y;
                    *passBufferPtr++ = lightPos.z;
                    ++passBufferPtr;

                    //vec3 lights[numLights].diffuse
                    ColourValue colour = lights[i].light->getDiffuseColour() *
                                         lights[i].light->getPowerScale();
                    *passBufferPtr++ = colour.r;
                    *passBufferPtr++ = colour.g;
                    *passBufferPtr++ = colour.b;
                    ++passBufferPtr;

                    //vec3 lights[numLights].specular
                    colour = lights[i].light->getSpecularColour() * lights[i].light->getPowerScale();
                    *passBufferPtr++ = colour.r;
                    *passBufferPtr++ = colour.g;
                    *passBufferPtr++ = colour.b;
                    ++passBufferPtr;

                    //vec3 lights[numLights].attenuation;
                    Real attenRange     = lights[i].light->getAttenuationRange();
                    Real attenLinear    = lights[i].light->getAttenuationLinear();
                    Real attenQuadratic = lights[i].light->getAttenuationQuadric();
                    *passBufferPtr++ = attenRange;
                    *passBufferPtr++ = attenLinear;
                    *passBufferPtr++ = attenQuadratic;
                    ++passBufferPtr;

                    //vec3 lights[numLights].spotDirection;
                    Vector3 spotDir = viewMatrix3 * lights[i].light->getDerivedDirection();
                    *passBufferPtr++ = spotDir.x;
                    *passBufferPtr++ = spotDir.y;
                    *passBufferPtr++ = spotDir.z;
                    ++passBufferPtr;

                    //vec3 lights[numLights].spotParams;
                    Radian innerAngle = lights[i].light->getSpotlightInnerAngle();
                    Radian outerAngle = lights[i].light->getSpotlightOuterAngle();
                    *passBufferPtr++ = 1.0f / ( cosf( innerAngle.valueRadians() * 0.5f ) -
                                             cosf( outerAngle.valueRadians() * 0.5f ) );
                    *passBufferPtr++ = cosf( outerAngle.valueRadians() * 0.5f );
                    *passBufferPtr++ = lights[i].light->getSpotlightFalloff();
                    ++passBufferPtr;

                    if( (size_t)i < shadowNode->getLocalTextures().size() )
                    {
                        //vec2 lights[numLights].invShadowMapSize
                        //TODO: textures[0] is out of bounds when using shadow atlas. Also see how what
                        //changes need to be done so that UV calculations land on the right place
                        uint32 texWidth  = shadowNode->getLocalTextures()[i].textures[0]->getWidth();
                        uint32 texHeight = shadowNode->getLocalTextures()[i].textures[0]->getHeight();
                        *passBufferPtr++ = 1.0f / texWidth;
                        *passBufferPtr++ = 1.0f / texHeight;
                        passBufferPtr += 2;
                    }
                    else
                    {
                        //If we have 3 directional lights and two shadow mapped lights, this is possible.
                        passBufferPtr += 4;
                    }
                }
            }
            else
            {
                //No shadow maps, only pass directional lights
                const LightListInfo &globalLightList = sceneManager->getGlobalLightList();

                for( int32 i=0; i<numDirectionalLights; ++i )
                {
                    Vector4 lightPos4 = globalLightList.lights[i]->getAs4DVector();
                    Vector3 lightPos = viewMatrix3 * Vector3( lightPos4.x, lightPos4.y, lightPos4.z );

                    //vec3 lights[numLights].position
                    *passBufferPtr++ = lightPos.x;
                    *passBufferPtr++ = lightPos.y;
                    *passBufferPtr++ = lightPos.z;
                    ++passBufferPtr;

                    //vec3 lights[numLights].diffuse
                    ColourValue colour = globalLightList.lights[i]->getDiffuseColour() *
                                         globalLightList.lights[i]->getPowerScale();
                    *passBufferPtr++ = colour.r;
                    *passBufferPtr++ = colour.g;
                    *passBufferPtr++ = colour.b;
                    ++passBufferPtr;

                    //vec3 lights[numLights].specular
                    colour = globalLightList.lights[i]->getSpecularColour() * globalLightList.lights[i]->getPowerScale();
                    *passBufferPtr++ = colour.r;
                    *passBufferPtr++ = colour.g;
                    *passBufferPtr++ = colour.b;
                    ++passBufferPtr;
                }
            }
        }
        else
        {
            //vec2 depthRange;
            Real fNear, fFar;
            shadowNode->getMinMaxDepthRange( camera, fNear, fFar );
            const Real depthRange = fFar - fNear;
            *passBufferPtr++ = fNear;
            *passBufferPtr++ = 1.0f / depthRange;
            passBufferPtr += 2;
        }

        assert( (size_t)(passBufferPtr - startupPtr) == mapSize );

        passBuffer->unmap( UO_KEEP_PERSISTENT );

        mLastTextureHash = 0;

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    uint32 HlmsPbs::fillBuffersFor( const HlmsCache *cache, const QueuedRenderable &queuedRenderable,
                                    bool casterPass, const HlmsCache *lastCache,
                                    CommandBuffer *commandBuffer )
    {
        assert( dynamic_cast<const HlmsPbsDatablock*>( queuedRenderable.renderable->getDatablock() ) );
        const HlmsPbsDatablock *datablock = static_cast<const HlmsPbsDatablock*>(
                                                queuedRenderable.renderable->getDatablock() );

        if( !lastCache || lastCache->type != HLMS_PBS )
        {
            //We changed HlmsType, rebind the shared textures.
            FastArray<TexturePtr>::const_iterator itor = mPreparedPass.shadowMaps.begin();
            FastArray<TexturePtr>::const_iterator end  = mPreparedPass.shadowMaps.end();

            if( !casterPass )
            {
                size_t texUnit = 1;
                while( itor != end )
                {
                    *commandBuffer->addCommand<CbTexture>() = CbTexture( texUnit, true, itor->get() );
                    ++texUnit;
                    ++itor;
                }
            }
            else
            {
                *commandBuffer->addCommand<CbTextureDisableFrom>() = CbTextureDisableFrom( 1 );
            }

            mLastTextureHash = 0;
        }

        uint32 * RESTRICT_ALIAS currentMappedConstBuffer    = mCurrentMappedConstBuffer;
        float * RESTRICT_ALIAS currentMappedTexBuffer       = mCurrentMappedTexBuffer;

        bool hasSkeletonAnimation = queuedRenderable.renderable->hasSkeletonAnimation();

        const Matrix4 &worldMat = queuedRenderable.movableObject->_getParentNodeFullTransform();

        if( (currentMappedConstBuffer - mStartMappedConstBuffer) + 1 >= mCurrentConstBufferSize )
            currentMappedConstBuffer = mapNextConstBuffer( commandBuffer );

        //---------------------------------------------------------------------------
        //                          ---- VERTEX SHADER ----
        //---------------------------------------------------------------------------
#if !OGRE_DOUBLE_PRECISION
        if( !hasSkeletonAnimation )
        {
            if( (currentMappedTexBuffer - mStartMappedTexBuffer) +
                    16 * (1 + !casterPass) >= mCurrentTexBufferSize )
            {
                currentMappedTexBuffer = mapNextTexBuffer( commandBuffer );
            }

            //uint worldMaterialIdx[]
            *currentMappedConstBuffer++ = datablock->getAssignedSlot() & 0x1FF;

            //mat4 worldViewProj
            Matrix4 tmp = mPreparedPass.viewProjMatrix * worldMat;
    #ifdef OGRE_GLES2_WORKAROUND_1
            tmp = tmp.transpose();
    #endif
            memcpy( currentMappedTexBuffer, &tmp, sizeof(Matrix4) );
            currentMappedTexBuffer += 16;

            //mat4 worldView
            tmp = mPreparedPass.viewMatrix.concatenateAffine( worldMat );
    #ifdef OGRE_GLES2_WORKAROUND_1
            tmp = tmp.transpose();
    #endif
            memcpy( currentMappedTexBuffer, &tmp, sizeof(Matrix4) * !casterPass );
            currentMappedTexBuffer += 16 * !casterPass;
        }
        else
        {
            //uint worldMaterialIdx[]
            *currentMappedConstBuffer++ = (((mCurrentMappedTexBuffer - mStartMappedTexBuffer) / 3) << 9) |
                                            (datablock->getAssignedSlot() & 0x1FF);

            uint16 numWorldTransforms = queuedRenderable.renderable->getNumWorldTransforms();
            assert( numWorldTransforms <= 256 );

            if( (currentMappedTexBuffer - mStartMappedTexBuffer) +
                    12 * numWorldTransforms >= mCurrentTexBufferSize )
            {
                currentMappedTexBuffer = mapNextTexBuffer( commandBuffer );
            }

            //vec4 worldMat[][3]
            //TODO: Don't rely on a virtual function + make a direct 4x3 copy
            Matrix4 tmp[256];
            queuedRenderable.renderable->getWorldTransforms( tmp );
            for( size_t i=0; i<numWorldTransforms; ++i )
            {
                memcpy( currentMappedTexBuffer, &tmp[i], 12 * sizeof(float) );
                currentMappedTexBuffer += 12;
            }
        }
#else
    #error Not Coded Yet! (cannot use memcpy on Matrix4)
#endif

        //---------------------------------------------------------------------------
        //                          ---- PIXEL SHADER ----
        //---------------------------------------------------------------------------

        if( !casterPass )
        {
            if( datablock->mTextureHash != mLastTextureHash )
            {
                //Rebind textures
                size_t texUnit = mPreparedPass.shadowMaps.size() + 1;
                for( size_t i=0; i<NUM_PBSM_TEXTURE_TYPES; ++i )
                {
                    const TexturePtr &texturePtr = datablock->getTexture( i );
                    if( !texturePtr.isNull() )
                    {
                        *commandBuffer->addCommand<CbTexture>() =
                                CbTexture( texUnit++, true, texturePtr.get(), datablock->mSamplerblocks[i] );
                    }
                }

                *commandBuffer->addCommand<CbTextureDisableFrom>() = CbTextureDisableFrom( texUnit );
            }
        }

        mCurrentMappedConstBuffer   = currentMappedConstBuffer;
        mCurrentMappedTexBuffer     = currentMappedTexBuffer;

        return (mCurrentConstBufferSize - mStartMappedConstBuffer) - 1;
    }
    //-----------------------------------------------------------------------------------
    void HlmsPbs::unmapConstBuffer(void)
    {
        if( mStartMappedConstBuffer )
        {
            //Unmap the current buffer
            ConstBufferPacked *constBuffer = mConstBuffers[mCurrentConstBuffer];
            constBuffer->unmap( UO_KEEP_PERSISTENT, 0,
                                (mCurrentMappedConstBuffer - mStartMappedConstBuffer) * sizeof(uint32) );

            ++mCurrentConstBuffer;

            mStartMappedConstBuffer     = 0;
            mCurrentMappedConstBuffer   = 0;
        }
    }
    //-----------------------------------------------------------------------------------
    DECL_MALLOC uint32* HlmsPbs::mapNextConstBuffer( CommandBuffer *commandBuffer )
    {
        unmapConstBuffer();

        if( mCurrentConstBuffer > mConstBuffers.size() )
        {
            //mConstBuffers.push_back(  ); TODO
        }

        ConstBufferPacked *constBuffer = mConstBuffers[mCurrentConstBuffer];

        mStartMappedConstBuffer     = reinterpret_cast<uint32*>(
                                            constBuffer->map( 0, constBuffer->getNumElements(),
                                                              MS_PERSISTENT_INCOHERENT ) );
        mCurrentMappedConstBuffer   = mStartMappedConstBuffer;

        *commandBuffer->addCommand<CbShaderBuffer>() = CbShaderBuffer( 0, constBuffer, 0, 0 );

        return mStartMappedConstBuffer;
    }
    //-----------------------------------------------------------------------------------
    void HlmsPbs::unmapTexBuffer( CommandBuffer *commandBuffer )
    {
        //Save our progress
        mTexLastOffset = (mCurrentMappedTexBuffer - mStartMappedTexBuffer) * sizeof(float);

        if( mStartMappedTexBuffer )
        {
            //Unmap the current buffer
            TexBufferPacked *texBuffer = mTexBuffers[mCurrentTexBuffer];
            texBuffer->unmap( UO_KEEP_PERSISTENT, 0, mTexLastOffset );

            CbShaderBuffer *shaderBufferCmd = reinterpret_cast<CbShaderBuffer*>(
                        commandBuffer->getCommandFromOffset( mLastTexBufferCmdOffset ) );
            if( shaderBufferCmd )
            {
                assert( shaderBufferCmd->bufferPacked == texBuffer );
                shaderBufferCmd->bindSizeBytes = mTexLastOffset;
            }
        }

        mStartMappedTexBuffer   = 0;
        mCurrentMappedTexBuffer = 0;

        //Ensure the proper alignment
        mTexLastOffset = alignToNextMultiple( mTexLastOffset, mVaoManager->getTexBufferAlignment() );
    }
    //-----------------------------------------------------------------------------------
    DECL_MALLOC float* HlmsPbs::mapNextTexBuffer( CommandBuffer *commandBuffer )
    {
        unmapTexBuffer( commandBuffer );

        TexBufferPacked *texBuffer = mTexBuffers[mCurrentTexBuffer];

        //We'll go out of bounds. This buffer is full. Get a new one and remap from 0.
        if( mTexLastOffset >= texBuffer->getTotalSizeBytes() )
        {
            mTexLastOffset = 0;
            ++mCurrentTexBuffer;

            if( mCurrentTexBuffer >= mTexBuffers.size() )
            {
                //mTexBuffers.push_back(  ); TODO
            }

            texBuffer = mTexBuffers[mCurrentTexBuffer];
        }

        mStartMappedTexBuffer   = reinterpret_cast<float*>(
                                            texBuffer->map( mTexLastOffset,
                                                            texBuffer->getNumElements() - mTexLastOffset,
                                                            MS_PERSISTENT_INCOHERENT, false ) );
        mCurrentMappedTexBuffer = mStartMappedTexBuffer;

        CbShaderBuffer *shaderBufferCmd = commandBuffer->addCommand<CbShaderBuffer>();
        *shaderBufferCmd = CbShaderBuffer( 0, texBuffer, 0, 0 );

        mLastTexBufferCmdOffset = commandBuffer->getCommandOffset( shaderBufferCmd );

        return mStartMappedTexBuffer;
    }
    //-----------------------------------------------------------------------------------
    void HlmsPbs::destroyAllBuffers(void)
    {
        mCurrentPassBuffer  = 0;
        mCurrentConstBuffer = 0;
        mCurrentTexBuffer   = 0;
        mTexLastOffset      = 0;

        {
            TexBufferPackedVec::const_iterator itor = mTexBuffers.begin();
            TexBufferPackedVec::const_iterator end  = mTexBuffers.end();

            while( itor != end )
                mVaoManager->destroyTexBuffer( *itor++ );

            mTexBuffers.clear();
        }

        {
            ConstBufferPackedVec::const_iterator itor = mConstBuffers.begin();
            ConstBufferPackedVec::const_iterator end  = mConstBuffers.end();

            while( itor != end )
                mVaoManager->destroyConstBuffer( *itor++ );

            mConstBuffers.clear();
        }

        {
            ConstBufferPackedVec::const_iterator itor = mPassBuffers.begin();
            ConstBufferPackedVec::const_iterator end  = mPassBuffers.end();

            while( itor != end )
                mVaoManager->destroyConstBuffer( *itor++ );

            mPassBuffers.clear();
        }
    }
    //-----------------------------------------------------------------------------------
    void HlmsPbs::frameEnded(void)
    {
        mCurrentPassBuffer  = 0;
        mCurrentConstBuffer = 0;
        mCurrentTexBuffer   = 0;
        mTexLastOffset      = 0;

        TexBufferPackedVec::const_iterator itor = mTexBuffers.begin();
        TexBufferPackedVec::const_iterator end  = mTexBuffers.end();

        while( itor != end )
        {
            (*itor)->advanceFrame();
            ++itor;
        }
    }
    //-----------------------------------------------------------------------------------
    HlmsDatablock* HlmsPbs::createDatablockImpl( IdString datablockName,
                                                       const HlmsMacroblock *macroblock,
                                                       const HlmsBlendblock *blendblock,
                                                       const HlmsParamVec &paramVec )
    {
        return OGRE_NEW HlmsPbsDatablock( datablockName, this, macroblock, blendblock, paramVec );
    }
}
