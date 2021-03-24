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
	
	sRenderer_loading.c/.cpp
	Demo state function implementations.

	****************************************************
	*** THIS IS ONE OF YOUR DEMO'S MAIN SOURCE FILES ***
	*** Implement your demo logic pertaining to      ***
	***     LOADING in this file.                    ***
	****************************************************
*/

#include "_utility/sRenderer-macros.h"


//-----------------------------------------------------------------------------

// uncomment this to link extension library (if available)
//#define A3_USER_ENABLE_EXTENSION

// **WARNING: FOR TESTING/COMPARISON ONLY, DO NOT USE IN DELIVERABLE BUILDS**
// uncomment this to allow shader decoding (if available)
//#define A3_USER_ENABLE_SHADER_DECODING


//-----------------------------------------------------------------------------

#ifdef A3_USER_ENABLE_SHADER_DECODING
// override shader loading function name before including
#define a3shaderCreateFromFileList a3shaderCreateFromFileListEncoded
#endif	// A3_USER_ENABLE_SHADER_DECODING


#ifdef _WIN32
#ifdef A3_USER_ENABLE_EXTENSION
// force link extension lib
#pragma comment(lib,"animal3D-A3DX.lib")
#endif	// A3_USER_ENABLE_EXTENSION
#ifdef A3_USER_ENABLE_SHADER_DECODING
// add lib for shader decoding
#pragma comment(lib,"animal3D-UtilityLib.lib")
#endif	// A3_USER_ENABLE_SHADER_DECODING
#endif	// _WIN32


// define resource directories
#define A3_DEMO_RES_DIR	"../../../../resource/"
#define A3_DEMO_GLSL	A3_DEMO_RES_DIR"glsl/"
#define A3_DEMO_TEX		A3_DEMO_RES_DIR"tex/"
#define A3_DEMO_OBJ		A3_DEMO_RES_DIR"obj/"

// define resource subdirectories
#define A3_DEMO_VS		A3_DEMO_GLSL"4x/vs/"
#define A3_DEMO_TS		A3_DEMO_GLSL"4x/ts/"
#define A3_DEMO_GS		A3_DEMO_GLSL"4x/gs/"
#define A3_DEMO_FS		A3_DEMO_GLSL"4x/fs/"
#define A3_DEMO_CS		A3_DEMO_GLSL"4x/cs/"


//-----------------------------------------------------------------------------

#include "sRenderer.h"

#include <stdio.h>


//-----------------------------------------------------------------------------
// GENERAL UTILITIES

a3real4x4r renderer_setAtlasTransform_internal(a3real4x4p m_out,
	const a3ui16 atlasWidth, const a3ui16 atlasHeight,
	const a3ui16 subTexturePosX, const a3ui16 subTexturePosY,
	const a3ui16 subTextureWidth, const a3ui16 subTextureHeight,
	const a3ui16 subTextureBorderPadding, const a3ui16 subTextureAdditionalPadding)
{
	a3real4x4SetIdentity(m_out);
	m_out[0][0] = (a3real)(subTextureWidth) / (a3real)(atlasWidth);
	m_out[1][1] = (a3real)(subTextureHeight) / (a3real)(atlasHeight);
	m_out[3][0] = (a3real)(subTexturePosX + subTextureBorderPadding + subTextureAdditionalPadding) / (a3real)(atlasWidth);
	m_out[3][1] = (a3real)(subTexturePosY + subTextureBorderPadding + subTextureAdditionalPadding) / (a3real)(atlasHeight);
	return m_out;
}


// initialize texture set
inline void renderer_initTextureSet(a3_Texture const* textureSet[4],
	a3_Texture const* tex_dm, a3_Texture const* tex_sm, a3_Texture const* tex_nm, a3_Texture const* tex_hm)
{
	textureSet[0] = tex_dm;
	textureSet[1] = tex_sm;
	textureSet[2] = tex_nm;
	textureSet[3] = tex_hm;
}


//-----------------------------------------------------------------------------
// uniform helpers

#define renderer_getUniformLocation(demoProgram, handleName, getLocFunc) (demoProgram->handleName = getLocFunc(demoProgram->program, #handleName))
#define renderer_getUniformLocationValid(demoProgram, handleName, getLocFunc) if (renderer_getUniformLocation(demoProgram, handleName, getLocFunc) >= 0)
#define renderer_setUniformDefault(demoProgram, handleName, sendFunc, type, value) \
	renderer_getUniformLocationValid(demoProgram, handleName, a3shaderUniformGetLocation) \
		sendFunc(type, demoProgram->handleName, 1, value)
#define renderer_setUniformDefaultMat(demoProgram, handleName, sendFunc, type, value) \
	renderer_getUniformLocationValid(demoProgram, handleName, a3shaderUniformGetLocation) \
		sendFunc(type, 0, demoProgram->handleName, 1, value)
#define renderer_setUniformDefaultBlock(demoProgram, handleName, value) \
	renderer_getUniformLocationValid(demoProgram, handleName, a3shaderUniformBlockGetLocation) \
		a3shaderUniformBlockBind(demoProgram->program, demoProgram->handleName, value)

#define renderer_setUniformDefaultFloat(demoProgram, handleName, value) renderer_setUniformDefault(demoProgram, handleName, a3shaderUniformSendFloat, a3unif_single, value)
#define renderer_setUniformDefaultVec2(demoProgram, handleName, value) renderer_setUniformDefault(demoProgram, handleName, a3shaderUniformSendFloat, a3unif_vec2, value)
#define renderer_setUniformDefaultVec3(demoProgram, handleName, value) renderer_setUniformDefault(demoProgram, handleName, a3shaderUniformSendFloat, a3unif_vec3, value)
#define renderer_setUniformDefaultVec4(demoProgram, handleName, value) renderer_setUniformDefault(demoProgram, handleName, a3shaderUniformSendFloat, a3unif_vec4, value)
#define renderer_setUniformDefaultDouble(demoProgram, handleName, value) renderer_setUniformDefault(demoProgram, handleName, a3shaderUniformSendDouble, a3unif_single, value)
#define renderer_setUniformDefaultDVec2(demoProgram, handleName, value) renderer_setUniformDefault(demoProgram, handleName, a3shaderUniformSendDouble, a3unif_vec2, value)
#define renderer_setUniformDefaultDVec3(demoProgram, handleName, value) renderer_setUniformDefault(demoProgram, handleName, a3shaderUniformSendDouble, a3unif_vec3, value)
#define renderer_setUniformDefaultDVec4(demoProgram, handleName, value) renderer_setUniformDefault(demoProgram, handleName, a3shaderUniformSendDouble, a3unif_vec4, value)
#define renderer_setUniformDefaultInteger(demoProgram, handleName, value) renderer_setUniformDefault(demoProgram, handleName, a3shaderUniformSendInt, a3unif_single, value)
#define renderer_setUniformDefaultIVec2(demoProgram, handleName, value) renderer_setUniformDefault(demoProgram, handleName, a3shaderUniformSendInt, a3unif_vec2, value)
#define renderer_setUniformDefaultIVec3(demoProgram, handleName, value) renderer_setUniformDefault(demoProgram, handleName, a3shaderUniformSendInt, a3unif_vec3, value)
#define renderer_setUniformDefaultIVec4(demoProgram, handleName, value) renderer_setUniformDefault(demoProgram, handleName, a3shaderUniformSendInt, a3unif_vec4, value)
#define renderer_setUniformDefaultMat2(demoProgram, handleName) renderer_setUniformDefaultMat(demoProgram, handleName, a3shaderUniformSendFloatMat, a3unif_mat2, a3mat2_identity.mm)
#define renderer_setUniformDefaultMat3(demoProgram, handleName) renderer_setUniformDefaultMat(demoProgram, handleName, a3shaderUniformSendFloatMat, a3unif_mat3, a3mat3_identity.mm)
#define renderer_setUniformDefaultMat4(demoProgram, handleName) renderer_setUniformDefaultMat(demoProgram, handleName, a3shaderUniformSendFloatMat, a3unif_mat4, a3mat4_identity.mm)


//-----------------------------------------------------------------------------
// LOADING

// utility to load geometry
void renderer_loadGeometry(sRenderer *renderer)
{
	// tmp descriptor for loaded model
	typedef struct a3_TAG_DEMOSTATELOADEDMODEL {
		const a3byte *modelFilePath;
		const a3real *transform;
		a3_ModelLoaderFlag flag;
	} sRendererLoadedModel;

	// static model transformations
	static const a3mat4 downscale50x_y2z_x2y = {
		 0.00f, +0.02f,  0.00f,  0.00f,
		 0.00f,  0.00f, +0.02f,  0.00f,
		+0.02f,  0.00f,  0.00f,  0.00f,
		 0.00f,  0.00f,  0.00f, +1.00f,
	};
	static const a3mat4 scale1x_z2y = {
		+0.05f,  0.00f,  0.00f,  0.00f,
		 0.00f,  0.00f, -0.05f,  0.00f,
		 0.00f, +0.05f,  0.00f,  0.00f,
		 0.00f,  0.00f,  0.00f, +1.00f,
	};

	// pointer to shared vbo/ibo
	a3_VertexBuffer *vbo_ibo;
	a3_VertexArrayDescriptor *vao;
	a3_VertexDrawable *currentDrawable;
	a3ui32 sharedVertexStorage = 0, sharedIndexStorage = 0;
	a3ui32 numVerts = 0;
	a3ui32 i;


	// file streaming (if requested)
	a3_FileStream fileStream[1] = { 0 };
	const a3byte *const geometryStream = "./data/gpro_base_geom.dat";

	// geometry data
	a3_GeometryData displayShapesData[2] = { 0 };
	a3_GeometryData proceduralShapesData[7] = { 0 };
	a3_GeometryData loadedModelsData[1] = { 0 };
	const a3ui32 displayShapesCount = rendererArrayLen(displayShapesData);
	const a3ui32 proceduralShapesCount = rendererArrayLen(proceduralShapesData);
	const a3ui32 loadedModelsCount = rendererArrayLen(loadedModelsData);

	// common index format
	a3_IndexFormatDescriptor sceneCommonIndexFormat[1] = { 0 };
	a3ui32 bufferOffset, *const bufferOffsetPtr = &bufferOffset;


	// procedural scene objects
	// attempt to load stream if requested
	if (renderer->streaming && a3fileStreamOpenRead(fileStream, geometryStream))
	{
		// read from stream

		// static display objects
		for (i = 0; i < displayShapesCount; ++i)
			a3fileStreamReadObject(fileStream, displayShapesData + i, (a3_FileStreamReadFunc)a3geometryLoadDataBinary);

		// procedurally-generated objects
		for (i = 0; i < proceduralShapesCount; ++i)
			a3fileStreamReadObject(fileStream, proceduralShapesData + i, (a3_FileStreamReadFunc)a3geometryLoadDataBinary);

		// loaded model objects
		for (i = 0; i < loadedModelsCount; ++i)
			a3fileStreamReadObject(fileStream, loadedModelsData + i, (a3_FileStreamReadFunc)a3geometryLoadDataBinary);

		// done
		a3fileStreamClose(fileStream);
	}
	// not streaming or stream doesn't exist
	else if (!renderer->streaming || a3fileStreamOpenWrite(fileStream, geometryStream))
	{
		// create new data
		a3_ProceduralGeometryDescriptor displayShapes[rendererArrayLen(displayShapesData)] = { a3geomShape_none };
		a3_ProceduralGeometryDescriptor proceduralShapes[rendererArrayLen(proceduralShapesData)] = { a3geomShape_none };
		const sRendererLoadedModel loadedShapes[rendererArrayLen(loadedModelsData)] = {
			{ A3_DEMO_OBJ"teapot/teapot.obj", downscale50x_y2z_x2y.mm, a3model_calculateVertexTangents },
		};

		// static scene procedural objects
		//	(axes, grid)
		a3proceduralCreateDescriptorAxes(displayShapes + 0, a3geomFlag_wireframe, 0.0f, 1);
		a3proceduralCreateDescriptorPlane(displayShapes + 1, a3geomFlag_wireframe, a3geomAxis_default, 20.0f, 20.0f, 20, 20);
		for (i = 0; i < displayShapesCount; ++i)
		{
			a3proceduralGenerateGeometryData(displayShapesData + i, displayShapes + i, 0);
			a3fileStreamWriteObject(fileStream, displayShapesData + i, (a3_FileStreamWriteFunc)a3geometrySaveDataBinary);
		}

		// other procedurally-generated objects
		a3proceduralCreateDescriptorPlane(proceduralShapes + 0, a3geomFlag_tangents, a3geomAxis_default, 1.0f, 1.0f, 2, 2);
		a3proceduralCreateDescriptorBox(proceduralShapes + 1, a3geomFlag_tangents, 1.0f, 1.0f, 1.0f, 1, 1, 1);
		a3proceduralCreateDescriptorSphere(proceduralShapes + 2, a3geomFlag_tangents, a3geomAxis_default, 1.0f, 32, 24);
		a3proceduralCreateDescriptorCylinder(proceduralShapes + 3, a3geomFlag_tangents, a3geomAxis_default, 1.0f, 1.0f, 32, 4, 4);
		a3proceduralCreateDescriptorCapsule(proceduralShapes + 4, a3geomFlag_tangents, a3geomAxis_default, 1.0f, 1.0f, 32, 12, 4);
		a3proceduralCreateDescriptorTorus(proceduralShapes + 5, a3geomFlag_tangents, a3geomAxis_default, 1.0f, 0.25f, 32, 24);
		a3proceduralCreateDescriptorCone(proceduralShapes + 6, a3geomFlag_tangents, a3geomAxis_default, 1.0f, 1.0, 32, 1, 1);
		for (i = 0; i < proceduralShapesCount; ++i)
		{
			a3proceduralGenerateGeometryData(proceduralShapesData + i, proceduralShapes + i, 0);
			a3fileStreamWriteObject(fileStream, proceduralShapesData + i, (a3_FileStreamWriteFunc)a3geometrySaveDataBinary);
		}

		// objects loaded from mesh files
		for (i = 0; i < loadedModelsCount; ++i)
		{
			a3modelLoadOBJ(loadedModelsData + i, loadedShapes[i].modelFilePath, loadedShapes[i].flag, loadedShapes[i].transform);
			a3fileStreamWriteObject(fileStream, loadedModelsData + i, (a3_FileStreamWriteFunc)a3geometrySaveDataBinary);
		}

		// done
		a3fileStreamClose(fileStream);
	}


	// GPU data upload process: 
	//	- determine storage requirements
	//	- allocate buffer
	//	- create vertex arrays using unique formats
	//	- create drawable and upload data

	// get storage size
	sharedVertexStorage = numVerts = 0;
	for (i = 0; i < displayShapesCount; ++i)
	{
		sharedVertexStorage += a3geometryGetVertexBufferSize(displayShapesData + i);
		numVerts += displayShapesData[i].numVertices;
	}
	for (i = 0; i < proceduralShapesCount; ++i)
	{
		sharedVertexStorage += a3geometryGetVertexBufferSize(proceduralShapesData + i);
		numVerts += proceduralShapesData[i].numVertices;
	}
	for (i = 0; i < loadedModelsCount; ++i)
	{
		sharedVertexStorage += a3geometryGetVertexBufferSize(loadedModelsData + i);
		numVerts += loadedModelsData[i].numVertices;
	}


	// common index format required for shapes that share vertex formats
	a3geometryCreateIndexFormat(sceneCommonIndexFormat, numVerts);
	sharedIndexStorage = 0;
	for (i = 0; i < displayShapesCount; ++i)
		sharedIndexStorage += a3indexFormatGetStorageSpaceRequired(sceneCommonIndexFormat, displayShapesData[i].numIndices);
	for (i = 0; i < proceduralShapesCount; ++i)
		sharedIndexStorage += a3indexFormatGetStorageSpaceRequired(sceneCommonIndexFormat, proceduralShapesData[i].numIndices);
	for (i = 0; i < loadedModelsCount; ++i)
		sharedIndexStorage += a3indexFormatGetStorageSpaceRequired(sceneCommonIndexFormat, loadedModelsData[i].numIndices);

	// create shared buffer
	vbo_ibo = renderer->vbo_staticSceneObjectDrawBuffer;
	a3bufferCreateSplit(vbo_ibo, "vbo/ibo:scene", a3buffer_vertex, sharedVertexStorage, sharedIndexStorage, 0, 0);
	sharedVertexStorage = 0;


	// create vertex formats and drawables
	// axes: position and color
	vao = renderer->vao_position_color;
	a3geometryGenerateVertexArray(vao, "vao:pos+col", displayShapesData + 0, vbo_ibo, sharedVertexStorage);
	currentDrawable = renderer->draw_axes;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, displayShapesData + 0, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);

	// grid: position attribute only
	// overlay objects are also just position
	vao = renderer->vao_position;
	a3geometryGenerateVertexArray(vao, "vao:pos", displayShapesData + 1, vbo_ibo, sharedVertexStorage);
	currentDrawable = renderer->draw_grid;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, displayShapesData + 1, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);

	// models
	vao = renderer->vao_tangentbasis_texcoord;
	a3geometryGenerateVertexArray(vao, "vao:tb+tc", loadedModelsData + 0, vbo_ibo, sharedVertexStorage);
	currentDrawable = renderer->draw_teapot;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, loadedModelsData + 0, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	currentDrawable = renderer->draw_unit_plane_z;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, proceduralShapesData + 0, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	currentDrawable = renderer->draw_unit_box;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, proceduralShapesData + 1, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	currentDrawable = renderer->draw_unit_sphere;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, proceduralShapesData + 2, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	currentDrawable = renderer->draw_unit_cylinder;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, proceduralShapesData + 3, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	currentDrawable = renderer->draw_unit_capsule;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, proceduralShapesData + 4, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	currentDrawable = renderer->draw_unit_torus;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, proceduralShapesData + 5, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	currentDrawable = renderer->draw_unit_cone;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, proceduralShapesData + 6, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);


	// release data when done
	for (i = 0; i < displayShapesCount; ++i)
		a3geometryReleaseData(displayShapesData + i);
	for (i = 0; i < proceduralShapesCount; ++i)
		a3geometryReleaseData(proceduralShapesData + i);
	for (i = 0; i < loadedModelsCount; ++i)
		a3geometryReleaseData(loadedModelsData + i);
}


// utility to load shaders
void renderer_loadShaders(sRenderer *renderer)
{
	// structure to help with shader management
	typedef struct a3_TAG_DEMOSTATESHADER {
		a3_Shader shader[1];
		a3byte shaderName[32];

		a3_ShaderType shaderType;
		a3ui32 srcCount;
		const a3byte* filePath[8];	// max number of source files per shader
	} sRendererShader;

	// direct to demo programs
	sRendererProgram *currentDemoProg;
	a3i32 flag;
	a3ui32 i, j;

	// maximum uniform buffer size
	const a3ui32 uBlockSzMax = a3shaderUniformBlockMaxSize();

	// some default uniform values
	const a3f32 defaultFloat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const a3f64 defaultDouble[] = { 0.0, 0.0, 0.0, 1.0 };
	const a3i32 defaultInt[] = { 0, 0, 0, 1 };
	const a3i32 defaultTexUnits[] = {
		a3tex_unit00, a3tex_unit01, a3tex_unit02, a3tex_unit03,
		a3tex_unit04, a3tex_unit05, a3tex_unit06, a3tex_unit07,
		a3tex_unit08, a3tex_unit09, a3tex_unit10, a3tex_unit11,
		a3tex_unit12, a3tex_unit13, a3tex_unit14, a3tex_unit15
	};

	// FSQ matrix
	const a3mat4 fsq = {
		2.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 2.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};


	// list of all unique shaders
	// this is a good idea to avoid multi-loading 
	//	those that are shared between programs
	union {
		struct {
			// vertex shaders
			// base
			sRendererShader
				passthru_transform_vs[1],
				passColor_transform_vs[1],
				passthru_transform_instanced_vs[1],
				passColor_transform_instanced_vs[1];
			sRendererShader
				passTexcoord_transform_vs[1],
				passTangentBasis_transform_vs[1],
				passTexcoord_transform_instanced_vs[1],
				passTangentBasis_transform_instanced_vs[1];

			// fragment shaders
			// base
			sRendererShader
				drawColorUnif_fs[1],
				drawColorAttrib_fs[1];
			sRendererShader
				drawTexture_fs[1],
				drawLambert_fs[1],
				drawPhong_fs[1];
		};
	} shaderList = {
		{
			// vs
			// base
			{ { { 0 },	"shdr-vs:passthru-trans",			a3shader_vertex  ,	1,{ A3_DEMO_VS"passthru_transform_vs4x.glsl" } } },
			{ { { 0 },	"shdr-vs:pass-col-trans",			a3shader_vertex  ,	1,{ A3_DEMO_VS"passColor_transform_vs4x.glsl" } } },
			{ { { 0 },	"shdr-vs:passthru-trans-inst",		a3shader_vertex  ,	1,{ A3_DEMO_VS"passthru_transform_instanced_vs4x.glsl" } } },
			{ { { 0 },	"shdr-vs:pass-col-trans-inst",		a3shader_vertex  ,	1,{ A3_DEMO_VS"passColor_transform_instanced_vs4x.glsl" } } },
			// 00-common
			{ { { 0 },	"shdr-vs:pass-tex-trans",			a3shader_vertex  ,	1,{ A3_DEMO_VS"00-common/passTexcoord_transform_vs4x.glsl" } } },
			{ { { 0 },	"shdr-vs:pass-tb-trans",			a3shader_vertex  ,	1,{ A3_DEMO_VS"00-common/passTangentBasis_transform_vs4x.glsl" } } },
			{ { { 0 },	"shdr-vs:pass-tex-trans-inst",		a3shader_vertex  ,	1,{ A3_DEMO_VS"00-common/passTexcoord_transform_instanced_vs4x.glsl" } } },
			{ { { 0 },	"shdr-vs:pass-tb-trans-inst",		a3shader_vertex  ,	1,{ A3_DEMO_VS"00-common/passTangentBasis_transform_instanced_vs4x.glsl" } } },

			// fs
			// base
			{ { { 0 },	"shdr-fs:draw-col-unif",			a3shader_fragment,	1,{ A3_DEMO_FS"drawColorUnif_fs4x.glsl" } } },
			{ { { 0 },	"shdr-fs:draw-col-attr",			a3shader_fragment,	1,{ A3_DEMO_FS"drawColorAttrib_fs4x.glsl" } } },
			// 00-common
			{ { { 0 },	"shdr-fs:draw-tex",					a3shader_fragment,	1,{ A3_DEMO_FS"00-common/drawTexture_fs4x.glsl" } } },
			{ { { 0 },	"shdr-fs:draw-Lambert",				a3shader_fragment,	2,{ A3_DEMO_FS"00-common/drawLambert_fs4x.glsl",
																					A3_DEMO_FS"00-common/utilCommon_fs4x.glsl",} } },
			{ { { 0 },	"shdr-fs:draw-Phong",				a3shader_fragment,	2,{ A3_DEMO_FS"00-common/drawPhong_fs4x.glsl",
																					A3_DEMO_FS"00-common/utilCommon_fs4x.glsl",} } },
		}
	};
	sRendererShader *const shaderListPtr = (sRendererShader *)(&shaderList), *shaderPtr;
	const a3ui32 numUniqueShaders = sizeof(shaderList) / sizeof(sRendererShader);


	printf("\n\n---------------- LOAD SHADERS STARTED  ---------------- \n");


	// load unique shaders: 
	//	- load file contents
	//	- create and compile shader object
	//	- release file contents
	for (i = 0; i < numUniqueShaders; ++i)
	{
		shaderPtr = shaderListPtr + i;
		flag = a3shaderCreateFromFileList(shaderPtr->shader,
			shaderPtr->shaderName, shaderPtr->shaderType,
			shaderPtr->filePath, shaderPtr->srcCount);
		if (flag == 0)
			printf("\n ^^^^ SHADER %u '%s' FAILED TO COMPILE \n\n", i, shaderPtr->shader->handle->name);
	}


	// setup programs: 
	//	- create program object
	//	- attach shader objects

	// base programs: 
	// transform-only program
	currentDemoProg = renderer->prog_transform;
	a3shaderProgramCreate(currentDemoProg->program, "prog:transform");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passthru_transform_vs->shader);
	// transform-only program with instancing
	currentDemoProg = renderer->prog_transform_instanced;
	a3shaderProgramCreate(currentDemoProg->program, "prog:transform-inst");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passthru_transform_instanced_vs->shader);
	// uniform color program
	currentDemoProg = renderer->prog_drawColorUnif;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-col-unif");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passthru_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawColorUnif_fs->shader);
	// color attrib program
	currentDemoProg = renderer->prog_drawColorAttrib;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-col-attr");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passColor_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawColorAttrib_fs->shader);
	// uniform color program with instancing
	currentDemoProg = renderer->prog_drawColorUnif_instanced;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-col-unif-inst");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passthru_transform_instanced_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawColorUnif_fs->shader);
	// color attrib program with instancing
	currentDemoProg = renderer->prog_drawColorAttrib_instanced;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-col-attr-inst");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passColor_transform_instanced_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawColorAttrib_fs->shader);

	// 00-common programs: 
	// texturing
	currentDemoProg = renderer->prog_drawTexture;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-tex");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passTexcoord_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawTexture_fs->shader);
	// texturing with instancing
	currentDemoProg = renderer->prog_drawTexture_instanced;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-tex-inst");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passTexcoord_transform_instanced_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawTexture_fs->shader);
	// Lambert
	currentDemoProg = renderer->prog_drawLambert;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-Lambert");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passTangentBasis_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawLambert_fs->shader);
	// Lambert with instancing
	currentDemoProg = renderer->prog_drawLambert_instanced;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-Lambert-inst");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passTangentBasis_transform_instanced_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawLambert_fs->shader);
	// Phong
	currentDemoProg = renderer->prog_drawPhong;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-Phong");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passTangentBasis_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawPhong_fs->shader);
	// Phong with instancing
	currentDemoProg = renderer->prog_drawPhong_instanced;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-Phong-inst");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passTangentBasis_transform_instanced_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawPhong_fs->shader);


	// activate a primitive for validation
	// makes sure the specified geometry can draw using programs
	// good idea to activate the drawable with the most attributes
	a3vertexDrawableActivate(renderer->draw_axes);

	// link and validate all programs
	for (i = 0; i < rendererMaxCount_shaderProgram; ++i)
	{
		currentDemoProg = renderer->shaderProgram + i;
		flag = a3shaderProgramLink(currentDemoProg->program);
		if (flag == 0)
			printf("\n ^^^^ PROGRAM %u '%s' FAILED TO LINK \n\n", i, currentDemoProg->program->handle->name);

		flag = a3shaderProgramValidate(currentDemoProg->program);
		if (flag == 0)
			printf("\n ^^^^ PROGRAM %u '%s' FAILED TO VALIDATE \n\n", i, currentDemoProg->program->handle->name);
	}

	// if linking fails, contingency plan goes here
	// otherwise, release shaders
	for (i = 0; i < numUniqueShaders; ++i)
	{
		shaderPtr = shaderListPtr + i;
		a3shaderRelease(shaderPtr->shader);
	}


	// prepare uniforms algorithmically instead of manually for all programs
	// get uniform and uniform block locations and set default values for all 
	//	programs that have a uniform that will either never change or is
	//	consistent for all programs
	for (i = 0; i < rendererMaxCount_shaderProgram; ++i)
	{
		// activate program
		currentDemoProg = renderer->shaderProgram + i;
		a3shaderProgramActivate(currentDemoProg->program);

		// common VS
		renderer_setUniformDefaultMat4(currentDemoProg, uMVP);
		renderer_setUniformDefaultMat4(currentDemoProg, uMV);
		renderer_setUniformDefaultMat4(currentDemoProg, uP);
		renderer_setUniformDefaultMat4(currentDemoProg, uP_inv);
		renderer_setUniformDefaultMat4(currentDemoProg, uPB);
		renderer_setUniformDefaultMat4(currentDemoProg, uPB_inv);
		renderer_setUniformDefaultMat4(currentDemoProg, uMV_nrm);
		renderer_setUniformDefaultMat4(currentDemoProg, uMVPB);
		renderer_setUniformDefaultMat4(currentDemoProg, uMVPB_other);
		renderer_setUniformDefaultMat4(currentDemoProg, uAtlas);

		// common TS
		renderer_setUniformDefaultInteger(currentDemoProg, uLevelInner, defaultInt);
		renderer_setUniformDefaultInteger(currentDemoProg, uLevelOuter, defaultInt);

		// common texture
		renderer_setUniformDefaultInteger(currentDemoProg, uTex_dm, defaultTexUnits + 0);
		renderer_setUniformDefaultInteger(currentDemoProg, uTex_sm, defaultTexUnits + 1);
		renderer_setUniformDefaultInteger(currentDemoProg, uTex_nm, defaultTexUnits + 2);
		renderer_setUniformDefaultInteger(currentDemoProg, uTex_hm, defaultTexUnits + 3);
		renderer_setUniformDefaultInteger(currentDemoProg, uTex_ramp_dm, defaultTexUnits + 4);
		renderer_setUniformDefaultInteger(currentDemoProg, uTex_ramp_sm, defaultTexUnits + 5);
		renderer_setUniformDefaultInteger(currentDemoProg, uTex_shadow, defaultTexUnits + 6);
		renderer_setUniformDefaultInteger(currentDemoProg, uTex_project, defaultTexUnits + 7);
		renderer_setUniformDefaultInteger(currentDemoProg, uImage00, defaultTexUnits + 0);
		renderer_setUniformDefaultInteger(currentDemoProg, uImage01, defaultTexUnits + 1);
		renderer_setUniformDefaultInteger(currentDemoProg, uImage02, defaultTexUnits + 2);
		renderer_setUniformDefaultInteger(currentDemoProg, uImage03, defaultTexUnits + 3);
		renderer_setUniformDefaultInteger(currentDemoProg, uImage04, defaultTexUnits + 4);
		renderer_setUniformDefaultInteger(currentDemoProg, uImage05, defaultTexUnits + 5);
		renderer_setUniformDefaultInteger(currentDemoProg, uImage06, defaultTexUnits + 6);
		renderer_setUniformDefaultInteger(currentDemoProg, uImage07, defaultTexUnits + 7);

		// common general
		renderer_setUniformDefaultInteger(currentDemoProg, uIndex, defaultInt);
		renderer_setUniformDefaultInteger(currentDemoProg, uCount, defaultInt);
		renderer_setUniformDefaultDouble(currentDemoProg, uAxis, defaultDouble);
		renderer_setUniformDefaultDouble(currentDemoProg, uSize, defaultDouble);
		renderer_setUniformDefaultDouble(currentDemoProg, uFlag, defaultDouble);
		renderer_setUniformDefaultDouble(currentDemoProg, uTime, defaultDouble);
		renderer_setUniformDefaultVec4(currentDemoProg, uColor0, a3vec4_one.v);
		renderer_setUniformDefaultVec4(currentDemoProg, uColor, a3vec4_one.v);

		// transformation uniform blocks
		renderer_setUniformDefaultBlock(currentDemoProg, ubTransformStack, renderer_program_blockTransformStack);
		renderer_setUniformDefaultBlock(currentDemoProg, ubTransformBlend, renderer_program_blockTransformBlend);
		renderer_setUniformDefaultBlock(currentDemoProg, ubTransformMVP, renderer_program_blockTransformStack);
		renderer_setUniformDefaultBlock(currentDemoProg, ubTransformMVPB, renderer_program_blockTransformBlend);

		// lighting and shading uniform blocks
		renderer_setUniformDefaultBlock(currentDemoProg, ubMaterial, renderer_program_blockMaterial);
		renderer_setUniformDefaultBlock(currentDemoProg, ubLight, renderer_program_blockLight);
	}


	// allocate uniform buffers
	for (i = 0, j = rendererArrayLen(renderer->ubo_light); i < j; ++i)
		a3bufferCreate(renderer->ubo_light + i, "ubo:light", a3buffer_uniform, a3index_countMaxShort, 0);
	for (i = 0, j = rendererArrayLen(renderer->ubo_transform); i < j; ++i)
		a3bufferCreate(renderer->ubo_transform + i, "ubo:transform", a3buffer_uniform, a3index_countMaxShort, 0);


	printf("\n\n---------------- LOAD SHADERS FINISHED ---------------- \n");

	//done
	a3shaderProgramDeactivate();
	a3vertexDrawableDeactivate();
}


// utility to load textures
void renderer_loadTextures(sRenderer* renderer)
{	
	// indexing
	a3_Texture* tex;
	a3ui32 i;
	a3ui16 w, h;

	// structure for texture loading
	typedef struct a3_TAG_DEMOSTATETEXTURE {
		a3_Texture* texture;
		a3byte textureName[32];
		const a3byte* filePath;
	} sRendererTexture;

	// texture objects
	union {
		struct {
			sRendererTexture
				texSkyClouds[1],
				texSkyWater[1],
				texRampDM[1],
				texRampSM[1],
				texTestSprite[1],
				texChecker[1];
		};
	} textureList = {
		{
			{ renderer->tex_skybox_clouds,	"tex:sky-clouds",	A3_DEMO_TEX"bg/sky_clouds.png" },
			{ renderer->tex_skybox_water,	"tex:sky-water",	A3_DEMO_TEX"bg/sky_water.png" },
			{ renderer->tex_ramp_dm,		"tex:ramp-dm",		A3_DEMO_TEX"sprite/celRamp_dm.png" },
			{ renderer->tex_ramp_sm,		"tex:ramp-sm",		A3_DEMO_TEX"sprite/celRamp_sm.png" },
			{ renderer->tex_testsprite,	"tex:testsprite",	A3_DEMO_TEX"sprite/spriteTest8x8.png" },
			{ renderer->tex_checker,		"tex:checker",		A3_DEMO_TEX"sprite/checker.png" },
		}
	};
	const a3ui32 numTextures = sizeof(textureList) / sizeof(sRendererTexture);
	sRendererTexture* const textureListPtr = (sRendererTexture*)(&textureList), * texturePtr;

	// load all textures
	for (i = 0; i < numTextures; ++i)
	{
		texturePtr = textureListPtr + i;
		a3textureCreateFromFile(texturePtr->texture, texturePtr->textureName, texturePtr->filePath);
		a3textureActivate(texturePtr->texture, a3tex_unit00);
		a3textureDefaultSettings();
	}

	// change settings on a per-texture or per-type basis
	// atlases & skyboxes
	tex = renderer->tex_skybox_clouds;
	for (i = 0; i < 2; ++i, ++tex)
	{
		a3textureActivate(tex, a3tex_unit00);
		a3textureChangeFilterMode(a3tex_filterLinear); // linear pixel blending
	}
	// ramps
	tex = renderer->tex_ramp_dm;
	for (i = 0; i < 2; ++i, ++tex)
	{
		a3textureActivate(tex, a3tex_unit00);
		a3textureChangeRepeatMode(a3tex_repeatClamp, a3tex_repeatClamp); // clamp both axes
	}


	// texture atlas matrices
	tex = renderer->tex_checker;
	w = tex->width;
	h = tex->height;
	//renderer_setAtlasTransform_internal(renderer->atlas_earth.m, w, h, 0, 0, 1024, 512, 8, 8);
	//renderer_setAtlasTransform_internal(renderer->atlas_mars.m, w, h, 0, 544, 1024, 512, 8, 8);
	//renderer_setAtlasTransform_internal(renderer->atlas_moon.m, w, h, 0, 1088, 1024, 512, 8, 8);
	//renderer_setAtlasTransform_internal(renderer->atlas_marble.m, w, h, 1056, 0, 512, 512, 8, 8);
	//renderer_setAtlasTransform_internal(renderer->atlas_copper.m, w, h, 1056, 544, 512, 512, 8, 8);
	//renderer_setAtlasTransform_internal(renderer->atlas_stone.m, w, h, 1600, 0, 256, 256, 8, 8);
	//renderer_setAtlasTransform_internal(renderer->atlas_checker.m, w, h, 1888, 0, 128, 128, 8, 8);


	// done
	a3textureDeactivate(a3tex_unit00);
}


// utility to load framebuffers
void renderer_loadFramebuffers(sRenderer* renderer)
{
	// create framebuffers and change their texture settings if need be
	a3_Framebuffer* fbo;
	a3ui32 i, j;

	// frame sizes
	const a3ui16 frameWidth1 = renderer->frameWidth, frameHeight1 = renderer->frameHeight;
	const a3ui16 frameWidth2 = frameWidth1 / 2, frameHeight2 = frameHeight1 / 2;
	const a3ui16 frameWidth4 = frameWidth2 / 2, frameHeight4 = frameHeight2 / 2;
	const a3ui16 frameWidth8 = frameWidth4 / 2, frameHeight8 = frameHeight4 / 2;
	const a3ui16 shadowMapSize = 2048;

	// storage precision and targets
	const a3_FramebufferColorType colorType_scene = a3fbo_colorRGBA16;
	const a3_FramebufferDepthType depthType_scene = a3fbo_depth24_stencil8;
	const a3ui32 targets_scene = 1;
	const a3_FramebufferColorType colorType_composite = a3fbo_colorRGBA16;
	const a3_FramebufferDepthType depthType_composite = a3fbo_depthDisable;
	const a3ui32 targets_composite = 1;


	// initialize framebuffers: MRT, color and depth formats, size
	fbo = renderer->fbo_c16x4_d24s8;
	a3framebufferCreate(fbo, "fbo:c16x4;d24s8",
		4, a3fbo_colorRGBA16, a3fbo_depth24_stencil8,
		frameWidth1, frameHeight1);


	// change texture settings for all framebuffers
	for (i = 0, fbo = renderer->framebuffer;
		i < rendererMaxCount_framebuffer;
		++i, ++fbo)
	{
		// color, if applicable
		for (j = 0; j < fbo->color; ++j)
		{
			a3framebufferBindColorTexture(fbo, a3tex_unit00, j);
			a3textureChangeRepeatMode(a3tex_repeatClamp, a3tex_repeatClamp);
			a3textureChangeFilterMode(a3tex_filterLinear);
		}

		// depth, if applicable
		if (fbo->depthStencil)
		{
			a3framebufferBindDepthTexture(fbo, a3tex_unit00);
			a3textureChangeRepeatMode(a3tex_repeatClamp, a3tex_repeatClamp);
			a3textureChangeFilterMode(a3tex_filterLinear);
		}
	}


	// deactivate texture
	a3textureDeactivate(a3tex_unit00);
}


//-----------------------------------------------------------------------------

// internal utility for refreshing drawable
inline void a3_refreshDrawable_internal(a3_VertexDrawable *drawable, a3_VertexArrayDescriptor *vertexArray, a3_IndexBuffer *indexBuffer)
{
	drawable->vertexArray = vertexArray;
	if (drawable->indexType)
		drawable->indexBuffer = indexBuffer;
}


// the handle release callbacks are no longer valid; since the library was 
//	reloaded, old function pointers are out of scope!
// could reload everything, but that would mean rebuilding GPU data...
//	...or just set new function pointers!
void renderer_loadValidate(sRenderer* renderer)
{
	a3_BufferObject* currentBuff = renderer->drawDataBuffer,
		* const endBuff = currentBuff + rendererMaxCount_drawDataBuffer;
	a3_VertexArrayDescriptor* currentVAO = renderer->vertexArray,
		* const endVAO = currentVAO + rendererMaxCount_vertexArray;
	sRendererProgram* currentProg = renderer->shaderProgram,
		* const endProg = currentProg + rendererMaxCount_shaderProgram;
	a3_UniformBuffer* currentUBO = renderer->uniformBuffer,
		* const endUBO = currentUBO + rendererMaxCount_uniformBuffer;
	a3_Texture* currentTex = renderer->texture,
		* const endTex = currentTex + rendererMaxCount_texture;
	a3_Framebuffer* currentFBO = renderer->framebuffer,
		* const endFBO = currentFBO + rendererMaxCount_framebuffer;

	// set pointers to appropriate release callback for different asset types
	while (currentBuff < endBuff)
		a3bufferHandleUpdateReleaseCallback(currentBuff++);
	while (currentVAO < endVAO)
		a3vertexArrayHandleUpdateReleaseCallback(currentVAO++);
	while (currentProg < endProg)
		a3shaderProgramHandleUpdateReleaseCallback((currentProg++)->program);
	while (currentUBO < endUBO)
		a3bufferHandleUpdateReleaseCallback(currentUBO++);
	while (currentTex < endTex)
		a3textureHandleUpdateReleaseCallback(currentTex++);
	while (currentFBO < endFBO)
		a3framebufferHandleUpdateReleaseCallback(currentFBO++);

	// re-link specific object pointers for different asset types
	currentBuff = renderer->vbo_staticSceneObjectDrawBuffer;

	currentVAO = renderer->vao_position_color;
	currentVAO->vertexBuffer = currentBuff;
	a3_refreshDrawable_internal(renderer->draw_axes, currentVAO, currentBuff);

	currentVAO = renderer->vao_position;
	currentVAO->vertexBuffer = currentBuff;
	a3_refreshDrawable_internal(renderer->draw_grid, currentVAO, currentBuff);

	currentVAO = renderer->vao_position_normal_texcoord;
	currentVAO->vertexBuffer = currentBuff;
	a3_refreshDrawable_internal(renderer->draw_unit_plane_z, currentVAO, currentBuff);
	a3_refreshDrawable_internal(renderer->draw_unit_box, currentVAO, currentBuff);
	a3_refreshDrawable_internal(renderer->draw_unit_sphere, currentVAO, currentBuff);
	a3_refreshDrawable_internal(renderer->draw_unit_cylinder, currentVAO, currentBuff);
	a3_refreshDrawable_internal(renderer->draw_unit_capsule, currentVAO, currentBuff);
	a3_refreshDrawable_internal(renderer->draw_unit_torus, currentVAO, currentBuff);
	a3_refreshDrawable_internal(renderer->draw_unit_cone, currentVAO, currentBuff);

	currentVAO = renderer->vao_tangentbasis_texcoord;
	currentVAO->vertexBuffer = currentBuff;
	a3_refreshDrawable_internal(renderer->draw_teapot, currentVAO, currentBuff);
}


//-----------------------------------------------------------------------------
