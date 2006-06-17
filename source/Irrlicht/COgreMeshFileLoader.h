// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
// orginally written by Christian Stehno, modified by Nikolaus Gebhardt

#ifndef __C_OGRE_MESH_FILE_LOADER_H_INCLUDED__
#define __C_OGRE_MESH_FILE_LOADER_H_INCLUDED__

#include "IMeshLoader.h"
#include "IFileSystem.h"
#include "IVideoDriver.h"
#include "irrString.h"
#include "SMesh.h"
#include "SMeshBuffer.h"
#include "IMeshManipulator.h"
#include "matrix4.h"

namespace irr
{
namespace scene
{

//! Meshloader capable of loading 3ds meshes.
class COgreMeshFileLoader : public IMeshLoader
{
public:

	//! Constructor
	COgreMeshFileLoader(IMeshManipulator* manip,io::IFileSystem* fs, video::IVideoDriver* driver);

	//! destructor
	virtual ~COgreMeshFileLoader();

	//! returns true if the file maybe is able to be loaded by this class
	//! based on the file extension (e.g. ".cob")
	virtual bool isALoadableFileExtension(const c8* fileName);

	//! creates/loads an animated mesh from the file.
	//! \return Pointer to the created mesh. Returns 0 if loading failed.
	//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
	//! See IUnknown::drop() for more information.
	virtual IAnimatedMesh* createMesh(irr::io::IReadFile* file);

private:

	// byte-align structures
	#ifdef _MSC_VER
	#	pragma pack( push, packing )
	#	pragma pack( 1 )
	#	define PACK_STRUCT
	#elif defined( __GNUC__ )
	#	define PACK_STRUCT	__attribute__((packed))
	#else
	#	error compiler not supported
	#endif

	struct ChunkHeader
	{
		u16 id;
		u32 length;
	} PACK_STRUCT;

	// Default alignment
	#ifdef _MSC_VER
	#	pragma pack( pop, packing )
	#endif

	#undef PACK_STRUCT


	struct ChunkData
	{
		ChunkData() : read(0) {}

		ChunkHeader header;
		u32 read;
	};

	struct OgreMaterial
	{
		OgreMaterial() : Name(""), Filename("") {}

		video::SMaterial Material;
		core::stringc Name;
		core::stringc Filename;
	};

	struct OgreVertexBuffer
	{
		OgreVertexBuffer() : BindIndex(0), VertexSize(0), Data(0) {}
		void destroy() { delete [] Data; Data = 0; };

		u16 BindIndex,
		VertexSize;
		f32 *Data;
	};

	struct OgreVertexElement
	{
		u16 Source,
		Type,
		Semantic,
		Offset,
		Index;
	};

	struct OgreGeometry
	{
		s32 NumVertex;
		core::array<OgreVertexElement> Elements;
		core::array<OgreVertexBuffer> Buffers;
		core::array<core::vector3df> Vertices;
		core::array<core::vector3df> Normals;
		core::array<s32> Colors;
		core::array<core::vector2df> TexCoords;
	};

	struct OgreTextureAlias
	{
		OgreTextureAlias() {};
		OgreTextureAlias(const core::stringc& a, const core::stringc& b) : Texture(a), Alias(b) {};
		core::stringc Texture;
		core::stringc Alias;
	};

	struct OgreSubMesh
	{
		core::stringc Material;
		bool SharedVertices;
		core::array<s32> Indices;
		OgreGeometry Geometry;
		u16 Operation;
		core::array<OgreTextureAlias> TextureAliases;
		bool Indices32Bit;
	};

	struct OgreMesh
	{
		bool SkeletalAnimation;
		OgreGeometry Geometry;
		core::array<OgreSubMesh> SubMeshes;
		core::vector3df BBoxMinEdge;
		core::vector3df BBoxMaxEdge;
		f32 BBoxRadius;
	};

	bool readChunk(io::IReadFile* file);
	bool readObjectChunk(io::IReadFile* file, ChunkData& parent, OgreMesh& mesh);
	bool readGeometry(io::IReadFile* file, ChunkData& parent, OgreGeometry& geometry);
	bool readVertexDeclaration(io::IReadFile* file, ChunkData& parent, OgreGeometry& geometry);
	bool readVertexBuffer(io::IReadFile* file, ChunkData& parent, OgreGeometry& geometry);
	bool readSubMesh(io::IReadFile* file, ChunkData& parent, OgreSubMesh& subMesh);
	bool readPercentageChunk(io::IReadFile* file, ChunkData& chunk, float&percentage);

	void readChunkData(io::IReadFile* file, ChunkData& data);
	void readString(io::IReadFile* file, ChunkData& data, core::stringc& out);
	void readBool(io::IReadFile* file, ChunkData& data, bool& out);
	void readInt(io::IReadFile* file, ChunkData& data, s32& out);
	void readShort(io::IReadFile* file, ChunkData& data, u16& out);
	void readFloat(io::IReadFile* file, ChunkData& data, f32& out);
	void readVector(io::IReadFile* file, ChunkData& data, core::vector3df& out);

	scene::SMeshBuffer* composeMeshBuffer(const core::array<s32>& indices, const OgreGeometry& geom, const core::stringc& material);
	void composeObject(void);
	void getMaterialToken(io::IReadFile* file, core::stringc& token);
	void loadMaterials(io::IReadFile* file);
	core::stringc getTextureFileName(const core::stringc& texture, core::stringc& model);
	void setCurrentlyLoadingPath(io::IReadFile* file);
	void clearMeshes();

	inline bool isspace(c8 c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }
	inline u16 bswap_16(u16 c)   { return (c >> 8) | (c << 8); }
	inline u32 bswap_32(u32 c)   
	{
		c = (c >> 16) | (c << 16);
		c = ((c >> 8) & 0xFF00FF) | ((c << 8) & 0xFF00FF00);
		return c;
	}

	io::IFileSystem* FileSystem;
	video::IVideoDriver* Driver;

	core::stringc Version;
	bool SwapEndian;
	core::array<OgreMesh> Meshes;
	core::stringc CurrentlyLoadingFromPath;

	core::array<OgreMaterial> Materials;

	SMesh* Mesh;
	IMeshManipulator* Manipulator;
};

} // end namespace scene
} // end namespace irr

#endif

