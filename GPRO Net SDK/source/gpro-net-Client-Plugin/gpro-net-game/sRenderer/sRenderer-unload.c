/*
	Copyright 2011-2021 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein
	
	sRenderer_unloading.c/.cpp
	Demo state function implementations.

	****************************************************
	*** THIS IS ONE OF YOUR DEMO'S MAIN SOURCE FILES ***
	*** Implement your demo logic pertaining to      ***
	***     UNLOADING in this file.                  ***
	****************************************************
*/

//-----------------------------------------------------------------------------

#include "sRenderer.h"

#include <stdio.h>


//-----------------------------------------------------------------------------
// UNLOADING
// this is where the union array style comes in handy; don't need a single 
//	release statement for each and every object... just iterate and release!

// utility to unload geometry
void renderer_unloadGeometry(sRenderer* renderer)
{
	a3_BufferObject* currentBuff = renderer->drawDataBuffer,
		* const endBuff = currentBuff + rendererMaxCount_drawDataBuffer;
	a3_VertexArrayDescriptor* currentVAO = renderer->vertexArray,
		* const endVAO = currentVAO + rendererMaxCount_vertexArray;
	a3_VertexDrawable* currentDraw = renderer->drawable,
		* const endDraw = currentDraw + rendererMaxCount_drawable;

	while (currentBuff < endBuff)
		a3bufferRelease(currentBuff++);
	while (currentVAO < endVAO)
		a3vertexArrayReleaseDescriptor(currentVAO++);
	while (currentDraw < endDraw)
		a3vertexDrawableRelease(currentDraw++);
}

// utility to unload shaders
void renderer_unloadShaders(sRenderer* renderer)
{
	sRendererProgram* currentProg = renderer->shaderProgram,
		* const endProg = currentProg + rendererMaxCount_shaderProgram;
	a3_UniformBuffer* currentUBO = renderer->uniformBuffer,
		* const endUBO = currentUBO + rendererMaxCount_uniformBuffer;

	while (currentProg < endProg)
		a3shaderProgramRelease((currentProg++)->program);
	while (currentUBO < endUBO)
		a3bufferRelease(currentUBO++);
}


// utility to unload textures
void renderer_unloadTextures(sRenderer* renderer)
{
	a3_Texture* currentTex = renderer->texture,
		* const endTex = currentTex + rendererMaxCount_texture;

	while (currentTex < endTex)
		a3textureRelease(currentTex++);
}


// utility to unload framebuffers
void renderer_unloadFramebuffers(sRenderer* renderer)
{
	a3_Framebuffer* currentFBO = renderer->framebuffer,
		* const endFBO = currentFBO + rendererMaxCount_framebuffer;

	while (currentFBO < endFBO)
		a3framebufferRelease(currentFBO++);
}


//-----------------------------------------------------------------------------

// confirm that all graphics objects were unloaded
void renderer_unloadValidate(const sRenderer* renderer)
{
	a3ui32 handle;
	const a3_BufferObject* currentBuff = renderer->drawDataBuffer,
		* const endBuff = currentBuff + rendererMaxCount_drawDataBuffer;
	const a3_VertexArrayDescriptor* currentVAO = renderer->vertexArray,
		* const endVAO = currentVAO + rendererMaxCount_vertexArray;
	const sRendererProgram* currentProg = renderer->shaderProgram,
		* const endProg = currentProg + rendererMaxCount_shaderProgram;
	const a3_UniformBuffer* currentUBO = renderer->uniformBuffer,
		* const endUBO = currentUBO + rendererMaxCount_uniformBuffer;
	const a3_Texture* currentTex = renderer->texture,
		* const endTex = currentTex + rendererMaxCount_texture;
	const a3_Framebuffer* currentFBO = renderer->framebuffer,
		* const endFBO = currentFBO + rendererMaxCount_framebuffer;

	handle = 0;
	while (currentBuff < endBuff)
		handle += (currentBuff++)->handle->handle;
	if (handle)
		printf("\n A3 Warning: One or more draw data buffers not released.");

	handle = 0;
	while (currentVAO < endVAO)
		handle += (currentVAO++)->handle->handle;
	if (handle)
		printf("\n A3 Warning: One or more vertex arrays not released.");

	handle = 0;
	while (currentProg < endProg)
		handle += (currentProg++)->program->handle->handle;
	if (handle)
		printf("\n A3 Warning: One or more shader programs not released.");

	handle = 0;
	while (currentUBO < endUBO)
		handle += (currentUBO++)->handle->handle;
	if (handle)
		printf("\n A3 Warning: One or more uniform buffers not released.");

	handle = 0;
	while (currentTex < endTex)
		handle += (currentTex++)->handle->handle;
	if (handle)
		printf("\n A3 Warning: One or more textures not released.");

	handle = 0;
	while (currentFBO < endFBO)
		handle += (currentFBO++)->handle->handle;
	if (handle)
		printf("\n A3 Warning: One or more framebuffers not released.");
}


//-----------------------------------------------------------------------------
