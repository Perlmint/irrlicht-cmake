// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
// orginally written by Christian Stehno, modified by Nikolaus Gebhardt

#include "COgreMeshFileLoader.h"
#include "os.h"
#include "SMeshBuffer.h"
#include "SAnimatedMesh.h"
#include "fast_atof.h"
#include <string.h>


namespace irr
{
namespace scene
{

// Main Chunks
const u16 COGRE_HEADER= 0x1000;
const u16 COGRE_MESH= 0x3000;

// sub chunks of COGRE_MESH
const u16 COGRE_SUBMESH= 0x4000;
const u16 COGRE_GEOMETRY= 0x5000;
const u16 COGRE_SKELETON_LINK= 0x6000;
const u16 COGRE_BONE_ASSIGNMENT= 0x7000;
const u16 COGRE_MESH_LOD= 0x8000;
const u16 COGRE_MESH_BOUNDS= 0x9000;
const u16 COGRE_MESH_SUBMESH_NAME_TABLE= 0xA000;
const u16 COGRE_MESH_EDGE_LISTS= 0xB000;

// sub chunks of COGRE_GEOMETRY
const u16 COGRE_GEOMETRY_VERTEX_DECLARATION= 0x5100;
const u16 COGRE_GEOMETRY_VERTEX_ELEMENT= 0x5110;
const u16 COGRE_GEOMETRY_VERTEX_BUFFER= 0x5200;
const u16 COGRE_GEOMETRY_VERTEX_BUFFER_DATA= 0x5210;

// sub chunks of COGRE_SUBMESH
const u16 COGRE_SUBMESH_OPERATION= 0x4010;
const u16 COGRE_SUBMESH_BONE_ASSIGNMENT= 0x4100;
const u16 COGRE_SUBMESH_TEXTURE_ALIAS= 0x4200;

//! Constructor
COgreMeshFileLoader::COgreMeshFileLoader(IMeshManipulator* manip,io::IFileSystem* fs, video::IVideoDriver* driver)
: FileSystem(fs), Driver(driver), Manipulator(manip), Mesh(0)
{
	if (FileSystem)
		FileSystem->grab();

	if (Driver)
		Driver->grab();
}



//! destructor
COgreMeshFileLoader::~COgreMeshFileLoader()
{
	clearMeshes();

	if (FileSystem)
		FileSystem->drop();

	if (Driver)
		Driver->drop();

	if (Mesh)
		Mesh->drop();
}



//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".bsp")
bool COgreMeshFileLoader::isALoadableFileExtension(const c8* filename)
{
	return strstr(filename, ".mesh")!=0;
}



//! creates/loads an animated mesh from the file.
//! \return Pointer to the created mesh. Returns 0 if loading failed.
//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
//! See IUnknown::drop() for more information.
IAnimatedMesh* COgreMeshFileLoader::createMesh(io::IReadFile* file)
{
	s16 id;

	file->seek(0);
	file->read(&id, 2);

	if (id == COGRE_HEADER)
		SwapEndian=false;
	else if (id == 0x0010)
		SwapEndian=true;
	else
		return false;
	ChunkData data;
	readString(file, data, Version);
	if (Version != "[MeshSerializer_v1.30]")
		return false;

	clearMeshes();
	if (Mesh)
		Mesh->drop();

	Mesh = new SMesh();
	setCurrentlyLoadingPath(file);
	loadMaterials(file);

	if (readChunk(file))
	{
		// success
		SAnimatedMesh* am = new SAnimatedMesh();
		am->Type = EAMT_3DS;

		for (s32 i=0; i<Mesh->getMeshBufferCount(); ++i)
			((SMeshBuffer*)Mesh->getMeshBuffer(i))->recalculateBoundingBox();

		Mesh->recalculateBoundingBox();

		am->addMesh(Mesh);
		am->recalculateBoundingBox();
		Mesh->drop();
		Mesh = 0;
        	return am;
	}

	Mesh->drop();
	Mesh = 0;

    return 0;
}


bool COgreMeshFileLoader::readChunk(io::IReadFile* file)
{
	while(file->getPos() < file->getSize())
	{
		ChunkData data;
		readChunkData(file, data);

		switch(data.header.id)
		{
		case COGRE_MESH:
			{
				Meshes.push_back(OgreMesh());
				readObjectChunk(file, data, Meshes.getLast());
				composeObject();
			}
			break;
		default:
			return true;
		}
	}

	return true;
}


bool COgreMeshFileLoader::readObjectChunk(io::IReadFile* file, ChunkData& parent, OgreMesh& mesh)
{
	readBool(file, parent, mesh.SkeletalAnimation);
	while ((parent.read < parent.header.length)&&(file->getPos() < file->getSize()))
	{
		ChunkData data;
		readChunkData(file, data);

		switch(data.header.id)
		{
			case COGRE_GEOMETRY:
			{
				readGeometry(file, data, mesh.Geometry);
			}
			break;
			case COGRE_SUBMESH: 
				mesh.SubMeshes.push_back(OgreSubMesh());
				readSubMesh(file, data, mesh.SubMeshes.getLast());
			break;
			case COGRE_MESH_BOUNDS: 
			{
				readVector(file, data, mesh.BBoxMinEdge);
				readVector(file, data, mesh.BBoxMaxEdge);
				readFloat(file, data, mesh.BBoxRadius);
			}
			break;
			case COGRE_SKELETON_LINK: 
			case COGRE_BONE_ASSIGNMENT: 
			case COGRE_MESH_LOD: 
			case COGRE_MESH_SUBMESH_NAME_TABLE:
			case COGRE_MESH_EDGE_LISTS:
				// ignore chunk
				file->seek(data.header.length-data.read, true);
				data.read += data.header.length-data.read;
				break;
			default:
				parent.read=parent.header.length;
				file->seek(-(int)sizeof(ChunkHeader), true);
				return true;
		}
		parent.read += data.read;
	}
	return true;
}


bool COgreMeshFileLoader::readGeometry(io::IReadFile* file, ChunkData& parent, OgreGeometry& geometry)
{
	readInt(file, parent, geometry.NumVertex);
	while(parent.read < parent.header.length)
	{
		ChunkData data;
		readChunkData(file, data);

		switch(data.header.id)
		{
		case COGRE_GEOMETRY_VERTEX_DECLARATION:
			readVertexDeclaration(file, data, geometry);
			break;
		case COGRE_GEOMETRY_VERTEX_BUFFER:
			readVertexBuffer(file, data, geometry);
			break;
		default:
			// ignore chunk
			file->seek(data.header.length-data.read, true);
			data.read += data.header.length-data.read;
		}
		parent.read += data.read;
	}
	return true;
}


bool COgreMeshFileLoader::readVertexDeclaration(io::IReadFile* file, ChunkData& parent, OgreGeometry& geometry)
{
	while(parent.read < parent.header.length)
	{
		ChunkData data;
		readChunkData(file, data);

		switch(data.header.id)
		{
		case COGRE_GEOMETRY_VERTEX_ELEMENT:
		{
			OgreVertexElement elem;
			readShort(file, data, elem.Source);
			readShort(file, data, elem.Type);
			readShort(file, data, elem.Semantic);
			readShort(file, data, elem.Offset);
			elem.Offset /= sizeof(f32);
			readShort(file, data, elem.Index);
			geometry.Elements.push_back(elem);
		}
			break;
		default:
			// ignore chunk
			file->seek(data.header.length-data.read, true);
			data.read += data.header.length-data.read;
		}
		parent.read += data.read;
	}
	return true;
}


bool COgreMeshFileLoader::readVertexBuffer(io::IReadFile* file, ChunkData& parent, OgreGeometry& geometry)
{
	OgreVertexBuffer buf;
	readShort(file, parent, buf.BindIndex);
	readShort(file, parent, buf.VertexSize);
	buf.VertexSize /= sizeof(f32);
	ChunkData data;
	readChunkData(file, data);

	if (data.header.id == COGRE_GEOMETRY_VERTEX_BUFFER_DATA)
	{
		buf.Data = new f32[geometry.NumVertex*buf.VertexSize];
		for (s32 i=0; i<geometry.NumVertex*buf.VertexSize; ++i)
			readFloat(file, data, buf.Data[i]);
	}

	geometry.Buffers.push_back(buf);
	parent.read += data.read;
	return true;
}


bool COgreMeshFileLoader::readSubMesh(io::IReadFile* file, ChunkData& parent, OgreSubMesh& subMesh)
{
	readString(file, parent, subMesh.Material);
	readBool(file, parent, subMesh.SharedVertices);

	s32 numIndices;
	readInt(file, parent, numIndices);
	subMesh.Indices.set_used(numIndices);

	readBool(file, parent, subMesh.Indices32Bit);

	if (subMesh.Indices32Bit)
		for (s32 i=0; i<numIndices; ++i)
			readInt(file, parent, subMesh.Indices[i]);
	else
		for (s32 i=0; i<numIndices; ++i)
		{
			u16 num;
			readShort(file, parent, num);
			subMesh.Indices[i]=num;
		}

	if (!subMesh.SharedVertices)
	{
		ChunkData data;
		readChunkData(file, data);

		if (data.header.id==COGRE_GEOMETRY)
		{
			readGeometry(file, data, subMesh.Geometry);
		}
		parent.read += data.read;
	}

	while(parent.read < parent.header.length)
	{
		ChunkData data;
		readChunkData(file, data);

		switch(data.header.id)
		{
		case COGRE_SUBMESH_OPERATION:
			readShort(file, data, subMesh.Operation);
			break;
		case COGRE_SUBMESH_TEXTURE_ALIAS:
		{
			core::stringc texture, alias;
			readString(file, data, texture);
			readString(file, data, alias);
			subMesh.TextureAliases.push_back(OgreTextureAlias(texture,alias));
		}
			break;
		case COGRE_SUBMESH_BONE_ASSIGNMENT:
			// currently ignore chunk
			file->seek(data.header.length-data.read, true);
			data.read += data.header.length-data.read;
			break;
		default:
			parent.read=parent.header.length;
			file->seek(-(int)sizeof(ChunkHeader), true);
			return true;
		}
		parent.read += data.read;
	}
	return true;
}


scene::SMeshBuffer* COgreMeshFileLoader::composeMeshBuffer(const core::array<s32>& indices, const OgreGeometry& geom, const core::stringc& material)
{
	scene::SMeshBuffer *mb=new scene::SMeshBuffer();
	for (u32 k=0; k<Materials.size(); ++k)
		if (material==Materials[k].Name)
		{
			mb->Material=Materials[k].Material;
			if (Materials[k].Filename.size())
			{
				mb->Material.Texture1 = Driver->getTexture(Materials[k].Filename.c_str());

				if (!mb->Material.Texture1)
				{
					// retry with relative path
					core::stringc relative = CurrentlyLoadingFromPath;
					relative += "\\";
					relative += Materials[k].Filename;
					mb->Material.Texture1 = Driver->getTexture(relative.c_str());
				}	
			}
			break;
		}
	mb->Indices.set_used(indices.size());
	for (u32 i=0; i<indices.size(); ++i)
		mb->Indices[i]=indices[i];

	mb->Vertices.set_used(geom.NumVertex);
	for (u32 i=0; i<geom.Elements.size(); ++i)
	{
		if (geom.Elements[i].Semantic==1) //Pos
		{
			for (u32 j=0; j<geom.Buffers.size(); ++j)
			{
				if (geom.Elements[i].Index==geom.Buffers[j].BindIndex)
				{
					u32 eSize=geom.Buffers[j].VertexSize;
					u32 ePos=geom.Elements[i].Offset;
					for (s32 k=0; k<geom.NumVertex; ++k)
					{
						mb->Vertices[k].Color=mb->Material.DiffuseColor;
						mb->Vertices[k].Pos.set(geom.Buffers[j].Data[ePos],geom.Buffers[j].Data[ePos+1],geom.Buffers[j].Data[ePos+2]);
						ePos += eSize;
					}
				}
			}
		}
		if (geom.Elements[i].Semantic==4) //Normal
		{
			for (u32 j=0; j<geom.Buffers.size(); ++j)
			{
				if (geom.Elements[i].Index==geom.Buffers[j].BindIndex)
				{
					u32 eSize=geom.Buffers[j].VertexSize;
					u32 ePos=geom.Elements[i].Offset;
					for (s32 k=0; k<geom.NumVertex; ++k)
					{
						mb->Vertices[k].Normal.set(geom.Buffers[j].Data[ePos],geom.Buffers[j].Data[ePos+1],geom.Buffers[j].Data[ePos+2]);
						ePos += eSize;
					}
				}
			}
		}
		if (geom.Elements[i].Semantic==7) //TexCoord
		{
			for (u32 j=0; j<geom.Buffers.size(); ++j)
			{
				if (geom.Elements[i].Index==geom.Buffers[j].BindIndex)
				{
					u32 eSize=geom.Buffers[j].VertexSize;
					u32 ePos=geom.Elements[i].Offset;
					for (s32 k=0; k<geom.NumVertex; ++k)
					{
						mb->Vertices[k].TCoords.set(geom.Buffers[j].Data[ePos],geom.Buffers[j].Data[ePos+1]);
						ePos += eSize;
					}
				}
			}
		}
	}
	return mb;
}


void COgreMeshFileLoader::composeObject(void)
{
	for (u32 i=0; i<Meshes.size(); ++i)
	{
		for (u32 j=0; j<Meshes[i].SubMeshes.size(); ++j)
		{
			SMeshBuffer* mb;
			if (Meshes[i].SubMeshes[j].SharedVertices)
				mb = composeMeshBuffer(Meshes[i].SubMeshes[j].Indices, Meshes[i].Geometry, Meshes[i].SubMeshes[j].Material);
			else
				mb = composeMeshBuffer(Meshes[i].SubMeshes[j].Indices, Meshes[i].SubMeshes[j].Geometry, Meshes[i].SubMeshes[j].Material);

			if (mb != 0)
			{
				Mesh->addMeshBuffer(mb);
				mb->drop();
			}
		}
	}
}


core::stringc COgreMeshFileLoader::getTextureFileName(const core::stringc& texture, 
						 core::stringc& model)
{
	s32 idx = -1;
	idx = model.findLast('/');

	if (idx == -1)
		idx = model.findLast('\\');

	if (idx == -1)
		return core::stringc();

	core::stringc p = model.subString(0, idx+1);
	p.append(texture);
	return p;
}


void COgreMeshFileLoader::getMaterialToken(io::IReadFile* file, core::stringc& token)
{
	c8 c=0;
	token = "";

	file->read(&c, sizeof(c8));
	while (isspace(c) && (file->getPos() < file->getSize()))
		file->read(&c, sizeof(c8));
	do
	{
		if (c=='/')
		{
			file->read(&c, sizeof(c8));
			if (c=='/')
			{
				while(c!='\n')
					file->read(&c, sizeof(c8));
			}
			else
			{
				token.append('/');
				if (isspace(c))
					return;
			}
		}
		token.append(c);
		file->read(&c, sizeof(c8));
	}
	while ((!isspace(c)) && (file->getPos() < file->getSize()));
}


void COgreMeshFileLoader::loadMaterials(io::IReadFile* meshFile)
{
	core::stringc token, filename = meshFile->getFileName();
	core::stringc material = filename.subString(0, filename.size()-4) + "material";
	io::IReadFile* file = FileSystem->createAndOpenFile(material.c_str());

	if (!file)
	{
		os::Printer::log("Could not load OGRE material", material.c_str());
		return;
	}

	getMaterialToken(file, token);

	while (file->getPos() < file->getSize())
	{
		Materials.push_back(OgreMaterial());
		OgreMaterial& mat = Materials.getLast();

		if (token != "material")
			return;
		getMaterialToken(file, mat.Name);
		getMaterialToken(file, token); //open brace
		getMaterialToken(file, token);
		while(token != "technique")
			getMaterialToken(file, token);
		getMaterialToken(file, token); //open brace
		getMaterialToken(file, token);
		while(token != "pass")
			getMaterialToken(file, token);
		getMaterialToken(file, token); //open brace
		getMaterialToken(file, token);
		u32 inBlocks=3;
		while(inBlocks)
		{
			if (token=="ambient")
			{
				getMaterialToken(file, token);
				if (token!="vertexcolour")
				{
					video::SColorf col;
					col.r=core::fast_atof(token.c_str());
					getMaterialToken(file, token);
					col.g=core::fast_atof(token.c_str());
					getMaterialToken(file, token);
					col.b=core::fast_atof(token.c_str());
					getMaterialToken(file, token);
					col.a=core::fast_atof(token.c_str());
					if ((col.r==0.0f)&&(col.g==0.0f)&&(col.b==0.0f))
						mat.Material.AmbientColor=video::SColor(255,255,255,255);
					else
						mat.Material.AmbientColor=col.toSColor();
				}
			}
			else if (token=="diffuse")
			{
				getMaterialToken(file, token);
				if (token!="vertexcolour")
				{
					video::SColorf col;
					col.r=core::fast_atof(token.c_str());
					getMaterialToken(file, token);
					col.g=core::fast_atof(token.c_str());
					getMaterialToken(file, token);
					col.b=core::fast_atof(token.c_str());
					getMaterialToken(file, token);
					col.a=core::fast_atof(token.c_str());
					if ((col.r==0.0f)&&(col.g==0.0f)&&(col.b==0.0f))
						mat.Material.DiffuseColor=video::SColor(255,255,255,255);
					else
						mat.Material.DiffuseColor=col.toSColor();
				}
			}
			else if (token=="specular")
			{
				getMaterialToken(file, token);
				if (token!="vertexcolour")
				{
					video::SColorf col;
					col.r=core::fast_atof(token.c_str());
					getMaterialToken(file, token);
					col.g=core::fast_atof(token.c_str());
					getMaterialToken(file, token);
					col.b=core::fast_atof(token.c_str());
					getMaterialToken(file, token);
					col.a=core::fast_atof(token.c_str());
					if ((col.r==0.0f)&&(col.g==0.0f)&&(col.b==0.0f))
						mat.Material.SpecularColor=video::SColor(255,255,255,255);
					else
						mat.Material.SpecularColor=col.toSColor();
					getMaterialToken(file, token);
					mat.Material.Shininess=core::fast_atof(token.c_str());
				}
			}
			else if (token=="emissive")
			{
				getMaterialToken(file, token);
				if (token!="vertexcolour")
				{
					video::SColorf col;
					col.r=core::fast_atof(token.c_str());
					getMaterialToken(file, token);
					col.g=core::fast_atof(token.c_str());
					getMaterialToken(file, token);
					col.b=core::fast_atof(token.c_str());
					getMaterialToken(file, token);
					col.a=core::fast_atof(token.c_str());
					if ((col.r==0.0f)&&(col.g==0.0f)&&(col.b==0.0f))
						mat.Material.EmissiveColor=video::SColor(255,255,255,255);
					else
						mat.Material.EmissiveColor=col.toSColor();
				}
			}
			else if (token=="lighting")
			{
				getMaterialToken(file, token);
				mat.Material.Lighting=(token=="on");
			}
			else if (token=="scene_blend")
			{
				getMaterialToken(file, token);
				if (token=="add")
					mat.Material.MaterialType=video::EMT_TRANSPARENT_ADD_COLOR;
				else if (token=="alpha_blend")
					mat.Material.MaterialType=video::EMT_TRANSPARENT_ALPHA_CHANNEL;
				else if (token=="colour_blend")
					mat.Material.MaterialType=video::EMT_TRANSPARENT_VERTEX_ALPHA;
			}
			else if (token=="texture_unit")
			{
				getMaterialToken(file, token); //open brace
				getMaterialToken(file, token);
				while(token != "}")
				{
					if (token=="texture")
					{
						getMaterialToken(file, mat.Filename);
					}
					getMaterialToken(file, token);
				}
			}
			getMaterialToken(file, token);
			if (token=="{")
				++inBlocks;
			else if (token=="}")
				--inBlocks;
		}
		getMaterialToken(file, token);
	}

	file->drop();
}


void COgreMeshFileLoader::readChunkData(io::IReadFile* file, ChunkData& data)
{
	file->read(&data.header, sizeof(ChunkHeader));
	if (SwapEndian)
	{
		data.header.id = bswap_16(data.header.id);
		data.header.length = bswap_32(data.header.length);
	}
	data.read += sizeof(ChunkHeader);
}


void COgreMeshFileLoader::readString(io::IReadFile* file, ChunkData& data, core::stringc& out)
{
	c8 c = 0;
	out = "";

	while (c!='\n')
	{
		file->read(&c, sizeof(c8));
		if (c!='\n')
			out.append(c);

	}
	data.read+=out.size()+1;
}


void COgreMeshFileLoader::readBool(io::IReadFile* file, ChunkData& data, bool& out)
{
	c8 c = 0;

	file->read(&c, sizeof(c8));
	out=(c!=0);
	++data.read;
}


void COgreMeshFileLoader::readInt(io::IReadFile* file, ChunkData& data, s32& out)
{
	file->read(&out, sizeof(s32));
	if (SwapEndian)
	{
		out = bswap_32(out);
	}
	data.read+=sizeof(s32);
}


void COgreMeshFileLoader::readShort(io::IReadFile* file, ChunkData& data, u16& out)
{
	file->read(&out, sizeof(u16));
	if (SwapEndian)
	{
		out = bswap_16(out);
	}
	data.read+=sizeof(u16);
}


void COgreMeshFileLoader::readFloat(io::IReadFile* file, ChunkData& data, f32& out)
{
	if (SwapEndian)
	{
		u32 tmp;
		file->read(&tmp, sizeof(f32));
		*((u32*)&out)=bswap_32(tmp);
	}
	else
		file->read(&out, sizeof(f32));
	data.read+=sizeof(f32);
}


void COgreMeshFileLoader::readVector(io::IReadFile* file, ChunkData& data, core::vector3df& out)
{
	readFloat(file, data, out.X);
	readFloat(file, data, out.Y);
	readFloat(file, data, out.Z);
}

void COgreMeshFileLoader::setCurrentlyLoadingPath(io::IReadFile* file)
{
	CurrentlyLoadingFromPath = file->getFileName();
	int idx = CurrentlyLoadingFromPath.findLast('/');

	if (idx != -1)
	{
		CurrentlyLoadingFromPath = CurrentlyLoadingFromPath.subString(0, idx);
	}
	else
	{
		idx = CurrentlyLoadingFromPath.findLast('\\');

		if (idx != -1)
			CurrentlyLoadingFromPath = CurrentlyLoadingFromPath.subString(0, idx);
	}
}

void COgreMeshFileLoader::clearMeshes()
{
	for (u32 i=0; i<Meshes.size(); ++i)
	{
		for (int h=0; h<(int)Meshes[i].Geometry.Buffers.size(); ++h)
			Meshes[i].Geometry.Buffers[h].destroy();

		for (u32 j=0; j<Meshes[i].SubMeshes.size(); ++j)
		{
			for (int h=0; h<(int)Meshes[i].SubMeshes[j].Geometry.Buffers.size(); ++h)
				Meshes[i].SubMeshes[j].Geometry.Buffers[h].destroy();
		}
	}

	Meshes.clear();
}


} // end namespace scene
} // end namespace irr

