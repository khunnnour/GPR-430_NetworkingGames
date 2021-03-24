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
	
	sRenderer.h
	Renderer interface and programmer function declarations using animal3D.
*/

#ifndef _GPRO_NET_SRENDERER_H_
#define _GPRO_NET_SRENDERER_H_


//-----------------------------------------------------------------------------
// animal3D framework includes

#include "animal3D/animal3D.h"
#include "animal3D-A3DM/animal3D-A3DM.h"
#include "animal3D-A3DG/animal3D-A3DG.h"


//-----------------------------------------------------------------------------
// other demo includes

#include "_utility/sRenderer-program.h"
#include "_utility/sRenderer-object.h"
#include "_utility/sRenderer-macros.h"
#include "_utility/sRenderer-util.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#endif	// __cplusplus


//-----------------------------------------------------------------------------

// text mode names
typedef enum eRenderer_TextDisplayName
{
	renderer_textDisabled,			// no text overlay
	renderer_textControls,			// display controls
	renderer_textControls_gen,		// display general controls
	renderer_textData,				// display data

	renderer_text_max
} eRenderer_TextDisplayName;


// object maximum counts for easy array storage
// good idea to make these numbers greater than what you actually need 
//	and if you end up needing more just increase the count... there's 
//	more than enough memory to hold extra objects
typedef enum eRenderer_ObjectMaxCount
{
	rendererMaxCount_drawDataBuffer = 1,
	rendererMaxCount_vertexArray = 4,
	rendererMaxCount_drawable = 16,

	rendererMaxCount_shaderProgram = 32,
	rendererMaxCount_uniformBuffer = 8,

	rendererMaxCount_texture = 32,

	rendererMaxCount_framebuffer = 16,
} eRenderer_ObjectMaxCount;

	
//-----------------------------------------------------------------------------

// persistent renderer data structure
typedef struct sRenderer
{
	//-------------------------------------------------------------------------
	// general variables pertinent to the state

	// terminate key pressed
	a3i32 exitFlag;

	// asset streaming between loads enabled (careful!)
	a3boolean streaming;

	// window and full-frame dimensions
	a3ui32 windowWidth, windowHeight;
	a3real windowWidthInv, windowHeightInv, windowAspect;
	a3ui32 frameWidth, frameHeight;
	a3real frameWidthInv, frameHeightInv, frameAspect;
	a3i32 frameBorder;


	//-------------------------------------------------------------------------
	// objects that have known or fixed instance count in the whole demo

	// text renderer
	a3boolean textInit;
	a3ui32 textMode;
	a3_TextRenderer text[2];


	//-------------------------------------------------------------------------
	// scene variables and objects

	// toggle grid in scene and axes superimposed, as well as other mods
	a3boolean displayGrid, displaySkybox;
	a3boolean displayWorldAxes, displayObjectAxes;

	// atlas matrices
	a3mat4 atlas_earth, atlas_mars, atlas_moon, atlas_marble, atlas_copper, atlas_stone, atlas_checker;

	// texture sets
	a3_Texture const* texSet_earth[4], * texSet_mars[4], * texSet_stone[4];


	//-------------------------------------------------------------------------
	// object arrays: organized as anonymous unions for two reasons: 
	//	1. easy to manage entire sets of the same type of object using the 
	//		array component
	//	2. at the same time, variables are named pointers


	// draw data buffers
	union {
		a3_VertexBuffer drawDataBuffer[rendererMaxCount_drawDataBuffer];
		struct {
			a3_VertexBuffer
				vbo_staticSceneObjectDrawBuffer[1];			// buffer to hold all data for static scene objects (e.g. grid)
		};
	};

	// vertex array objects
	union {
		a3_VertexArrayDescriptor vertexArray[rendererMaxCount_vertexArray];
		struct {
			a3_VertexArrayDescriptor
				vao_tangentbasis_texcoord[1],				// VAO for vertex format with complete tangent basis, with texcoords
				vao_position_normal_texcoord[1];			// VAO for vertex format with complete tangent basis, with texcoords
			a3_VertexArrayDescriptor
				vao_position_color[1],						// VAO for vertex format with position and color
				vao_position[1];							// VAO for vertex format with only position
		};
	};

	// drawables
	union {
		a3_VertexDrawable drawable[rendererMaxCount_drawable];
		struct {
			a3_VertexDrawable
				draw_grid[1],								// wireframe ground plane to emphasize scaling
				draw_axes[1];								// coordinate axes at the center of the world
			a3_VertexDrawable
				draw_unit_box[1],							// unit box (width = height = depth = 1)
				draw_unit_sphere[1],						// unit sphere (radius = 1)
				draw_unit_cylinder[1],						// unit cylinder (radius = length = 1)
				draw_unit_capsule[1],						// unit capsule (radius = length = 1)
				draw_unit_torus[1],							// unit torus (major radius = 1)
				draw_unit_cone[1],							// unit cone (radius = height = 1)
				draw_unit_plane_z[1];						// unit plane (width = height = 1) with Z normal
			a3_VertexDrawable
				draw_teapot[1];								// can't not have a Utah teapot
		};
	};


	// shader programs and uniforms
	union {
		sRendererProgram shaderProgram[rendererMaxCount_shaderProgram];
		struct {
			sRendererProgram
				prog_transform_instanced[1],				// transform vertex only with instancing; no fragment shader
				prog_transform[1];							// transform vertex only; no fragment shader
			sRendererProgram
				prog_drawColorAttrib_instanced[1],			// draw color attribute with instancing
				prog_drawColorUnif_instanced[1],			// draw uniform color with instancing
				prog_drawColorAttrib[1],					// draw color attribute
				prog_drawColorUnif[1];						// draw uniform color
			sRendererProgram
				prog_drawPhong_instanced[1],				// draw Phong shading model with instancing
				prog_drawLambert_instanced[1],				// draw Lambert shading model with instancing
				prog_drawTexture_instanced[1],				// draw texture with instancing
				prog_drawPhong[1],							// draw Phong shading model
				prog_drawLambert[1],						// draw Lambert shading model
				prog_drawTexture[1];						// draw texture
		};
	};

	// uniform buffers
	union {
		a3_UniformBuffer uniformBuffer[rendererMaxCount_uniformBuffer];
		struct {
			a3_UniformBuffer
				ubo_light[4],								// uniform buffers for light data
				ubo_transform[4];							// uniform buffers for transformation data
		};
	};


	// textures
	union {
		a3_Texture texture[rendererMaxCount_texture];
		struct {
			a3_Texture
				tex_skybox_clouds[1],
				tex_skybox_water[1],
				tex_ramp_dm[1],
				tex_ramp_sm[1],
				tex_testsprite[1],
				tex_checker[1];
		};
	};


	// framebuffers
	union {
		a3_Framebuffer framebuffer[rendererMaxCount_framebuffer];
		struct {
			a3_Framebuffer
				fbo_c16x4_d24s8[1];		// 16-bit color buffer (4 targets) and depth-stencil buffer (24/8)
		};
	};


	//-------------------------------------------------------------------------
	// the end
} sRenderer;


//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#endif	// !_GPRO_NET_SRENDERER_H_