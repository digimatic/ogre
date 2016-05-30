/*
  -----------------------------------------------------------------------------
  This source file is part of OGRE
  (Object-oriented Graphics Rendering Engine)
  For the latest info, see http://www.ogre3d.org

Copyright (c) 2000-2016 Torus Knot Software Ltd

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

#include "OgreMetalDevice.h"
#include "OgreMetalRenderSystem.h"

#import <Metal/MTLDevice.h>
#import <Metal/MTLCommandQueue.h>
#import <Metal/MTLCommandBuffer.h>

namespace Ogre
{
    MetalDevice::MetalDevice( MetalRenderSystem *renderSystem ) :
        mDevice( 0 ),
        mMainCommandQueue( 0 ),
        mCurrentCommandBuffer( 0 ),
        mBlitEncoder( 0 ),
        mRenderEncoder( 0 ),
        mRenderSystem( renderSystem ),
        mStallSemaphore( 0 )
    {
        mStallSemaphore = dispatch_semaphore_create( 0 );
    }
    //-------------------------------------------------------------------------
    MetalDevice::~MetalDevice()
    {
        mDevice = 0;
        mMainCommandQueue = 0;
        mCurrentCommandBuffer = 0;
        mBlitEncoder = 0;
        mRenderEncoder = 0;
    }
    //-------------------------------------------------------------------------
    void MetalDevice::init(void)
    {
        mDevice = MTLCreateSystemDefaultDevice();
        mMainCommandQueue = [mDevice newCommandQueue];
        mCurrentCommandBuffer = [mMainCommandQueue commandBuffer];
        mBlitEncoder = 0;
        mRenderEncoder = 0;
    }
    //-------------------------------------------------------------------------
    void MetalDevice::endBlitEncoder(void)
    {
        if( mBlitEncoder )
        {
            [mBlitEncoder endEncoding];
            mBlitEncoder = 0;
        }
    }
    //-------------------------------------------------------------------------
    void MetalDevice::endRenderEncoder(void)
    {
        if( mRenderEncoder )
        {
            [mRenderEncoder endEncoding];
            mRenderEncoder = 0;

            if( mRenderSystem->getActiveDevice() == this )
                mRenderSystem->_notifyActiveEncoderEnded();
        }
    }
    //-------------------------------------------------------------------------
    void MetalDevice::endComputeEncoder(void)
    {
    }
    //-------------------------------------------------------------------------
    void MetalDevice::endAllEncoders(void)
    {
        endBlitEncoder();
        endRenderEncoder();
        endComputeEncoder();
    }
    //-------------------------------------------------------------------------
    void MetalDevice::commitAndNextCommandBuffer(void)
    {
        endAllEncoders();
        //Push the command buffer to the GPU
        [mCurrentCommandBuffer commit];
        mCurrentCommandBuffer = [mMainCommandQueue commandBuffer];
    }
    //-------------------------------------------------------------------------
    id<MTLBlitCommandEncoder> MetalDevice::getBlitEncoder(void)
    {
        if( !mBlitEncoder )
        {
            endRenderEncoder();
            endComputeEncoder();

            mBlitEncoder = [mCurrentCommandBuffer blitCommandEncoder];
        }

        return mBlitEncoder;
    }
    //-------------------------------------------------------------------------
    void MetalDevice::stall(void)
    {
        __block dispatch_semaphore_t blockSemaphore = mStallSemaphore;
        [mCurrentCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer)
        {
            dispatch_semaphore_signal( blockSemaphore );
        }];
        commitAndNextCommandBuffer();

        const long result = dispatch_semaphore_wait( mStallSemaphore, DISPATCH_TIME_FOREVER );

        if( result != 0 )
        {
            OGRE_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR,
                         "Failure while waiting for a MetalFence. Could be out of GPU memory. "
                         "Update your video card drivers. If that doesn't help, "
                         "contact the developers. Error code: " + StringConverter::toString( result ),
                         "MetalDevice::stall" );
        }

        //TODO: Call a manager so we can inform everyone it's safe (to avoid more unnecessary stalls)
    }
}
