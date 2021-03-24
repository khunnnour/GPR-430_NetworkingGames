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
	
	sRenderer-util.h
	Drawing utilities for renderer.
*/

#ifndef _GPRO_NET_SRENDERER_UTIL_H_
#define _GPRO_NET_SRENDERER_UTIL_H_


//-----------------------------------------------------------------------------
// animal3D framework includes

#include "animal3D/animal3D.h"
#include "animal3D-A3DG/animal3D-A3DG.h"
#include "animal3D-A3DM/animal3D-A3DM.h"


//-----------------------------------------------------------------------------
// demo includes

#include "sRenderer-program.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#endif	// __cplusplus

	
//-----------------------------------------------------------------------------
// general utilities

// get the inverse transpose of a matrix
inline a3real4x4r renderer_quickInverseTranspose_internal(a3real4x4p m_out, const a3real4x4p m_in);

// invert and transpose a matrix
inline a3real4x4r renderer_quickInvertTranspose_internal(a3real4x4p m_inout);

// get the transpose of a matrix with the bottom row set to zero
inline a3real4x4r renderer_quickTransposedZeroBottomRow(a3real4x4p m_out, const a3real4x4p m_in);


//-----------------------------------------------------------------------------
// rendering states

// set default blending mode for composition
inline void renderer_enableCompositeBlending();

// set default blending mode for accumulation
inline void renderer_enableAdditiveBlending();

// set default state for writing to stencil buffer
inline void renderer_enableStencilWrite();

// set default state for comparing with stencil buffer
inline void renderer_enableStencilCompare();

// set default state for renderer
inline void renderer_setDefaultGraphicsState();

// set state for scene draw
inline void renderer_setSceneState(a3_Framebuffer const* currentWriteFBO, a3boolean displaySkybox);


//-----------------------------------------------------------------------------

inline void renderer_drawModelSimple(a3real4x4p modelViewProjectionMat, a3real4x4p const viewProjectionMat, a3real4x4p const modelMat, sRendererProgram const* program);

inline void renderer_drawModelSimple_activateModel(a3real4x4p modelViewProjectionMat, a3real4x4p const viewProjectionMat, a3real4x4p const modelMat, sRendererProgram const* program, a3_VertexDrawable const* drawable);

inline void renderer_drawModelSolidColor(a3real4x4p modelViewProjectionMat, a3real4x4p const viewProjectionMat, a3real4x4p const modelMat, sRendererProgram const* program, a3_VertexDrawable const* drawable, a3real4p const color);

inline void renderer_drawModelTexturedColored_invertModel(a3real4x4p modelViewProjectionMat, a3real4x4p const viewProjectionMat, a3real4x4p const modelMat, a3real4x4p const atlasMat, sRendererProgram const* program, a3_VertexDrawable const* drawable, a3_Texture const* texture, a3real4p const color);

inline void renderer_drawModelLighting(a3real4x4p modelViewProjectionMat, a3real4x4p modelViewMat, a3real4x4p const viewProjectionMat, a3real4x4p const viewMat, a3real4x4p const modelMat, sRendererProgram const* program, a3_VertexDrawable const* drawable, a3real4p const color);

inline void renderer_drawModelLighting_bias_other(a3real4x4p modelViewProjectionBiasMat_other, a3real4x4p modelViewProjectionMat, a3real4x4p modelViewMat, a3real4x4p const viewProjectionBiasMat_other, a3real4x4p const viewProjectionMat, a3real4x4p const viewMat, a3real4x4p const modelMat, sRendererProgram const* program, a3_VertexDrawable const* drawable, a3real4p const color);

inline void renderer_drawStencilTest(a3real4x4p modelViewProjectionMat, a3real4x4p const viewProjectionMat, a3real4x4p const modelMat, sRendererProgram const* program, a3_VertexDrawable const* drawable);


//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#include "_inl/sRenderer-util.inl"


#endif	// !_GPRO_NET_SRENDERER_UTIL_H_